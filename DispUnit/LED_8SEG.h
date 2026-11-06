#pragma once

#include <stdint.h>
#include "SpiPort.h"
#include <stdlib.h>

#define KILOBIT   0XFE //KiloBit
#define HUNDREDS  0XFD //hundreds
#define TENS      0XFB //tens
#define UNITS     0XF7 //units
#define Dot       0x80 //decimal separator
class LED_8SEG
{
    public:
    enum class PosCode : uint8_t
    {
        Position1000= 0XFE,
        Position100 = 0XFD,
        Position10 = 0XFB,
        Position1 = 0XF7
    };

    enum class SegmentCode : uint8_t
    {
        Blank = 0x00,
        Digit0 = 0x3F,
        Digit1 = 0x06,
        Digit2 = 0x5B,
        Digit3 = 0x4F,
        Digit4 = 0x66,
        Digit5 = 0x6D,
        Digit6 = 0x7D,
        Digit7 = 0x07,
        Digit8 = 0x7F,
        Digit9 = 0x6F,
        DigitA = 0x77,
        Digita = 0xDC,
        DigitB = 0x7C,
        DigitC = 0x39,
        Digitd = 0x5E,
        DigitE = 0x79,
        DigitF = 0x71,
        DigitH = 0x76,
        Digith = 0x74,
        DigitI = 0x30,
        DigitL = 0x38,
        Digitn = 0x54,
        Digito = 0x5C,
        Digitr = 0x50,
        DigitS = 0x6D,
        Digitt = 0x70,
        Digitu = 0x1C,
        DigitU = 0x3E,
        DigitDot = 0x80
    };

    LED_8SEG(SpiPort* spi_port, uint8_t repetion_rate_ms);
    ~LED_8SEG() = default;

    void init();
    void run();
    void setContent(const SegmentCode *content, size_t length);
    void setDigit(uint8_t buf_index, SegmentCode code, bool dot = false);
    SegmentCode num2Digit(uint8_t num) { return Num2DigitMap[num]; }

    private:
    void sendCommand(PosCode pos, SegmentCode seg);
 

    SpiPort* m_spiPort;
    uint8_t m_repetionRate_ms;
    uint32_t m_updateTime_us;
    // content of the 4 digits, index 0 = ones, index 1 = tens, index 2 = hundreds, index 3 = thousends
    SegmentCode m_content[4];
    SegmentCode m_workContent[4];
    static PosCode m_posCodeMap[4];
    uint16_t m_digitCnt;

    static SegmentCode Num2DigitMap[];
};
