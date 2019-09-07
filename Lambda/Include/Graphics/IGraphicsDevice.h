#pragma once
#include "IObject.h"
#include "System/EventDispatcher.h"
#include "System/IWindow.h"
#include "ICommandList.h"

namespace Lambda
{
	class IBuffer;
	class ISamplerState;
	class ITexture2D;
	class IShader;
	class IGraphicsPipelineState;
	class IRenderPass;
	class IResourceState;

	struct BufferDesc;
	struct Texture2DDesc;
	struct ShaderDesc;
	struct SamplerStateDesc;
	struct GraphicsPipelineStateDesc;
	struct RenderPassDesc;
	struct ResourceStateDesc;

    
    struct GraphicsDeviceDesc
    {
        uint32 Flags = GRAPHICS_CONTEXT_FLAG_NONE;
        GraphicsApi Api = GRAPHICS_API_VULKAN;
    };
    
    
	class LAMBDA_API IGraphicsDevice
	{
	public:
		LAMBDA_INTERFACE(IGraphicsDevice);

		IGraphicsDevice() = default;
		~IGraphicsDevice() = default;

		virtual void CreateCommandList(ICommandList** ppList, CommandListType type) const = 0;
		virtual void CreateBuffer(IBuffer** ppBuffer, const ResourceData* pInitalData, const BufferDesc& desc) const = 0;
		virtual void CreateTexture2D(ITexture2D** ppTexture, const ResourceData* pInitalData, const Texture2DDesc& desc) const = 0;
		virtual void CreateShader(IShader** ppShader, const ShaderDesc& desc) const = 0;
		virtual void CreateSamplerState(ISamplerState** ppSamplerState, const SamplerStateDesc& desc) const = 0;
		virtual void CreateGraphicsPipelineState(IGraphicsPipelineState** ppPipelineState, const GraphicsPipelineStateDesc& desc) const = 0;
		virtual void CreateRenderPass(IRenderPass** ppRenderPass, const RenderPassDesc& desc) const = 0;
		virtual void CreateResourceState(IResourceState** ppResourceState, const ResourceStateDesc& desc) const = 0;

		virtual void DestroyCommandList(ICommandList** ppList) const = 0;
		virtual void DestroyBuffer(IBuffer** ppBuffer) const = 0;
		virtual void DestroyTexture2D(ITexture2D** ppTexture) const = 0;
		virtual void DestroyShader(IShader** ppShader) const = 0;
		virtual void DestroySamplerState(ISamplerState** ppSamplerState) const = 0;
		virtual void DestroyGraphicsPipelineState(IGraphicsPipelineState** ppPipelineState) const = 0;
		virtual void DestroyRenderPass(IRenderPass** ppRenderPass) const = 0;
		virtual void DestroyResourceState(IResourceState** ppResourceState) const = 0;
		virtual void Destroy() const = 0;

		virtual void ExecuteCommandList(ICommandList* const * ppLists, uint32 numLists) const = 0;
		virtual void ExecuteCommandListAndPresent(ICommandList* const* ppLists, uint32 numLists) const = 0;
		virtual void Present() const = 0;
		virtual void WaitForGPU() const = 0;
		virtual void GPUWaitForFrame() const = 0;

		virtual void* GetNativeHandle() const  = 0;
		virtual ITexture2D* GetDepthStencil() const  = 0;
		virtual ITexture2D* GetCurrentRenderTarget() const  = 0;
		virtual ResourceFormat GetBackBufferFormat() const  = 0;
		virtual uint32 GetCurrentBackBufferIndex() const  = 0;
		virtual uint32 GetSwapChainWidth() const  = 0;
		virtual uint32 GetSwapChainHeight() const  = 0;

	private:
		virtual bool InternalOnEvent(const Event& event) = 0;

	public:
		static IGraphicsDevice* Create(IWindow* pWindow, const GraphicsDeviceDesc& desc);
		static IGraphicsDevice* GetInstance();
		static bool OnEvent(const Event& event);

	protected:
		static IGraphicsDevice* s_pInstance;
	};
}
