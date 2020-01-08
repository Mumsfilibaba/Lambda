#pragma once
#include "Core/Engine/Layer.h"

namespace Lambda
{
    //------------
    //CSandBoxLayer 
    //------------
    class CSandBoxLayer final : public CLayer
    {
    public:
        CSandBoxLayer();
        ~CSandBoxLayer() = default;

        LAMBDA_NO_COPY(CSandBoxLayer);

        virtual bool DispatchEvent(const SSystemEvent& event) override final;
    };
}
