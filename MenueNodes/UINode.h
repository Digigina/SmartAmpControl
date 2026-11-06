#pragma once
#include <stdint.h>
#include "StepEncBase.h"
#include "LED_8SEG.h"

// ---------------------------------------------------------------------------
// Basisklasse für die Menue-Nodes, die über den Step-Encoder gesteuert werden
// ---------------------------------------------------------------------------
class UINode
{
    public:
    UINode(UINode* exitNode=nullptr);
    ~UINode() = default;
    
    // Virtuelle Methoden, die von den abgeleiteten Klassen implementiert werden müssen
    virtual void run()=0;
    virtual void onEnter()=0;
    virtual void onUpdate() {};
    static  void initialize(StepEncBase* stepEnc, LED_8SEG* display);
    static  void changeEvent() {if (m_currentNode) { m_currentNode->onUpdate(); }};
    static UINode* m_currentNode;

    protected:
    static StepEncBase* m_stepEnc;
    static LED_8SEG* m_display;
    int32_t m_lastRawPos;
    bool m_lastButtonState;
    uint32_t m_lastTime_us;
    UINode* m_exitNode;
 };