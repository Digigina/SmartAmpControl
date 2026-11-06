#pragma once
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"

#include "pico/binary_info.h"

class VolChControl
{
    public:
    VolChControl(i2c_inst_t* i2c_inst, uint8_t i2c_adr);
    void init(void (*updateCB)());

    void setVolume(uint8_t newVol);
    void setChannel(uint8_t newCha);
    uint8_t getVolume() { return m_curVol; };
    uint8_t getChannel() { return m_curCha; };
 
    private:
    void updateRegister();

    uint8_t m_curVol;
    uint8_t m_curCha;

    uint8_t m_gpioPortA;
    uint8_t m_gpioPortB;

    void (*m_updateCB)();

    protected:
    uint8_t     m_i2cAdr;
    i2c_inst_t* m_i2cInst;


};