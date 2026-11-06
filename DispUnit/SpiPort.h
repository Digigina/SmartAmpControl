#pragma once
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "pico/binary_info.h"

class SpiPort
{
    public:
    SpiPort(spi_inst_t* spi_inst, uint8_t cs_pin, uint8_t clk_pin, uint8_t mosi_pin);
    ~SpiPort();

    void init();
    void writeData(uint8_t data);
    void writeDataBuffer(const uint8_t* data, size_t length);
    void chipSelect(bool active);

    private:
    spi_inst_t* m_spiInst;
    uint8_t m_csPin;
    uint8_t m_clkPin;
    uint8_t m_mosiPin;
};