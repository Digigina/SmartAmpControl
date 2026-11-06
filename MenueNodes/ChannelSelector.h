#pragma once
#include <stdint.h>
#include "pico/binary_info.h"
#include "pico/stdlib.h"
#include "UINode.h"
#include "StepEncBase.h"
#include "LED_8SEG.h"
#include "VolChControl.h"

class ChannelSelector : public UINode
{
    public:
    ChannelSelector(UINode* exitNode, VolChControl* device);
    ~ChannelSelector() = default;

    virtual void run() override;
    void onEnter() override;

    private:
    void* m_nextNodeParam;
    uint8_t m_channel;

    int8_t  m_chaNo = 1;
    VolChControl* m_device;
  
    LED_8SEG::SegmentCode m_dispContent[4];

};