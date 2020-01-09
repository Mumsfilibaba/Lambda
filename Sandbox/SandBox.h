#pragma once
#include "Core/Engine/Layer.h"

namespace Lambda
{
    //------------
    //SandBoxLayer 
    //------------
    class SandBoxLayer final : public Layer
    {
    public:
        SandBoxLayer();
        ~SandBoxLayer() = default;

        LAMBDA_NO_COPY(SandBoxLayer);

        virtual void OnUpdate(const Time& deltaTime) override final;
        virtual bool OnSystemEvent(const SystemEvent& event) override final;
    };
}
