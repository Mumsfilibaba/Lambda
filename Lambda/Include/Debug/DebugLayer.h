#pragma once
#include "System/Layer.h"
#include "Events/EventDispatcher.h"
#include "Events/KeyEvent.h"

namespace Lambda
{
    class DebugLayer : public Layer
    {
    public:
        LAMBDA_NO_COPY(DebugLayer);
        
        DebugLayer();
        ~DebugLayer() = default;

        virtual bool OnEvent(const Event& event) override final;
        virtual uint32 GetRecivableCategories() const override final;
    private:
        bool OnKeyPressed(const KeyPressedEvent& event);
    };
}
