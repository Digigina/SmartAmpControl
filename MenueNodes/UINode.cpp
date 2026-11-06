#include "UINode.h"

StepEncBase* UINode::m_stepEnc = nullptr;
LED_8SEG* UINode::m_display = nullptr;

UINode* UINode::m_currentNode = nullptr;

UINode::UINode(UINode* exitNode) 
{
	m_exitNode = exitNode;
}

void UINode::initialize(StepEncBase* stepEnc, LED_8SEG* display)
{
	m_stepEnc = stepEnc;
	m_display = display;
}