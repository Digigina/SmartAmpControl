#pragma once
#include <stdint.h>
#include "pico/binary_info.h"
#include "pico/stdlib.h"
#include "UINode.h"
#include "StepEncBase.h"
#include "LED_8SEG.h"

// ---------------------------------------------------------------------------
// Klasse zur Auswahl der Lautstärke und des Kanals über den Step-Encoder
// 
class VolChSelector : public UINode
{
    public:
    VolChSelector(UINode* exitNode);
    ~VolChSelector() = default;

    virtual void run() override;
    void onEnter() override;

    private:
    UINode* m_exitNode;
    uint8_t m_volume;
    uint8_t m_channel;

    uint8_t  m_chaNo = 1;
 
    LED_8SEG::SegmentCode m_dispContent[4];

};