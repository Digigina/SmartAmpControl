#pragma once

#include <cstdint>
#include "pico/stdlib.h"
#include "pico/util/queue.h"
#include "hardware/pio.h"
#include "hardware/irq.h"

#include "VolChControl.h"

// ---------------------------------------------------------------------------
// Klasse zur Einlesung der Fernbedienungssignale (NEC-Protokoll) über PIO
// Die Klasse ist für die Verwendung mit einer VolChControl-Instanz vorgesehen, die die
// empfangenen Signale verarbeitet (Lautstärke/Kanalumschaltung).
// ---------------------------------------------------------------------------
class RemoteProtocol
{
public:
    // Sonderwerte, die statt eines NEC-Frames in die Queue gelegt werden
    static constexpr uint32_t RAW_REPEAT_FRAME = 0xFFFFFFFF; // Repeat-Frame (IRQ-Flag 1)
    static constexpr uint32_t RAW_INVALID_LEAD = 22;         // vom PIO-Programm gepusht: ungueltiger Lead-Pulse

    // Anzahl Eintraege in der Queue (Tastendruecke, die zwischengepuffert werden koennen)
    static constexpr uint QUEUE_DEPTH = 8;

    // Konstruktor: initialisiert die PIO, die State Machine und die Queue
    RemoteProtocol(uint8_t necAdr, VolChControl* pVolChCtrl);

    // Holt maximal EIN Ereignis aus der Queue (nicht blockierend).
    // true  -> gueltiger Tastencode in key/address
    // false -> nichts da, Repeat-Frame oder ungueltiges Signal
    bool checkIRSignal(uint8_t *key, uint8_t *address);

    // Wird ausschliesslich aus dem PIO-Interrupt aufgerufen
    void onPioIrq();

    PIO  getPio() const { return m_pio; }
    uint getSM()  const { return m_sm_inst; }

    // Anzahl verworfener Ereignisse (Queue voll), nur zur Diagnose
    uint32_t getDroppedCount() const { return m_dropped; }

    // Wird in der Hauptschleife aufgerufen, um die empfangenen Signale zu verarbeiten
    void run();

private:
    void enqueueFromIsr(uint32_t raw);

private:
    PIO      m_pio;
    uint     m_sm_inst;
    uint     m_offset_sm;
    queue_t  m_ir_queue;
    volatile uint32_t m_dropped;

    VolChControl* m_pActor;
    uint8_t       m_necAdr;
};
