#pragma once

class RemoteProtocol;

// Die Interrupt Service Routine (ISR)
void pio_irq_handler();
void init_irq_handler(RemoteProtocol* pRemoteCtrl);
