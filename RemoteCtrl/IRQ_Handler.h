#pragma once

class RemoteProtocol;

// Die Interrupt Service Routine (ISR)
void pio_irq_handler();
// Initialisiert die Interrupt-Konfiguration und registriert die ISR
void init_irq_handler(RemoteProtocol* pRemoteCtrl);
