#include "LED_8SEG.h"
#include "pico/time.h"

LED_8SEG::SegmentCode LED_8SEG::Num2DigitMap[10] = {
    LED_8SEG::SegmentCode::Digit0,
    LED_8SEG::SegmentCode::Digit1,
    LED_8SEG::SegmentCode::Digit2,
    LED_8SEG::SegmentCode::Digit3,
    LED_8SEG::SegmentCode::Digit4,
    LED_8SEG::SegmentCode::Digit5,
    LED_8SEG::SegmentCode::Digit6,
    LED_8SEG::SegmentCode::Digit7,
    LED_8SEG::SegmentCode::Digit8,
    LED_8SEG::SegmentCode::Digit9  
};

LED_8SEG::PosCode LED_8SEG::m_posCodeMap[4] = {
    PosCode::Position1,
    PosCode::Position10,
    PosCode::Position100,
    PosCode::Position1000
};

LED_8SEG::LED_8SEG(SpiPort* spi_port, uint8_t repetion_rate_ms)
    : m_spiPort(spi_port),
      m_repetionRate_ms(repetion_rate_ms),
      m_updateTime_us(0)
{
}

void LED_8SEG::setContent(const SegmentCode* content, size_t length)
{
    if (content == nullptr) {
        return;
    }

    for (size_t index = 0; index < 4; ++index) {
        m_content[index] = SegmentCode::Blank;
    }

    const size_t count = length < 4 ? length : 4;
    for (size_t index = 0; index < count; ++index) {
        m_content[index] = content[index];
    }
}

void LED_8SEG::setDigit(uint8_t buf_index, SegmentCode code, bool dot)
{
    if (buf_index >= 4) {
        return;
    }

    uint8_t segments = static_cast<uint8_t>(code);
    if (dot) {
        segments |= static_cast<uint8_t>(SegmentCode::DigitDot);
    }
    m_content[buf_index] = static_cast<SegmentCode>(segments);
}

/**
 * @brief Initializes the display state and its SPI port.
 *
 * Clears all four digit values, records the initial refresh timestamp, and
 * initializes the configured SPI port.
 */
void LED_8SEG::init()
{
    const SegmentCode initialContent[4] = {
        SegmentCode::Blank,
        SegmentCode::Blank,
        SegmentCode::Blank,
        SegmentCode::Blank
    };
    setContent(initialContent, 4);
    
    m_updateTime_us = time_us_64();
    m_digitCnt = 0;
    m_spiPort->init();

}

/**
 * @brief Refreshes the four display digits when the update interval has elapsed.
 *
 * Returns without transmitting if less than `m_repetionRate_ms` milliseconds
 * have passed since the previous refresh. Otherwise, sends each digit's
 * position and segment code over SPI and records the refresh time.
 */
void LED_8SEG::run()
{
    const uint32_t currentTime_us = time_us_64();
    if (currentTime_us - m_updateTime_us < m_repetionRate_ms * 1000) {
        //sendCommand(m_posCodeMap[static_cast<size_t>(PosCode::Position1000)], SegmentCode::Blank);
        return;
    }
    m_updateTime_us = currentTime_us;

    if(m_digitCnt == 0)
    {
        for (uint8_t index = 0; index < 4; ++index) {
            m_workContent[index] = m_content[index];
        }
    }
    // for (uint8_t index = 0; index < 4; ++index) {
    //     sendCommand(m_posCodeMap[index], m_content[index]);
    // }
    // sendCommand(m_posCodeMap[3], SegmentCode::Blank);
    sendCommand(m_posCodeMap[m_digitCnt], m_workContent[m_digitCnt]);
    m_digitCnt++;
    if(m_digitCnt > 3)
        m_digitCnt = 0;

}

/**
 * @brief Sends a segment code to the specified display position.
 * @param pos Display position, encoded by LED_8SEG::PosCode.
 * @param digit Segment pattern, encoded by LED_8SEG::SegmentCode.
 * @details Activates chip select, sends the position and segment bytes over
 *          SPI, waits 100 microseconds, then deactivates chip select.
 */
void LED_8SEG::sendCommand(PosCode pos, SegmentCode digit) 
{
    m_spiPort->chipSelect(true);
    m_spiPort->writeData(static_cast<uint8_t>(pos));
    m_spiPort->writeData(static_cast<uint8_t>(digit));
    sleep_us(100);
    m_spiPort->chipSelect(false);
}
