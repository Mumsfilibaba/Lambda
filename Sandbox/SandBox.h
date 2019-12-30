#pragma once
#include "Core/Engine/Layer.h"

namespace Lambda
{
    class IEngine;

    //------------
    //CSandBoxLayer 
    //------------
    class CSandBoxLayer final : public CLayer
    {
    public:
        CSandBoxLayer(IEngine* pEngine);
        ~CSandBoxLayer() = default;

        LAMBDA_NO_COPY(CSandBoxLayer);

        virtual bool OnSystemEvent(const SSystemEvent& event) override final;
    };
}
