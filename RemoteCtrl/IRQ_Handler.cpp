#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/irq.h"
#include "pico/sem.h"         // Wichtig für Semaphoren-Unterstützung
#include "hardware/pio.h"
#include "IRQ_Handler.h"
#include "Protocol.h" 

static RemoteProtocol* g_remoteCtrl = nullptr;

// Die Interrupt Service Routine (ISR)
void pio_irq_handler()
{
    if (g_remoteCtrl != nullptr)
    {
        g_remoteCtrl->onPioIrq();
    }
}


void init_irq_handler(RemoteProtocol* pRemoteCtrl)
{
    g_remoteCtrl = pRemoteCtrl;
    uint irq_num = (g_remoteCtrl->getPio() == pio0) ? PIO0_IRQ_0 : PIO1_IRQ_0;
    irq_set_exclusive_handler(irq_num, pio_irq_handler);
    irq_set_enabled(irq_num, true);
    pio_set_irq0_source_enabled(g_remoteCtrl->getPio(), pis_interrupt0, true);
    pio_set_irq0_source_enabled(g_remoteCtrl->getPio(), pis_interrupt1, true);
}
