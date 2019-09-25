#include "LambdaPch.h"
#include "Debug/DebugLayer.h"

namespace Lambda
{
    DebugLayer::DebugLayer()
        : Layer("DebugLayer")
    {
    }


    bool DebugLayer::OnEvent(const Event& event)
    {
        EventForwarder forwarder;
        return forwarder.ForwardEvent(this, &DebugLayer::OnKeyPressed, event);
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
