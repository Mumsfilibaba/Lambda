#pragma once
#include "Core/CLayer.h"
#include "Graphics/Core/IDeviceContext.h"
#include "Graphics/Core/IShader.h"
#include "Graphics/Core/IPipelineState.h"
#include "Graphics/Core/IBuffer.h"
#include "Graphics/Core/ITexture.h"
#include "Graphics/Core/ISamplerState.h"
#include "Graphics/Core/IQuery.h"
#include "Graphics/CCamera.h"

namespace Lambda
{    
    //------------
    //SandBoxLayer
    //------------

    class SandBoxLayer : public CLayer
    {
    public:
        LAMBDA_NO_COPY(SandBoxLayer);

        SandBoxLayer();
        ~SandBoxLayer() = default;

        virtual void OnLoad() override final;
        virtual void OnUpdate(const CTime& dt) override final;
        virtual void OnRender(Renderer3D& renderer, const CTime& dt) override final;
        virtual void OnRenderUI(const CTime& dt) override final;
        virtual void OnRelease() override final;
    };
}
