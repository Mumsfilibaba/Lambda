#pragma once
#include <System/Application.h>
#include <Graphics/ICommandList.h>
#include <Graphics/IShader.h>
#include <Graphics/IPipelineState.h>
#include <Graphics/IBuffer.h>
#include <Graphics/ITexture2D.h>

namespace Lambda
{
	class SandBox final : public Application
	{
	public:
		SandBox();
		~SandBox();

		virtual void OnLoad() override final;
		virtual void OnUpdate(Time dt) override final;
		virtual void OnRender(Time dt) override final;
		virtual void OnRelease() override final;

	private:
		ICommandList* m_pCurrentList;
		ICommandList* m_pLists[3];
		IShader* m_pVS;
		IShader* m_pPS;
		IBuffer* m_pVertexBuffer;
		IBuffer* m_pColorBuffer;
		ITexture2D* m_pDepthBuffer;
		IGraphicsPipelineState* m_pPipelineState;

	public:
		static bool OnEvent(const Event& event);
	};
}