#pragma once
#include "Graphics/ITexture.h"
#include "VulkanAllocator.h"
#include "VulkanConversions.inl"

namespace Lambda
{
    class VulkanGraphicsDevice;
    
    
    class VulkanTexture final : public ITexture
    {
        friend class VulkanGraphicsDevice;
        
    public:
        LAMBDA_NO_COPY(VulkanTexture);

        VulkanTexture(const VulkanGraphicsDevice* pVkDevice, IVulkanAllocator* pAllocator, const TextureDesc& desc);
        VulkanTexture(const VulkanGraphicsDevice* pVkDevice, VkImage image, const TextureDesc& desc);
        ~VulkanTexture() = default;
        
        virtual void* GetNativeHandle() const override final;
        virtual TextureDesc GetDesc() const override final;
        
		void Release(VkDevice device);
        void Destroy(VkDevice device);

        void SetResolveResource(VulkanTexture* pResolveResource) const;
        void SetGraphicsPipelineResourceState(VkImageLayout resourceState) const;
        VulkanTexture* GetResolveResource() const;
        VkImageAspectFlags GetAspectFlags() const;
        VkImage GetImage() const;
        VkImageView GetImageView() const;
        VkImageLayout GetCurrentResourceState() const;
        VkFormat GetFormat() const;
        
    private:
        void Init(const VulkanGraphicsDevice* pVkDevice, const TextureDesc& desc);
        void InitFromResource(const VulkanGraphicsDevice* pVkDevice, VkImage image, const TextureDesc& desc);
        void CreateImageView(VkDevice device);
        
    private:
		IVulkanAllocator* const m_pAllocator;
		VulkanAllocation m_Memory;
        VkImage m_Image;
        VkImageView m_View;
        VkImageAspectFlags m_AspectFlags;
        bool m_IsOwner;
        mutable TextureDesc m_Desc;
        mutable VkImageLayout m_CurrentResourceState;
    };
    
    
    inline VkImageAspectFlags VulkanTexture::GetAspectFlags() const
    {
        return m_AspectFlags;
    }
    
    
    inline VkImage VulkanTexture::GetImage() const
    {
        return m_Image;
    }
    
    
    inline VkImageView VulkanTexture::GetImageView() const
    {
        return m_View;
    }
    
    
    inline VkImageLayout VulkanTexture::GetCurrentResourceState() const
    {
        return m_CurrentResourceState;
    }
    
    
    inline void VulkanTexture::SetResolveResource(VulkanTexture* pResolveResource) const
    {
        m_Desc.pResolveResource = pResolveResource;
    }
    
    
    inline void VulkanTexture::SetGraphicsPipelineResourceState(VkImageLayout resourceState) const
    {
        m_CurrentResourceState = resourceState;
    }
    
    
    inline VulkanTexture* VulkanTexture::GetResolveResource() const
    {
        return reinterpret_cast<VulkanTexture*>(m_Desc.pResolveResource);
    }
    
    
    inline VkFormat VulkanTexture::GetFormat() const
    {
        return ConvertResourceFormat(m_Desc.Format);
    }
}
