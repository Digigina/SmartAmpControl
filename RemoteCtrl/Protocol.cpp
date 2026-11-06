#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/clocks.h"

#include "pico/sem.h"         // Wichtig für Semaphoren-Unterstützung

#include "Protocol.h"
#include "IRQ_Handler.h"
#include "nec_receive.pio.h"

constexpr uint in_pin = 15; // IR-Empfänger
constexpr uint out_pin = 13; // Testausgang

constexpr float PIO_FREQUENCY = 35000.0f; // 35 kHz => 28,57 µs pro Zyklus

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

     // WICHTIG: Queue initialisieren, BEVOR der PIO-Interrupt freigegeben wird,
    // sonst kann die ISR in eine nicht initialisierte Queue schreiben.
    queue_init(&m_ir_queue, sizeof(uint32_t), QUEUE_DEPTH);

    // Interrupt Konfiguration
    init_irq_handler(this);

    // State Machines starten
    pio_sm_set_enabled(m_pio, m_sm_inst, true);
}

// ---------------------------------------------------------------------------
void RemoteProtocol::enqueueFromIsr(uint32_t raw)
{
    // queue_try_add ist ISR-sicher (Spinlock) und blockiert nie
    if (!queue_try_add(&m_ir_queue, &raw))
    {
        m_dropped = m_dropped + 1;   // Queue voll -> Ereignis verwerfen
    }
}

// ---------------------------------------------------------------------------
void RemoteProtocol::onPioIrq()
{
    bool data_pushed = false;

    if (pio_interrupt_get(m_pio, 0))
    {
        // Gueltige Daten: FIFO komplett leeren (sm_get auf leerem FIFO wuerde blockieren)
        while (!pio_sm_is_rx_fifo_empty(m_pio, m_sm_inst))
        {
            enqueueFromIsr(pio_sm_get(m_pio, m_sm_inst));
            data_pushed = true;
        }
        pio_interrupt_clear(m_pio, 0);
    }

    if (pio_interrupt_get(m_pio, 1))
    {
        // Repeat-Frame nur melden, wenn im selben Durchlauf nicht schon Daten kamen
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

    // Nicht blockierend: nichts in der Queue -> sofort zurueck
    if (!queue_try_remove(&m_ir_queue, &raw))
    {
        return false;
    }

    if (raw == RAW_REPEAT_FRAME)
    {
        IR_LOG("[Main Loop] Repeat-Frame detected.\n");
        return false;
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
        if (address == m_necAdr)
        {
            IR_LOG("[Main Loop] Signal fuer uns: 0x%02X\n", key);
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
                    IR_LOG("[Main Loop] Unbekannter Tastencode: 0x%02X\n", key);
                    break;
            }
        }
        else
        {
            IR_LOG("[Main Loop] Signal fuer andere Adresse: 0x%02X\n", address);
        }
    }
}
