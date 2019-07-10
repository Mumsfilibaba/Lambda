#pragma once
#include "Graphics/ITexture2D.h"
#include <vulkan/vulkan.h>
#include <vector>

namespace Lambda
{
    class VulkanTexture2D final : public ITexture2D
    {
        friend class VulkanGraphicsDevice;
        friend class VulkanCommandList;
        friend class VulkanFramebufferCache;
        
    public:
        LAMBDA_NO_COPY(VulkanTexture2D);

        VulkanTexture2D(VkImage image, VkExtent2D extent, ResourceFormat format, OptimizedClearValue clearValue, uint32 flags, uint32 miplevels, uint32 arraySize);
        VulkanTexture2D(VkDevice device, VkPhysicalDevice adapter, const Texture2DDesc& desc);
        VulkanTexture2D() = default;
        
        virtual void* GetNativeHandle() const override final;
        virtual Texture2DDesc GetDesc() const override final;
        virtual uint32 GetWidth() const override final;
        virtual uint32 GetHeight() const override final;
        
        VkImageLayout GetCurrentResourceState() const;
        void SetCurrentResourceState(VkImageLayout resourceState);
        
        void Destroy(VkDevice device);
        
    private:
        void Init(VkDevice device, VkPhysicalDevice adapter, const Texture2DDesc& desc);
        void InitFromResource(VkImage image, VkExtent2D extent, ResourceFormat format, OptimizedClearValue clearValue, uint32 flags, uint32 miplevels, uint32 arraySize);
        
        VkImageView GetImageView() const;
        void SetImageView(VkImageView imageView);
        
    private:
        VkImage m_Texture;
        VkImageView m_View;
        VkDeviceMemory m_DeviceMemory;
        VkImageLayout m_CurrentResourceState;
        Texture2DDesc m_Desc;
        bool m_IsOwner;
    };
    
    
    inline VkImageView VulkanTexture2D::GetImageView() const
    {
        return m_View;
    }
    
    
    inline void VulkanTexture2D::SetImageView(VkImageView imageView)
    {
        m_View = imageView;
    }
    
    
    inline VkImageLayout VulkanTexture2D::GetCurrentResourceState() const
    {
        return m_CurrentResourceState;
    }
    
    
    inline void VulkanTexture2D::SetCurrentResourceState(VkImageLayout resourceState)
    {
        m_CurrentResourceState = resourceState;
    }
}
