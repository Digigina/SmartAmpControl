#include "IRQ_Handler.h"
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/irq.h"
#include "pico/sem.h"
#include "hardware/pio.h"

class RemoteProtocol
{
    public:
    RemoteProtocol();
    bool checkIRSignal(uint8_t *key, uint8_t *address);

    PIO getPio() const { return m_pio; };
    uint getSM() const { return m_sm_inst; };
    semaphore_t* getSemaphore() { return &m_ir_semaphore; };

    public:
    volatile uint32_t m_ir_raw_data;

    private:
    PIO m_pio;
    uint m_sm_inst;
    uint m_offset_sm;
    semaphore_t m_ir_semaphore;

};