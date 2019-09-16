#pragma once
#include "IObject.h"
#include "System/EventDispatcher.h"
#include "System/IWindow.h"
#include "ICommandList.h"

namespace Lambda
{
    class IWindow;
	class IBuffer;
	class ISamplerState;
	class ITexture;
	class IShader;
	class IGraphicsPipelineState;
	class IRenderPass;
	class IPipelineResourceState;

	struct BufferDesc;
	struct TextureDesc;
	struct ShaderDesc;
	struct SamplerStateDesc;
	struct GraphicsPipelineStateDesc;
	struct RenderPassDesc;
	struct PipelineResourceStateDesc;

    
    struct GraphicsDeviceDesc
    {
        IWindow*    pWindow = nullptr;
        GraphicsApi Api = GRAPHICS_API_VULKAN;
        uint32      Flags = GRAPHICS_CONTEXT_FLAG_NONE;
		uint32      SampleCount = 1;
        uint32      BackBufferCount = 3;
    };
    
    
	class LAMBDA_API IGraphicsDevice
	{
	public:
		LAMBDA_INTERFACE(IGraphicsDevice);

		IGraphicsDevice() = default;
		~IGraphicsDevice() = default;

		virtual void CreateCommandList(ICommandList** ppList, CommandListType type) = 0;
		virtual void CreateBuffer(IBuffer** ppBuffer, const ResourceData* pInitalData, const BufferDesc& desc) = 0;
		virtual void CreateTexture(ITexture** ppTexture, const ResourceData* pInitalData, const TextureDesc& desc) = 0;
		virtual void CreateShader(IShader** ppShader, const ShaderDesc& desc) = 0;
		virtual void CreateSamplerState(ISamplerState** ppSamplerState, const SamplerStateDesc& desc) = 0;
		virtual void CreateGraphicsPipelineState(IGraphicsPipelineState** ppPipelineState, const GraphicsPipelineStateDesc& desc) = 0;
		virtual void CreateRenderPass(IRenderPass** ppRenderPass, const RenderPassDesc& desc) = 0;
		virtual void CreatePipelineResourceState(IPipelineResourceState** ppResourceState, const PipelineResourceStateDesc& desc) = 0;

		virtual void DestroyCommandList(ICommandList** ppList) = 0;
		virtual void DestroyBuffer(IBuffer** ppBuffer) = 0;
		virtual void DestroyTexture(ITexture** ppTexture) = 0;
		virtual void DestroyShader(IShader** ppShader) = 0;
		virtual void DestroySamplerState(ISamplerState** ppSamplerState) = 0;
		virtual void DestroyGraphicsPipelineState(IGraphicsPipelineState** ppPipelineState) = 0;
		virtual void DestroyRenderPass(IRenderPass** ppRenderPass) = 0;
		virtual void DestroyResourceState(IPipelineResourceState** ppResourceState) = 0;
		virtual void Destroy() const = 0;

		virtual void ExecuteCommandList(ICommandList* const * ppLists, uint32 numLists) const = 0;
		virtual void ExecuteCommandListAndPresent(ICommandList* const* ppLists, uint32 numLists) const = 0;
		virtual void Present() const = 0;
		virtual void WaitForGPU() const = 0;
		virtual void GPUWaitForFrame() const = 0;

        virtual GraphicsDeviceDesc  GetDesc() const = 0;
        virtual void*               GetNativeHandle() const = 0;
		virtual ITexture*           GetDepthStencil() const = 0;
		virtual ITexture*           GetRenderTarget() const = 0;
		virtual ResourceFormat      GetBackBufferFormat() const = 0;
		virtual uint32              GetBackBufferIndex() const = 0;
		virtual uint32              GetSwapChainWidth() const = 0;
		virtual uint32              GetSwapChainHeight() const = 0;

	private:
		virtual bool InternalOnEvent(const Event& event) = 0;

	public:
		static IGraphicsDevice* Create(const GraphicsDeviceDesc& desc);
		static IGraphicsDevice* GetInstance();
		static bool OnEvent(const Event& event);

	protected:
		static IGraphicsDevice* s_pInstance;
	};
}
