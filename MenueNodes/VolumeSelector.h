#pragma once
#include <stdint.h>
#include "pico/binary_info.h"
#include "pico/stdlib.h"
#include "UINode.h"
#include "StepEncBase.h"
#include "LED_8SEG.h"
#include "VolChControl.h"

// ---------------------------------------------------------------------------
// Klasse zur Auswahl der Lautstärke über den Step-Encoder
// ---------------------------------------------------------------------------
class VolumeSelector : public UINode
{
    public:
    // Konstruktor: exitNode = Node, zu dem gewechselt wird, wenn der Button gedrückt wird
    VolumeSelector(UINode* exitNode, VolChControl* device);
    ~VolumeSelector() = default;

    // Implementierung der virtuellen Methoden der Basisklasse UINode
    virtual void run() override;
    void onEnter() override;
    void onUpdate() override;

    private:
    // Aktuelle Lautstärke (0-63)
    int16_t       m_volume;
    // Pointer auf die Instanz der VolChControl-Klasse, die die Lautstärke steuert
    VolChControl* m_device;
 
    // Anzeigeinhalt für die 4-stellige 7-Segment-Anzeige
    LED_8SEG::SegmentCode m_dispContent[4];

};