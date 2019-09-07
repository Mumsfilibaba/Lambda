#pragma once
#include "Graphics/ITexture.h"
#include <vulkan/vulkan.h>
#include <vector>

namespace Lambda
{
    struct VulkanTextureDesc
    {
        VkImage Image                   = VK_NULL_HANDLE;
        VkExtent2D Extent               = { 0, 0};
        VkFormat Format                 = VK_FORMAT_UNDEFINED;
        VkClearValue ClearValue;
        bool Depth                      = false;
        VkImageUsageFlags UsageFlags    = 0;
        VkImageAspectFlags AspectFlags  = 0;
        VkSampleCountFlags Samples      = 0;
        uint32 MipLevels                = 1;
        uint32 ArraySize                = 1;
    };
    
    
    class VulkanTexture final : public ITexture
    {
        friend class VulkanGraphicsDevice;
        friend class VulkanCommandList;
        friend class VulkanFramebufferCache;
        
    public:
        LAMBDA_NO_COPY(VulkanTexture);

        VulkanTexture(VkDevice device, const VulkanTextureDesc& desc);
        VulkanTexture(VkDevice device, VkPhysicalDevice adapter, const TextureDesc& desc);
        VulkanTexture() = default;
        
        virtual void* GetNativeHandle() const override final;
        virtual TextureDesc GetDesc() const override final;
        virtual uint32 GetMipLevels() const override final;
        virtual uint32 GetWidth() const override final;
        virtual uint32 GetHeight() const override final;
        
        VkImageAspectFlags GetAspectFlags() const;
        VkImage GetImage() const;
        VkImageView GetImageView() const;
        VkImageLayout GetCurrentResourceState() const;
        VkFormat GetFormat() const;
        
        void SetCurrentResourceState(VkImageLayout resourceState) const;
        
        void Destroy(VkDevice device);
        
    private:
        void Init(VkDevice device, VkPhysicalDevice adapter, const TextureDesc& desc);
        void InitFromResource(VkDevice device, const VulkanTextureDesc& desc);
        void CreateImageView(VkDevice device);
        
    private:
        VkImage m_Texture;
        VkImageView m_View;
        VkDeviceMemory m_DeviceMemory;
        VkImageAspectFlags m_AspectFlags;
        VkFormat m_Format;
        mutable VkImageLayout m_CurrentResourceState;
		TextureDesc m_Desc;
        bool m_IsOwner;
    };
    
    
    inline VkImageAspectFlags VulkanTexture::GetAspectFlags() const
    {
        return m_AspectFlags;
    }
    
    
    inline VkImage VulkanTexture::GetImage() const
    {
        return m_Texture;
    }
    
    
    inline VkImageView VulkanTexture::GetImageView() const
    {
        return m_View;
    }
    
    
    inline VkImageLayout VulkanTexture::GetCurrentResourceState() const
    {
        return m_CurrentResourceState;
    }
    
    
    inline void VulkanTexture::SetCurrentResourceState(VkImageLayout resourceState) const
    {
        m_CurrentResourceState = resourceState;
    }
    
    
    inline VkFormat VulkanTexture::GetFormat() const
    {
        return m_Format;
    }
}
