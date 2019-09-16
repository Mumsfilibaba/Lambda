#pragma once
#include "Graphics/IGraphicsDevice.h"
#include "VKNAllocator.h"
#include "VKNUtilities.h"

#define FRAMES_AHEAD 3

namespace Lambda
{
	class VKNDevice;
    class VKNBuffer;
    class VKNTexture;
    class VKNCommandList;
    class VKNSamplerState;
	class VKNSwapChain;
	class VKNUploadBuffer;
	class VKNAllocator;
	class VKNFramebufferCache;
	class VKNBufferManager;
    
	//-----------------
	//VKNGraphicsDevice
	//-----------------

    class VKNGraphicsDevice final : public IGraphicsDevice
    {
    public:
        LAMBDA_NO_COPY(VKNGraphicsDevice);
        
        VKNGraphicsDevice(const GraphicsDeviceDesc& desc);
        ~VKNGraphicsDevice();
        
        virtual void CreateCommandList(ICommandList** ppList, CommandListType type) override final;
        virtual void CreateBuffer(IBuffer** ppBuffer, const ResourceData* pInitalData, const BufferDesc& desc) override final;
        virtual void CreateTexture(ITexture** ppTexture, const ResourceData* pInitalData, const TextureDesc& desc) override final;
        virtual void CreateShader(IShader** ppShader, const ShaderDesc& desc) override final;
        virtual void CreateSamplerState(ISamplerState** ppSamplerState, const SamplerStateDesc& desc) override final;
        virtual void CreateGraphicsPipelineState(IGraphicsPipelineState** ppPipelineState, const GraphicsPipelineStateDesc& desc) override final;
		virtual void CreateRenderPass(IRenderPass** ppRenderPass, const RenderPassDesc& desc) override final;
		virtual void CreatePipelineResourceState(IPipelineResourceState** ppResourceState, const PipelineResourceStateDesc& desc) override final;
				void CreateUploadBuffer(VKNUploadBuffer** ppUploadBuffer, uint64 sizeInBytes);

        virtual void DestroyCommandList(ICommandList** ppList) override final;
        virtual void DestroyBuffer(IBuffer** ppBuffer) override final;
        virtual void DestroyTexture(ITexture** ppTexture) override final;
        virtual void DestroyShader(IShader** ppShader) override final;
        virtual void DestroySamplerState(ISamplerState** ppSamplerState) override final;
        virtual void DestroyGraphicsPipelineState(IGraphicsPipelineState** ppPipelineState) override final;
		virtual void DestroyRenderPass(IRenderPass** ppRenderPass) override final;
		virtual void DestroyResourceState(IPipelineResourceState** ppResourceState) override final;
        virtual void Destroy() const override final;
        
        virtual void ExecuteCommandList(ICommandList* const * ppLists, uint32 numLists) const override final;
		virtual void ExecuteCommandListAndPresent(ICommandList* const* ppLists, uint32 numLists) const override final;
        virtual void Present() const override final;
        virtual void GPUWaitForFrame() const override final;
        virtual void WaitForGPU() const override final;
        
        virtual void*			    GetNativeHandle() const override final;
        virtual GraphicsDeviceDesc  GetDesc() const override final;
        virtual ITexture*		    GetDepthStencil() const override final;
        virtual ITexture*		    GetRenderTarget() const override final;
        virtual ResourceFormat	    GetBackBufferFormat() const override final;
        virtual uint32			    GetBackBufferIndex() const override final;
        virtual uint32			    GetSwapChainWidth() const override final;
        virtual uint32			    GetSwapChainHeight() const override final;

    private:
        void Init(const GraphicsDeviceDesc& desc);
        bool CreateDepthStencil();
		bool CreateMSAABuffer();
        void ReleaseDepthStencil();
		void ReleaseMSAABuffer();
        void GetNextFrame() const;
        
        virtual bool InternalOnEvent(const Event& event) override final;
        
    private:
		VKNDevice*					m_pDevice;
		VKNBufferManager*			m_pDynamicBufferManager;
		VKNFramebufferCache*		m_pFramebufferCache;
		VKNTexture*					m_pDepthStencil;
		VKNTexture*					m_pMSAABuffer;
		VKNSwapChain*				m_pSwapChain;
		VKNCommandList*				m_pCommandList;
		VKNAllocator*				m_pDeviceAllocator;
        VkQueue						m_GraphicsQueue;
        VkQueue						m_PresentationQueue;
		std::vector<VkFence>		m_Fences;
		std::vector<VkSemaphore>	m_RenderSemaphores;
		std::vector<VkSemaphore>	m_ImageSemaphores;
        mutable uint64				m_CurrentFrame;
        
    public:
		static VKNGraphicsDevice& GetInstance();
    };
}
