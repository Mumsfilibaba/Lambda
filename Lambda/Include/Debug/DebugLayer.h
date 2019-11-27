#pragma once
#include "Graphics/Core/ITexture.h"
#include "Graphics/Core/IShader.h"
#include "Graphics/Core/ISamplerState.h"
#include "Graphics/Core/IPipelineState.h"
#include "Graphics/Core/IBuffer.h"
#include "Core/Layer.h"
#include "Time/Timestep.h"
#include <imgui.h>

namespace Lambda
{
	class IBuffer;
	class IShader;
	class ITexture;
	class IRenderPass;
	class ISamplerState;
	class IDeviceContext;
	class IPipelineState;

	//-------
	//DebugLayer
	//-------

	class LAMBDA_API DebugLayer : public Layer
	{
	public:
		LAMBDA_NO_COPY(DebugLayer);

		DebugLayer();
		~DebugLayer() = default;

        void Begin(Timestep time);
        void End();
        void Draw(IDeviceContext* pContext);

        virtual void OnLoad() override final;
        virtual void OnRenderUI(Timestep dt) override final;
        virtual void OnRelease() override final;
    private:
        void Init();
	private:
		AutoRef<IShader> m_VS;
		AutoRef<IShader> m_PS;
		AutoRef<ISamplerState> m_SamplerState;
		AutoRef<IPipelineState>	m_PipelineState;
		AutoRef<ITexture> m_FontTexture;
		AutoRef<IBuffer> m_VertexBuffer;
		AutoRef<IBuffer> m_IndexBuffer;
		AutoRef<IShaderVariableTable> m_VariableTable;
	};
}
