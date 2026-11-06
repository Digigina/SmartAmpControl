#pragma once

#include "UINode.h"
#include "LED_8SEG.h"


struct MenueNodeEntry
{
    const LED_8SEG::SegmentCode name[4];
    UINode* node;
};

class MenueNode : public UINode
{
    public:
    MenueNode(UINode* exitNode, MenueNodeEntry* menueEntries);
    ~MenueNode() = default;

    virtual void run() override;
    void onEnter() override;

    private:
    MenueNodeEntry* m_menueEntries;
    uint8_t m_noOfEntries;
    uint8_t m_curentMenueEntry;
    LED_8SEG::SegmentCode m_dispContent[4];
};