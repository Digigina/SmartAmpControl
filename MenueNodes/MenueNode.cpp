#include "MenueNode.h"


MenueNode::MenueNode(UINode* exitNode, MenueNodeEntry* menueEntries)
    : UINode(exitNode),
      m_menueEntries(menueEntries)
{
}

void MenueNode::onEnter()
{
    m_noOfEntries = 0;
    while (m_menueEntries != nullptr && m_menueEntries[m_noOfEntries].node != nullptr) {
        m_noOfEntries++;
    }
    m_curentMenueEntry = 0;
    for(int i=0; i<4; i++)
    {
        m_dispContent[i] = m_menueEntries[m_curentMenueEntry].name[i];
    }
    m_display->setContent(m_dispContent, sizeof(m_dispContent)/sizeof(m_dispContent[0]));
    m_lastButtonState = m_stepEnc->getButtonState();
    m_lastRawPos = m_stepEnc->getPosition();
    m_lastTime_us = time_us_64();
}

void MenueNode::run()
{
    int32_t  delta;
    uint32_t raw_pos;
    bool     buttonState;
    uint32_t currentTime_us;

    currentTime_us = time_us_64();
    if (currentTime_us - m_lastTime_us > 10 * 1000) 
    {
        raw_pos = m_stepEnc->getPosition();
        delta = raw_pos - m_lastRawPos ;
        if(delta != 0 && m_lastButtonState == false)
        {
            m_curentMenueEntry += delta;
            m_curentMenueEntry %= m_noOfEntries;
            printf("Raw-Position: %#04.4x, Position: %2.2d\n", raw_pos, m_curentMenueEntry);
            m_lastRawPos = raw_pos;
            for(int i=0; i<4; i++)
            {
                m_dispContent[i] = m_menueEntries[m_curentMenueEntry].name[i];
            }
            m_display->setContent(m_dispContent, sizeof(m_dispContent)/sizeof(m_dispContent[0]));
        }

        buttonState = m_stepEnc->getButtonState();
        if(buttonState == false && m_lastButtonState == true)
        {
            m_currentNode = m_menueEntries[m_curentMenueEntry].node;
            m_currentNode->onEnter();
        }
        m_lastButtonState = buttonState;
    }
}