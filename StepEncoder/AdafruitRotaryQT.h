#pragma once
#include "StepEncBase.h"

class AdafruitRotaryQT : public StepEncBase
{
    public:
    AdafruitRotaryQT(i2c_inst_t* i2c_inst, uint8_t i2c_adr);
    ~AdafruitRotaryQT() override;

    bool initEncoder() override;
    bool getButtonState() override;
    uint32_t getPosition() override;

};