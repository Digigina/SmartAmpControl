#include "ChannelSelector.h"

ChannelSelector::ChannelSelector(UINode* exitNode, VolChControl* device) : UINode(exitNode), m_device(device)
{
    m_channel = 0;
}


void ChannelSelector::run()
{
    int32_t delta;
    bool chaIncr;
    uint32_t raw_pos;
    bool buttonState;
    uint32_t currentTime_us;

    currentTime_us = time_us_64();
    if (currentTime_us - m_lastTime_us > 10 * 1000) 
    {
        raw_pos = m_stepEnc->getPosition();
        delta = raw_pos - m_lastRawPos ;
        if(delta != 0 && m_lastButtonState == false)
        {
            m_chaNo += delta;
            if(m_chaNo > 4)
                m_chaNo = 1;
            else if(m_chaNo < 1)
                m_chaNo = 4;
            m_device->setChannel(m_chaNo);
            printf("Raw-Position: %#04.4x, Position: %2.2d\n", raw_pos, m_chaNo);
            m_lastRawPos = raw_pos;
            m_dispContent[0] = m_display->num2Digit(m_chaNo);
            m_display->setContent(m_dispContent, sizeof(m_dispContent)/sizeof(m_dispContent[0]));
        }

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

void ChannelSelector::onEnter()
{
    m_chaNo = m_device->getChannel();
    m_dispContent[3] = LED_8SEG::SegmentCode::DigitC;
    m_dispContent[2] = LED_8SEG::SegmentCode::DigitH;
    m_dispContent[1] = LED_8SEG::SegmentCode::Blank;
    m_dispContent[0] = m_display->num2Digit(m_chaNo);
    m_display->setContent(m_dispContent, sizeof(m_dispContent)/sizeof(m_dispContent[0]));
    m_lastButtonState = m_stepEnc->getButtonState();
    m_lastRawPos = m_stepEnc->getPosition();
    m_lastTime_us = time_us_64();
}