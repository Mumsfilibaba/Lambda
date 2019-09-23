#pragma once
#include "IObject.h"
#include "Events/EventDispatcher.h"
#include "System/IWindow.h"
#include "ICommandList.h"

namespace Lambda
{
    class IQuery;
    class IWindow;
	class IBuffer;
    class IShader;
    class ITexture;
    class IRenderPass;
    class ISamplerState;
    class IGraphicsPipelineState;
	class IPipelineResourceState;

    struct QueryDesc;
	struct BufferDesc;
    struct ShaderDesc;
    struct TextureDesc;
    struct RenderPassDesc;
    struct SamplerStateDesc;
    struct GraphicsPipelineStateDesc;
	struct PipelineResourceStateDesc;

    //------------------
    //GraphicsDeviceDesc
    //------------------
    
    struct GraphicsDeviceDesc
    {
        IWindow*    pWindow         = nullptr;
        GraphicsApi Api             = GRAPHICS_API_VULKAN;
        uint32      Flags           = GRAPHICS_CONTEXT_FLAG_NONE;
		uint32      SampleCount     = 1;
        uint32      BackBufferCount = 3;
    };

    //---------------
    //IGraphicsDevice
    //---------------

    class LAMBDA_API GraphicsLayer : public EventLayer
    {
    public:
        LAMBDA_NO_COPY(GraphicsLayer);
        
        GraphicsLayer();
        ~GraphicsLayer() = default;
        
        virtual void    OnPop() override final;
        virtual void    OnPush() override final;
        virtual bool    OnEvent(const Event& event) override final;
        virtual uint32  GetRecivableCategories() const override final;
    };

    //---------------
    //IGraphicsDevice
    //---------------
    
	class LAMBDA_API IGraphicsDevice
	{
        friend class GraphicsLayer;
        
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
        virtual void CreateQuery(IQuery** ppQuery, const QueryDesc& desc) = 0;
        
		virtual void DestroyCommandList(ICommandList** ppList) = 0;
		virtual void DestroyBuffer(IBuffer** ppBuffer) = 0;
		virtual void DestroyTexture(ITexture** ppTexture) = 0;
		virtual void DestroyShader(IShader** ppShader) = 0;
		virtual void DestroySamplerState(ISamplerState** ppSamplerState) = 0;
		virtual void DestroyGraphicsPipelineState(IGraphicsPipelineState** ppPipelineState) = 0;
		virtual void DestroyRenderPass(IRenderPass** ppRenderPass) = 0;
		virtual void DestroyResourceState(IPipelineResourceState** ppResourceState) = 0;
        virtual void DestroyQuery(IQuery** ppQuery) = 0;
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
		virtual bool OnEvent(const Event& event) = 0;

	public:
		static IGraphicsDevice* Create(const GraphicsDeviceDesc& desc);
		static IGraphicsDevice* Get();

	protected:
		static IGraphicsDevice* s_pInstance;
	};
}
