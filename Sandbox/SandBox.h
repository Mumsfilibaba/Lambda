#pragma once
#include "Core/Layer.h"
#include "Graphics/Core/IDeviceContext.h"
#include "Graphics/Core/IShader.h"
#include "Graphics/Core/IPipelineState.h"
#include "Graphics/Core/IBuffer.h"
#include "Graphics/Core/ITexture.h"
#include "Graphics/Core/ISamplerState.h"
#include "Graphics/Core/IQuery.h"
#include "Graphics/Camera.h"

namespace Lambda
{    
    //------------
    //SandBoxLayer
    //------------

    class SandBoxLayer : public Layer
    {
    public:
        LAMBDA_NO_COPY(SandBoxLayer);

        SandBoxLayer();
        ~SandBoxLayer() = default;

        virtual void OnLoad() override final;
        virtual void OnUpdate(Timestep dt) override final;
        virtual void OnRender(Renderer3D& renderer, Timestep dt) override final;
        virtual void OnRenderUI(Timestep dt) override final;
        virtual void OnRelease() override final;
    };
}
