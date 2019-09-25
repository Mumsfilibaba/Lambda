#include "LambdaPch.h"
#include "Debug/DebugLayer.h"

namespace Lambda
{
    DebugLayer::DebugLayer()
        : EventLayer("DebugLayer")
    {
    }


    void DebugLayer::OnPush()
    {
    }


    void DebugLayer::OnPop()
    {
    }


    bool DebugLayer::OnEvent(const Event& event)
    {
        return EventDispatcher::ForwardEvent(this, &DebugLayer::OnKeyPressed, event);
    }


    uint32 DebugLayer::GetRecivableCategories() const
    {
        return EVENT_CATEGORY_KEYBOARD;
    }


    bool DebugLayer::OnKeyPressed(const KeyPressedEvent& event)
    {
        return false;
    }
}
