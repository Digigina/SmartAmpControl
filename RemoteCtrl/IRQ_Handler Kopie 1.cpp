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
    bool data_valid = false;
    if (pio_interrupt_get(g_remoteCtrl->getPio(), 0)) 
    {
        
        if (!pio_sm_is_rx_fifo_empty(g_remoteCtrl->getPio(), g_remoteCtrl->getSM()))
        {
            // Daten liegen im FIFO vor
            // 1. Daten aus dem FIFO holen und dem Haputprogramm übergeben
            g_remoteCtrl->m_ir_raw_data = pio_sm_get(g_remoteCtrl->getPio(), g_remoteCtrl->getSM());
            // 2. Semaphore freigeben -> Weckt den Haupt-Loop auf!
            sem_release(g_remoteCtrl->getSemaphore());
            data_valid = true;
        }

        // 3. Interrupt in der PIO quittieren
        pio_interrupt_clear(g_remoteCtrl->getPio(), 0);
    }
    if (pio_interrupt_get(g_remoteCtrl->getPio(), 1)) 
    {
        // Repeat Frame wurde empfangen
        if(data_valid == false)
        {
            // Nur Meldung ans Hauptprogramm, wenn nicht zuvor ein gültiges Signal empfangen wurde.
             g_remoteCtrl->m_ir_raw_data = 0x11; // Kennzeichnung für Repeat Frame
            // Semaphore freigeben -> Weckt den Haupt-Loop auf!
            sem_release(g_remoteCtrl->getSemaphore());
        }
 
        // 3. Interrupt in der PIO quittieren
        pio_interrupt_clear(g_remoteCtrl->getPio(), 1);
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
