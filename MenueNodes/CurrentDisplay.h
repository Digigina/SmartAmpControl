#pragma once
#include <stdint.h>
#include "pico/binary_info.h"
#include "pico/stdlib.h"
#include "UINode.h"
#include "StepEncBase.h"
#include "CurrentSensor.h"
#include "LowPass.h"

class CurrentDisplay : public UINode
{
    public:
    CurrentDisplay(UINode* exitNode, CurrentSensor* device);
    ~CurrentDisplay() = default;

    virtual void run() override;
    void onEnter() override;

    private:
    CurrentSensor* m_device;
    uint16_t m_current;
    uint8_t  m_selCha;
 
    LED_8SEG::SegmentCode m_dispContent[4];

    LowPassFilter m_filter;

};