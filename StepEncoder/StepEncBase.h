#pragma once
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"

#include "pico/binary_info.h"



class StepEncBase
{
    public:
    StepEncBase(i2c_inst_t* i2c_inst, uint8_t i2c_adr);
    virtual ~StepEncBase();

    virtual bool initEncoder() = 0;
    virtual bool getButtonState() = 0;
    virtual uint32_t getPosition() = 0;


    protected:
    uint8_t     m_i2cAdr;
    i2c_inst_t* m_i2cInst;

};