#pragma once
#include "System/Application.h"
#include "Graphics/ICommandList.h"
#include "Graphics/IShader.h"
#include "Graphics/IPipelineState.h"
#include "Graphics/IBuffer.h"
#include "Graphics/ITexture2D.h"
#include "Graphics/ISamplerState.h"
#include "Math/Math.h"

namespace Lambda
{
	struct CameraBuffer
	{
		Math::Mat4f View;
		Math::Mat4f Proj;
	};


	class SandBox final : public Application
	{
	public:
		SandBox(const EngineParams& params = EngineParams());
		~SandBox();

		virtual void OnLoad() override final;
		virtual void OnUpdate(Time dt) override final;
		virtual void OnRender(Time dt) override final;
		virtual void OnRelease() override final;

		void CreateCamera(uint32 width, uint32 height);

	private:
		ICommandList* m_pLists[3];
        ICommandList* m_pCurrentList;
		IShader* m_pVS;
		IShader* m_pPS;
		IShader* m_pCompute;
		IBuffer* m_pVertexBuffer;
        IBuffer* m_pIndexBuffer;
		IBuffer* m_pColorBuffer;
		IBuffer* m_pCameraBuffer;
		ITexture2D* m_pDepthBuffer;
		ITexture2D* m_pTexture;
		ISamplerState* m_pSampler;
		IGraphicsPipelineState* m_pPipelineState;
		float m_Width;
		float m_Height;
		CameraBuffer m_Camera;

	public:
		static bool OnEvent(const Event& event);
	};
}
