#include "CurrentDisplay.h"

CurrentDisplay::CurrentDisplay(UINode* exitNode, CurrentSensor* device) : UINode(exitNode), m_device(device), m_filter(0.01)
{
    m_current = 0;
    m_dispContent[0] = LED_8SEG::SegmentCode::Digit0;
    m_dispContent[1] = LED_8SEG::SegmentCode::Digit0;
    m_dispContent[2] = LED_8SEG::SegmentCode::Digit0;
    m_dispContent[3] = LED_8SEG::SegmentCode::DigitL;
}


void CurrentDisplay::run()
{
    int32_t delta;
    uint32_t raw_pos;
    bool buttonState;
    uint32_t currentTime_us;

    currentTime_us = time_us_64();
    if (currentTime_us - m_lastTime_us > 1 * 1000) 
    {
        raw_pos = m_stepEnc->getPosition();
        delta = raw_pos - m_lastRawPos ;
        if(delta != 0 && m_lastButtonState == false)
        {
            m_selCha = raw_pos & 1;
            m_device->selectChannel(m_selCha);
            sleep_us(500);
            m_filter.reset(m_device->getValueMilliAmpere());
        }
        m_current = m_filter.pushValue(m_device->getValueMilliAmpere());
        printf("Currrent: %4.4d, Channel: %s\n", m_current, m_selCha?"Right":"Left");
        m_lastRawPos = raw_pos;
        m_dispContent[0] = m_display->num2Digit(m_current % 10);
        m_dispContent[1] = m_display->num2Digit((m_current / 10) % 10);
        m_dispContent[2] = m_display->num2Digit((m_current / 100) % 10);
        if(m_selCha == 0)
            m_dispContent[3] = LED_8SEG::SegmentCode::DigitL;
        else
            m_dispContent[3] = LED_8SEG::SegmentCode::Digitr;
        
        m_display->setContent(m_dispContent, sizeof(m_dispContent)/sizeof(m_dispContent[0]));

        buttonState = m_stepEnc->getButtonState();
        if(buttonState == false && m_lastButtonState == true)
        {
            if(m_exitNode)
            {
                m_currentNode = m_exitNode;
                m_currentNode->onEnter();
            }
        }
        m_lastButtonState = buttonState;
    }
}

void CurrentDisplay::onEnter()
{
    m_selCha = 0; // select left channel
    m_device->selectChannel(m_selCha);
    sleep_us(500);

    m_current = m_device->getValueMilliAmpere();
    m_filter.reset(m_current);
    m_dispContent[0] = m_display->num2Digit(m_current % 10);
    m_dispContent[1] = m_display->num2Digit((m_current / 10) % 10);
    m_dispContent[2] = m_display->num2Digit((m_current / 100) % 10);
    m_dispContent[3] = LED_8SEG::SegmentCode::DigitL;
    m_display->setContent(m_dispContent, sizeof(m_dispContent)/sizeof(m_dispContent[0]));
    m_lastRawPos = m_stepEnc->getPosition();
    m_lastButtonState = m_stepEnc->getButtonState();
    m_lastTime_us = time_us_64();
}