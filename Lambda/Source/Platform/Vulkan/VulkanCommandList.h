#pragma once
#include <string>
#include "Graphics/ICommandList.h"
#include "VulkanUploadBuffer.h"
#include "VulkanUtilities.h"

namespace Lambda
{
    class VulkanTexture2D;
	class VulkanResourceState;


    class VulkanCommandList final : public ICommandList
    {
        friend class VulkanGraphicsDevice;
        
    public:
        LAMBDA_NO_COPY(VulkanCommandList);
        
        VulkanCommandList(VkDevice device, CommandListType type);
        ~VulkanCommandList() = default;
        
        virtual void ClearRenderTarget(ITexture2D* pRenderTarget, float color[4]) override final;
        virtual void ClearDepthStencil(ITexture2D* pDepthStencil, float depth, uint8 stencil) override final;
        
		virtual void BeginRenderPass(IRenderPass* pRenderPass) override final;
		virtual void EndRenderPass() override final;

		virtual void SetViewport(const Viewport& viewport) override final;
        virtual void SetScissorRect(const Rectangle& scissorRect) override final;
        virtual void SetGraphicsPipelineState(IGraphicsPipelineState* pPSO) override final;
        virtual void SetVertexBuffer(IBuffer* pBuffer, uint32 slot) override final;
        virtual void SetIndexBuffer(IBuffer* pBuffer) override final;
		virtual void SetResourceState(IResourceState* pResourceState) override final;

        virtual void UpdateBuffer(IBuffer* pResource, const ResourceData* pData) override final;
        virtual void UpdateTexture(ITexture2D* pResource, const ResourceData* pData, uint32 subresource) override final;
        
        virtual void CopyBuffer(IBuffer* pDst, IBuffer* pSrc) override final;
        
        virtual void TransitionBuffer(const IBuffer* pBuffer, ResourceState state) override final;
        virtual void TransitionTexture(const ITexture2D* pTexture, ResourceState state) override final;
        
        virtual void DrawInstanced(uint32 vertexCountPerInstance, uint32 instanceCount, uint32 startVertexLocation, uint32 startInstanceLocation) override final;
        virtual void DrawIndexedInstanced(uint32 indexCountPerInstance, uint32 instanceCount, uint32 startIndexLocation, uint32 baseVertexLocation, uint32 startInstanceLocation) override final;
        
        virtual void SetName(const char* pName) override final;
        
        virtual void Close() override final;
        virtual void Reset() override final;
        
        virtual CommandListType GetType() const override final;
        virtual void* GetNativeHandle() const override final;
        
        void Destroy(VkDevice device);
        
    private:
        void Init(VkDevice device, CommandListType type);

    private:
        VkDevice m_Device;
        VkCommandPool m_CommandPool;
        VkCommandBuffer m_CommandBuffer;
        
        VulkanUploadBuffer m_BufferUpload;
        VulkanUploadBuffer m_TextureUpload;
        
		VulkanResourceState* m_pResourceState;

        VkRenderPass m_BoundRenderPass;
        VkFramebuffer m_BoundFrameBuffer;
        
        CommandListType m_Type;
        std::string m_Name;
    };
}
