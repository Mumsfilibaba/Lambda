#pragma once
#include "Graphics/ICommandList.h"
#include "VulkanUploadBuffer.h"
#include <string>
#include "VulkanHelpers.inl"

namespace Lambda
{
    //Helperstruct containing information about a shaderstage
    struct VulkanShaderStageData
    {
        VkDescriptorBufferInfo  UBInfos[LAMBDA_SHADERSTAGE_UNIFORM_COUNT];
        VkDescriptorImageInfo   TextureInfos[LAMBDA_SHADERSTAGE_TEXTURE_COUNT];
        VkDescriptorImageInfo   SamplerInfos[LAMBDA_SHADERSTAGE_SAMPLER_COUNT];
    };
    
    
    //Forward declarations
    class VulkanTexture2D;
    
    
    //Vulkan Implementation of CommandList
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
        virtual void SetScissorRect(const Rectangle& scissorRect) override final;
        virtual void SetGraphicsPipelineState(IGraphicsPipelineState* pPSO) override final;
        virtual void SetVertexBuffer(IBuffer* pBuffer, uint32 slot) override final;
        virtual void SetIndexBuffer(IBuffer* pBuffer) override final;
        
        virtual void TransitionBuffer(IBuffer* pBuffer, ResourceState resourceState) override final;
        virtual void TransitionTexture(ITexture2D* pTexture, ResourceState resourceState) override final;
        
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
        virtual void DrawIndexedInstanced(uint32 indexCountPerInstance, uint32 instanceCount, uint32 startIndexLocation, uint32 baseVertexLocation, uint32 startInstanceLocation) override final;
        
        virtual void SetName(const char* pName) override final;
        
        virtual void Close() override final;
        virtual void Reset() override final;
        
        virtual CommandListType GetType() const override final;
        virtual void* GetNativeHandle() const override final;
        
        void Destroy(VkDevice device);
        
    private:
        void Init(VkDevice device, CommandListType type);
        void InternalWriteConstantBufferDescriptorsToStage(uint32 shaderStage, uint32 startSlot, const IBuffer* const* ppBuffers, uint32 numBuffers);
        void InternalWriteTextureDescriptorsToStage(uint32 shaderStage, uint32 startSlot, const ITexture2D* const* ppTextures, uint32 numTextures);
        void InternalWriteSamplerDescriptorsToStage(uint32 shaderStage, uint32 startSlot, const ISamplerState* const* ppSamplers, uint32 numSamplers);
        
    private:
        VkDevice m_Device; //Store the device that was used when creating device
        VkCommandPool m_CommandPool;
        VkCommandBuffer m_CommandBuffer;
        
        VulkanUploadBuffer m_BufferUpload;
        VulkanUploadBuffer m_TextureUpload;
        
        VkClearColorValue m_ClearColor;
        
        VkDescriptorSet m_DescriptorSets[LAMBDA_SHADERSTAGE_COUNT];
        VkDescriptorPool m_DescriptorPool;
        VulkanShaderStageData m_ShaderSages[LAMBDA_SHADERSTAGE_COUNT];
        
        CommandListType m_Type;
        std::string m_Name;
        
        //Temp?
        VkRenderPass m_RenderPass;
        VkPipelineLayout m_PipelineLayout;
        const ITexture2D* m_pRT;
        const ITexture2D* m_pDS;
    };
}
