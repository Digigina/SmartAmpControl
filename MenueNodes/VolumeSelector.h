#pragma once
#include <stdint.h>
#include "pico/binary_info.h"
#include "pico/stdlib.h"
#include "UINode.h"
#include "StepEncBase.h"
#include "LED_8SEG.h"
#include "VolChControl.h"

class VolumeSelector : public UINode
{
    public:
    VolumeSelector(UINode* exitNode, VolChControl* device);
    ~VolumeSelector() = default;

    virtual void run() override;
    void onEnter() override;
    void onUpdate() override;

    private:
    int16_t       m_volume;
    VolChControl* m_device;
 
    LED_8SEG::SegmentCode m_dispContent[4];

};