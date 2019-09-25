#pragma once
#include "Events/EventDispatcher.h"
#include "Events/KeyEvent.h"

namespace Lambda
{
    class DebugLayer : public EventLayer
    {
    public:
        LAMBDA_NO_COPY(DebugLayer);
        
        DebugLayer();
        ~DebugLayer() = default;

        virtual void    OnPush() override final;
        virtual void    OnPop() override final;
        virtual bool    OnEvent(const Event& event) override final;
        virtual uint32  GetRecivableCategories() const override final;
        
    private:
        bool OnKeyPressed(const KeyPressedEvent& event);
    };
}
