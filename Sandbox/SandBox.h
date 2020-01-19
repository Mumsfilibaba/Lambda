#pragma once
#include "Core/Engine/Layer.h"

namespace Lambda
{
    //------------
    //SandBoxLayer 
    //------------
    class SandBoxLayer final : public CLayer
    {
    public:
        SandBoxLayer();
        ~SandBoxLayer() = default;

        LAMBDA_NO_COPY(SandBoxLayer);

        virtual void OnUpdate(const CTime& deltaTime) override final;
        virtual bool OnSystemEvent(const SSystemEvent& event) override final;
    };
}
