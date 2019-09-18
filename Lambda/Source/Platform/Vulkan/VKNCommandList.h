#pragma once
#include <string>
#include "Graphics/ICommandList.h"
#include "VKNUtilities.h"

namespace Lambda
{
    class VKNTexture;
	class IVKNAllocator;
    class VKNRenderPass;
	class VKNUploadBuffer;
    class VKNGraphicsDevice;
	class VKNPipelineResourceState;

	//--------------
	//VKNCommandList
	//--------------

    class VKNCommandList final : public ICommandList
    {
        friend class VKNGraphicsDevice;
        
    public:
        LAMBDA_NO_COPY(VKNCommandList);
        
        VKNCommandList(IVKNAllocator* pAllocator, CommandListType type);
        ~VKNCommandList() = default;
        
        virtual void ClearRenderTarget(ITexture* pRenderTarget, float color[4]) override final;
        virtual void ClearDepthStencil(ITexture* pDepthStencil, float depth, uint8 stencil) override final;
        
		virtual void BeginRenderPass(IRenderPass* pRenderPass) override final;
		virtual void EndRenderPass() override final;

        virtual void ResetQuery(IQuery* pQuery) override final;
        virtual void WriteTimeStamp(IQuery* pQuery, PipelineStage stage) override final;
        
		virtual void SetViewport(const Viewport& viewport) override final;
        virtual void SetScissorRect(const Rectangle& scissorRect) override final;
        virtual void SetVertexBuffer(IBuffer* pBuffer, uint32 slot) override final;
        virtual void SetIndexBuffer(IBuffer* pBuffer) override final;
        virtual void SetGraphicsPipelineState(IGraphicsPipelineState* pPSO) override final;
		virtual void SetGraphicsPipelineResourceState(IPipelineResourceState* pResourceState) override final;

        virtual void UpdateBuffer(IBuffer* pResource, const ResourceData* pData) override final;
        virtual void UpdateTexture(ITexture* pResource, const ResourceData* pData, uint32 mipLevel) override final;
        
        virtual void TransitionBuffer(const IBuffer* pBuffer, ResourceState state) override final;
        virtual void TransitionTexture(const ITexture* pTexture, ResourceState state, uint32 startMipLevel, uint32 numMipLevels) override final;
        
        virtual void CopyBuffer(IBuffer* pDst, IBuffer* pSrc) override final;

        virtual void DrawInstanced(uint32 vertexCountPerInstance, uint32 instanceCount, uint32 startVertexLocation, uint32 startInstanceLocation) override final;
        virtual void DrawIndexedInstanced(uint32 indexCountPerInstance, uint32 instanceCount, uint32 startIndexLocation, uint32 baseVertexLocation, uint32 startInstanceLocation) override final;
        
        virtual void SetName(const char* pName) override final;
        
        virtual void Close() override final;
        virtual void Reset() override final;
        
        virtual CommandListType GetType() const override final;
        virtual void*			GetNativeHandle() const override final;
        
        void CommitResources();
        void BlitTexture(VKNTexture* pDst, uint32 dstWidth, uint32 dstHeight, uint32 dstMipLevel, VKNTexture* pSrc, uint32 srcWidth, uint32 srcHeight, uint32 srcMipLevel);
        void Destroy(VkDevice device);
        
    private:
        void Init(IVKNAllocator* pAllocator, CommandListType type);

    private:
        VkCommandPool				m_CommandPool;
        VkCommandBuffer				m_CommandBuffer;  
		IVKNAllocator*				m_pAllocator;
        VKNUploadBuffer*			m_pBufferUpload;
        VKNUploadBuffer*			m_pTextureUpload;
        VKNRenderPass*				m_pRenderPass;
		VKNPipelineResourceState*	m_pResourceState;
        CommandListType				m_Type;
        std::string					m_Name;
    };
}
