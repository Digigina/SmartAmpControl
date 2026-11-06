#include "VolumeSelector.h"

VolumeSelector::VolumeSelector(UINode* exitNode, VolChControl* device) : UINode(exitNode), m_device(device)
{
    m_volume = 0;
    m_dispContent[0] = LED_8SEG::SegmentCode::Digit0;
    m_dispContent[1] = LED_8SEG::SegmentCode::Digit0;
    m_dispContent[2] = LED_8SEG::SegmentCode::Blank;
    m_dispContent[3] = LED_8SEG::SegmentCode::Digit1;
}


void VolumeSelector::run()
{
    int32_t delta;
    uint32_t raw_pos;
    bool buttonState;
    uint32_t currentTime_us;

    currentTime_us = time_us_64();
    if (currentTime_us - m_lastTime_us > 10 * 1000) 
    {
        raw_pos = m_stepEnc->getPosition();
        //raw_pos = rot_enc_read();
        delta = raw_pos - m_lastRawPos ;
        if(delta != 0 && m_lastButtonState == false)
        {
            m_volume += delta;
            if(m_volume > 63)
                m_volume = 63;
            else if(m_volume < 0)
                m_volume = 0;
            m_device->setVolume(m_volume);
            printf("Raw-Position: %#04.4x, Position: %2.2d\n", raw_pos, m_volume);
            m_lastRawPos = raw_pos;
            m_dispContent[0] = m_display->num2Digit(m_volume % 10);
            m_dispContent[1] = m_display->num2Digit((m_volume / 10) % 10);
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

void VolumeSelector::onEnter()
{
    m_volume = m_device->getVolume();
    m_dispContent[0] = m_display->num2Digit(m_volume % 10);
    m_dispContent[1] = m_display->num2Digit((m_volume / 10) % 10);
    m_dispContent[2] = LED_8SEG::SegmentCode::Blank;
    m_dispContent[3] = m_display->num2Digit(m_device->getChannel());
    m_display->setContent(m_dispContent, sizeof(m_dispContent)/sizeof(m_dispContent[0]));
    sleep_ms(100);
    m_lastRawPos = m_stepEnc->getPosition();
    m_lastButtonState = m_stepEnc->getButtonState();
    m_lastTime_us = time_us_64();
}

void VolumeSelector::onUpdate()
{
    m_volume = m_device->getVolume();
    m_dispContent[0] = m_display->num2Digit(m_volume % 10);
    m_dispContent[1] = m_display->num2Digit((m_volume / 10) % 10);
    m_dispContent[2] = LED_8SEG::SegmentCode::Blank;
    m_dispContent[3] = m_display->num2Digit(m_device->getChannel());
    m_display->setContent(m_dispContent, sizeof(m_dispContent)/sizeof(m_dispContent[0]));
}