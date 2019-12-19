#pragma once
#include "Core/CLayer.h"
#include "Core/Event/IEventListener.h"
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
	class CEnvironment;
	class CMouseMovedEvent;

    //------------
    //SandBoxLayer
    //------------

    class SandBoxLayer : public CLayer, public IEventListener
    {
    public:
        SandBoxLayer();
        ~SandBoxLayer() = default;
        
		LAMBDA_NO_COPY(SandBoxLayer);

		bool OnMouseMove(const CMouseMovedEvent& event);
		virtual bool OnEvent(const CEvent& event) override final;

        virtual void OnLoad() override final;
        virtual void OnUpdate(const CTime& dt) override final;
        virtual void OnRender(Renderer3D& renderer, const CTime& dt) override final;
        virtual void OnRenderUI(const CTime& dt) override final;
        virtual void OnRelease() override final;
    };
}
