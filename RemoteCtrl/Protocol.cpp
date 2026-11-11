#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/clocks.h"

#include "pico/sem.h"         // Wichtig für Semaphoren-Unterstützung

#include "Protocol.h"
#include "IRQ_Handler.h"
#include "nec_receive.pio.h"

constexpr uint in_pin = 15; // IR-Receiver
constexpr uint out_pin = 13; // Test-Output (for test purposes, can also be omitted)

constexpr float PIO_FREQUENCY = 35000.0f; // 35 kHz => 28,57 µs per cycle

constexpr uint32_t REPEAT_DELAY_US = 111000; // time span, in which Repeat-Frames are accepted

// Debug-Ausgaben ueber printf nur bei Bedarf aktivieren
// #define IR_DEBUG

#ifdef IR_DEBUG
    #define IR_LOG(...) printf(__VA_ARGS__)
#else
    #define IR_LOG(...) ((void)0)
#endif

RemoteProtocol::RemoteProtocol(uint8_t necAdr, VolChControl* pVolChCtrl) : m_pActor(pVolChCtrl), m_necAdr(necAdr)
{
    // Standard PIO & State Machine Setup
    m_pio = pio0;
    m_offset_sm = pio_add_program(m_pio, &nec_receive_program);
    m_sm_inst = pio_claim_unused_sm(m_pio, true);
    pio_set_gpio_base(m_pio, 0);

    float cpu_clk_freq_hz = static_cast<float>(clock_get_hz(clk_sys));
    float cycle_frequency = PIO_FREQUENCY; // cycle frequency for PIO state machine 35.kHz
    float div = cpu_clk_freq_hz / cycle_frequency;

     pio_sm_config c = nec_receive_program_get_default_config(m_offset_sm);
    
    // IN-Pin 
    sm_config_set_in_pins(&c, in_pin);
    sm_config_set_jmp_pin(&c, in_pin);
    sm_config_set_in_shift(&c, true, false, 32);
    
    pio_gpio_init(m_pio, in_pin);
    pio_sm_set_consecutive_pindirs(m_pio, m_sm_inst, in_pin, 1, false);
    
    sm_config_set_clkdiv(&c, div);
    pio_sm_init(m_pio, m_sm_inst, m_offset_sm, &c);

     // IMPORTANT: initialise Queue, BEVOR the PIO-Interrupt is enabled,
    // otherwise the ISR might write to an uninitialised Queue.
    queue_init(&m_ir_queue, sizeof(uint32_t), QUEUE_DEPTH);

    // Interrupt Configuration
    init_irq_handler(this);

    // State Machines starten
    pio_sm_set_enabled(m_pio, m_sm_inst, true);
}

// ---------------------------------------------------------------------------
void RemoteProtocol::enqueueFromIsr(uint32_t raw)
{
    // queue_try_add is ISR-save (Spinlock) and never blocks, so it can be called from the ISR context.
    if (!queue_try_add(&m_ir_queue, &raw))
    {
        m_dropped = m_dropped + 1;   // queue full -> omit Event
    }
}

// ---------------------------------------------------------------------------
void RemoteProtocol::onPioIrq()
{
    bool data_pushed = false;

    if (pio_interrupt_get(m_pio, 0))
    {
        // valid data: dump FIFO completely (sm_get on empty FIFO would blocking)
        while (!pio_sm_is_rx_fifo_empty(m_pio, m_sm_inst))
        {
            enqueueFromIsr(pio_sm_get(m_pio, m_sm_inst));
            data_pushed = true;
        }
        pio_interrupt_clear(m_pio, 0);
    }

    if (pio_interrupt_get(m_pio, 1))
    {
        // Repeat-Frame only report if in the same loop no data came from the FIFO 
        // (otherwise the Repeat-Frame is already included in the FIFO data)
        if (!data_pushed)
        {
            enqueueFromIsr(RAW_REPEAT_FRAME);
        }
        pio_interrupt_clear(m_pio, 1);
    }
}

// ---------------------------------------------------------------------------
bool RemoteProtocol::checkIRSignal(uint8_t *key, uint8_t *address)
{
    uint32_t raw;

    // none blocking, if queue is empty -> return immediately
    if (!queue_try_remove(&m_ir_queue, &raw))
    {
        return false;
    }

    if (raw == RAW_REPEAT_FRAME)
    {
        IR_LOG("[Main Loop] Repeat-Frame detected.\n");
        *address = 0xff;
        *key = 0xff;
        return true;
    }

    if (raw == RAW_INVALID_LEAD)
    {
        IR_LOG("[Main Loop] Invalid Lead Pulse.\n");
        return false;
    }

    // NEC-Paket zerlegen
    *address        = (raw >> 0)  & 0xFF;
    *key            = (raw >> 16) & 0xFF;
    uint8_t key_inv = (raw >> 24) & 0xFF;

    if ((*key ^ key_inv) == 0xFF)
    {
        IR_LOG("[Main Loop] Adresse: 0x%02X | Scancode: 0x%02X\n", *address, *key);
        return true;
    }

    IR_LOG("[Main Loop] Signal ungueltig 0x%08lX\n", (unsigned long)raw);
    return false;
}

void RemoteProtocol::run()
{
    uint8_t key, address;

    if (checkIRSignal(&key, &address))
    {
        if (address == 0xff && key == 0xff) // Repeat-Frame
        {
            if (m_repeatEnable && time_us_64() < m_repeatTimeOut_us)
            {
                IR_LOG("[Main Loop] Repeat-Frame accepted.\n");
                m_repeatTimeOut_us = time_us_64() + REPEAT_DELAY_US;
                address = m_necAdr; // Repeat-Frame -> use last Adresse
                key = m_lastKey; // Repeat-Frame -> use last Key
                m_repeatCount++;
                if(m_repeatCount < 3)
                {
                    // threshold to ensure single steps -> SLOW DOWN
                    return;
                }
            }
            else
            {
                m_repeatEnable = false;
                IR_LOG("[Main Loop] Repeat-Frame ignored.\n");
                return;
            }
        }
        else
        {
            m_repeatCount = 0; // reset repeat counter
        }
        if (address == m_necAdr)
        {
            IR_LOG("[Main Loop] Signal fuer uns: 0x%02X\n", key);
            m_repeatEnable = true;
            m_repeatTimeOut_us = time_us_64() + REPEAT_DELAY_US;
            m_lastKey = key; // store last key for Repeat-Frame
            switch (key)
            {
                case 0x14: // Vol+
                    m_pActor->setVolume(m_pActor->getVolume() + 1);
                    break;
                case 0x18: // Vol-
                    m_pActor->setVolume(m_pActor->getVolume() - 1);
                    break;
                case 0x15: // Input (Output on IR-Transmitter)
                    m_pActor->setChannel(m_pActor->getChannel() + 1);
                    break;
                 default:
                    IR_LOG("[Main Loop] Unknown key code: 0x%02X\n", key);
                    m_lastKey = 0; // reset last key, so that Repeat-Frame is ignored
                    m_repeatEnable = false;
                    break;
            }
        }
        else
        {
            IR_LOG("[Main Loop] Signal for foreign Adresse: 0x%02X\n", address);
        }
    }
    if (m_repeatEnable && time_us_64() >= m_repeatTimeOut_us)
    {
        // time out -> don't accept Repeat-Frame anymore
        m_repeatEnable = false;
    }
}
