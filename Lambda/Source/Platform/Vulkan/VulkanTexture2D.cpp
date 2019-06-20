#include "VulkanTexture2D.h"
#include "VulkanFramebufferCache.h"

namespace Lambda
{
    VulkanTexture2D::VulkanTexture2D(VkImage image, VkExtent2D extent, ResourceFormat format, OptimizedClearValue clearValue, uint32 flags, uint32 miplevels, uint32 arraySize)
    : m_Texture(VK_NULL_HANDLE),
    m_Views(),
    m_Desc(),
    m_IsOwner(false)
    {
        InitFromResource(image, extent, format, clearValue, flags, miplevels, arraySize);
    }
    
    
    VulkanTexture2D::VulkanTexture2D(VkDevice device, const Texture2DDesc& desc)
        : m_Texture(VK_NULL_HANDLE),
        m_Views(),
        m_Desc(),
        m_IsOwner(false)
    {
        Init(device, desc);
    }
    
    
    void VulkanTexture2D::InitFromResource(VkImage image, VkExtent2D extent, ResourceFormat format, OptimizedClearValue clearValue, uint32 flags, uint32 miplevels, uint32 arraySize)
    {
        //Set texture
        m_Texture = image;
        
        //Set description
        m_Desc.Width        = extent.width;
        m_Desc.Height       = extent.height;
        m_Desc.Flags        = flags;
        m_Desc.MipLevels    = miplevels;
        m_Desc.Format       = format;
        m_Desc.ArraySize    = arraySize;
        m_Desc.ClearValue   = clearValue;
        
        //We are not owners of this image
        m_IsOwner = false;
    }
    
    
    void VulkanTexture2D::Init(VkDevice device, const Texture2DDesc& desc)
    {
        
    }
    
    
    Texture2DDesc VulkanTexture2D::GetDesc() const
    {
        return m_Desc;
    }
    
    
    void VulkanTexture2D::Destroy(VkDevice device)
    {
        assert(device != VK_NULL_HANDLE);
        
        LOG_DEBUG_INFO("Vulkan: Destroying Texture2D '%p'\n", this);
        
        //Remove associated framebuffer if there is any
        VulkanFramebufferCache::ReleaseTexture(device, this);
        
        //Destroy views
        for (auto& view : m_Views)
        {
            if (view != VK_NULL_HANDLE)
            {
                vkDestroyImageView(device, view, nullptr);
                view = VK_NULL_HANDLE;
            }
        }
        
        //Destroy if texture was created from init
        if (m_IsOwner)
        {
            if (m_Texture != VK_NULL_HANDLE)
            {
                vkDestroyImage(device, m_Texture, nullptr);
                m_Texture = VK_NULL_HANDLE;
            }
        }
        
        //Delete me
        delete this;
    }
}
