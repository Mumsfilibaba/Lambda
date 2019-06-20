#pragma once
#include "Graphics/ICommandList.h"
#include <vulkan/vulkan.h>
#include <string>

namespace Lambda
{
    class VulkanTexture2D;
    class VulkanCommandList final : public ICommandList
    {
        friend class VulkanGraphicsDevice;
        
    public:
        LAMBDA_NO_COPY(VulkanCommandList);
        
        VulkanCommandList(VkDevice device, CommandListType type);
        ~VulkanCommandList() = default;
        
        virtual void ClearRenderTarget(ITexture2D* pRenderTarget, float color[4]) override final;
        virtual void ClearDepthStencil(ITexture2D* pDepthStencil, float depth, uint8 stencil) override final;
        
        virtual void SetRenderTarget(ITexture2D* pRenderTarget, ITexture2D* pDepthStencil) override final;
        virtual void SetViewport(const Viewport& viewport) override final;
        virtual void SetScissorRect(const Math::Rectangle& scissorRect) override final;
        virtual void SetPrimtiveTopology(PrimitiveTopology topology) override final;
        virtual void SetGraphicsPipelineState(IGraphicsPipelineState* pPSO) override final;
        virtual void SetVertexBuffer(IBuffer* pBuffer, uint32 slot) override final;
        
        virtual CommandListType GetType() const override final;
        virtual void* GetNativeHandle() const override final;
        
        virtual void TransitionResource(IBuffer* pResource, ResourceState resourceState) override final;
        virtual void TransitionResource(ITexture2D* pResource, ResourceState resourceState) override final;
        
        virtual void VSSetConstantBuffers(const IBuffer* const* ppBuffers, uint32 numBuffers, uint32 startSlot) override final;
        virtual void VSSetTextures(const ITexture2D* const* ppTextures, uint32 numTextures, uint32 startSlot) override final;
        virtual void VSSetSamplers(const ISamplerState* const* ppSamplerStates, uint32 numSamplers, uint32 startSlot) override final;
        
        virtual void HSSetConstantBuffers(const IBuffer* const* ppBuffers, uint32 numBuffers, uint32 startSlot) override final;
        virtual void HSSetTextures(const ITexture2D* const* ppTextures, uint32 numTextures, uint32 startSlot) override final;
        virtual void HSSetSamplers(const ISamplerState* const* ppSamplerStates, uint32 numSamplers, uint32 startSlot) override final;
        
        virtual void DSSetConstantBuffers(const IBuffer* const* ppBuffers, uint32 numBuffers, uint32 startSlot) override final;
        virtual void DSSetTextures(const ITexture2D* const* ppTextures, uint32 numTextures, uint32 startSlot) override final;
        virtual void DSSetSamplers(const ISamplerState* const* ppSamplerStates, uint32 numSamplers, uint32 startSlot) override final;
        
        virtual void GSSetConstantBuffers(const IBuffer* const* ppBuffers, uint32 numBuffers, uint32 startSlot) override final;
        virtual void GSSetTextures(const ITexture2D* const* ppTextures, uint32 numTextures, uint32 startSlot) override final;
        virtual void GSSetSamplers(const ISamplerState* const* ppSamplerStates, uint32 numSamplers, uint32 startSlot) override final;
        
        virtual void PSSetConstantBuffers(const IBuffer* const* ppBuffers, uint32 numBuffers, uint32 startSlot) override final;
        virtual void PSSetTextures(const ITexture2D* const* ppTextures, uint32 numTextures, uint32 startSlot) override final;
        virtual void PSSetSamplers(const ISamplerState* const* ppSamplerStates, uint32 numSamplers, uint32 startSlot) override final;
        
        virtual void UpdateBuffer(IBuffer* pResource, const ResourceData* pData) override final;
        virtual void UpdateTexture(ITexture2D* pResource, const ResourceData* pData, uint32 subresource) override final;
        
        virtual void CopyBuffer(IBuffer* pDst, IBuffer* pSrc) override final;
        
        virtual void DrawInstanced(uint32 vertexCountPerInstance, uint32 instanceCount, uint32 startVertexLocation, uint32 startInstanceLocation) override final;
        
        virtual void SetName(const char* pName) override final;
        
        virtual void Close() override final;
        virtual void Reset() override final;
        
        void Destroy(VkDevice device);
        
    private:
        void Init(VkDevice device, CommandListType type);
        
    private:
        VkCommandPool m_CommandPool;
        VkCommandBuffer m_CommandBuffer;
        CommandListType m_Type;
        std::string m_Name;
        const ITexture2D* m_pRT;
        VkRenderPass m_RenderPass;
    };
}
