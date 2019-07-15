#include "VulkanTexture2D.h"
#include "VulkanFramebufferCache.h"
#include "VulkanConversions.inl"
#include "VulkanHelpers.inl"

namespace Lambda
{
    VulkanTexture2D::VulkanTexture2D(VkImage image, VkExtent2D extent, ResourceFormat format, OptimizedClearValue clearValue, uint32 flags, uint32 miplevels, uint32 arraySize)
    : m_Texture(VK_NULL_HANDLE),
    m_View(VK_NULL_HANDLE),
    m_DeviceMemory(VK_NULL_HANDLE),
    m_CurrentResourceState(VK_IMAGE_LAYOUT_UNDEFINED),
    m_Desc(),
    m_IsOwner(false)
    {
        InitFromResource(image, extent, format, clearValue, flags, miplevels, arraySize);
    }
    
    
    VulkanTexture2D::VulkanTexture2D(VkDevice device, VkPhysicalDevice adapter, const Texture2DDesc& desc)
        : m_Texture(VK_NULL_HANDLE),
        m_View(VK_NULL_HANDLE),
        m_DeviceMemory(VK_NULL_HANDLE),
        m_CurrentResourceState(VK_IMAGE_LAYOUT_UNDEFINED),
        m_Desc(),
        m_IsOwner(false)
    {
        Init(device, adapter, desc);
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
    
    
    void VulkanTexture2D::Init(VkDevice device, VkPhysicalDevice adapter, const Texture2DDesc& desc)
    {
        //Setup image
        VkImageCreateInfo info = {};
        info.sType                  = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        info.pNext                  = nullptr;
        info.flags                  = 0;
        info.imageType              = VK_IMAGE_TYPE_2D;
        info.extent.width           = desc.Width;
        info.extent.height          = desc.Height;
        info.extent.depth           = 1;
        info.mipLevels              = desc.MipLevels;
        info.arrayLayers            = desc.ArraySize;
        info.pQueueFamilyIndices    = nullptr;
        info.queueFamilyIndexCount  = 0;
        info.format                 = ConvertResourceFormat(desc.Format);
        info.tiling                 = VK_IMAGE_TILING_OPTIMAL;
        info.initialLayout          = VK_IMAGE_LAYOUT_UNDEFINED;
        info.sharingMode            = VK_SHARING_MODE_EXCLUSIVE;
        info.samples                = VK_SAMPLE_COUNT_1_BIT;
        info.usage                  = VK_IMAGE_USAGE_TRANSFER_DST_BIT;
        
        //Set special usage
        if (desc.Flags & TEXTURE_FLAGS_SHADER_RESOURCE)
            info.usage |= VK_IMAGE_USAGE_SAMPLED_BIT;
        
        //Create image
        if (vkCreateImage(device, &info, nullptr, &m_Texture) != VK_SUCCESS)
        {
            LOG_DEBUG_ERROR("Vulkan: Failed to create texture\n");
            return;
        }
        else
        {

            LOG_DEBUG_INFO("Vulkan: Created texture. w=%u, h=%u, format=%s\n", desc.Width, desc.Height, VkFormatToString(info.format));
            
            //Set that we have created the texture and not external memory
            m_IsOwner = true;
            
            //Set desc
            m_Desc = desc;
        }
        
        //Get memory requirements
        VkMemoryRequirements memRequirements = {};
        vkGetImageMemoryRequirements(device, m_Texture, &memRequirements);
        
        //Set memoryproperty based on resource usage
        VkMemoryPropertyFlags properties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
        if (desc.Usage == RESOURCE_USAGE_DYNAMIC)
            properties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
        
        //Find memorytype
        uint32 memoryType = FindMemoryType(adapter, memRequirements.memoryTypeBits, properties);
        
        //Allocate memory
        VkMemoryAllocateInfo allocInfo = {};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.pNext = nullptr;
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex = memoryType;
        
        if (vkAllocateMemory(device, &allocInfo, nullptr, &m_DeviceMemory) != VK_SUCCESS)
        {
            LOG_DEBUG_ERROR("Vulkan: Failed to allocate memory for texture\n");
            return;
        }
        else
        {
            LOG_DEBUG_INFO("Vulkan: Allocated memory for texture\n");
            
            //Bind memory
            vkBindImageMemory(device, m_Texture, m_DeviceMemory, 0);
        }
        
        
        //Create image views
        VkImageViewCreateInfo viewInfo = {};
        viewInfo.sType      = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.pNext      = nullptr;
        viewInfo.flags      = 0;
        viewInfo.image      = m_Texture;
        viewInfo.viewType   = VK_IMAGE_VIEW_TYPE_2D;
        viewInfo.format     = ConvertResourceFormat(desc.Format);
        viewInfo.subresourceRange.aspectMask        = VK_IMAGE_ASPECT_COLOR_BIT;
        viewInfo.subresourceRange.baseMipLevel      = 0;
        viewInfo.subresourceRange.levelCount        = 1;
        viewInfo.subresourceRange.baseArrayLayer    = 0;
        viewInfo.subresourceRange.layerCount        = 1;
        
        if (vkCreateImageView(device, &viewInfo, nullptr, &m_View) != VK_SUCCESS)
        {
            LOG_DEBUG_ERROR("Vulkan: Failed to create image view\n");
        }
        else
        {
            LOG_DEBUG_INFO("Vulkan: Created image view\n");
        }
    }
    
    
    void* VulkanTexture2D::GetNativeHandle() const
    {
        return (void*)m_Texture;
    }
    
    
    Texture2DDesc VulkanTexture2D::GetDesc() const
    {
        return m_Desc;
    }
    
    
    uint32 VulkanTexture2D::GetWidth() const
    {
        return m_Desc.Width;
    }
    
    
    uint32 VulkanTexture2D::GetHeight() const
    {
        return m_Desc.Height;
    }
    
    
    void VulkanTexture2D::Destroy(VkDevice device)
    {
        assert(device != VK_NULL_HANDLE);
        
        LOG_DEBUG_INFO("Vulkan: Destroying Texture2D '%p'\n", this);
        
        //Remove associated framebuffer if there is any
        VulkanFramebufferCache::ReleaseTexture(device, this);
        
        //Destroy views
        if (m_View != VK_NULL_HANDLE)
        {
            vkDestroyImageView(device, m_View, nullptr);
            m_View = VK_NULL_HANDLE;
        }
        
        //Destroy if texture was created from init
        if (m_IsOwner)
        {
            //Destroy image
            if (m_Texture != VK_NULL_HANDLE)
            {
                vkDestroyImage(device, m_Texture, nullptr);
                m_Texture = VK_NULL_HANDLE;
            }
            
            //Free memory
            if (m_DeviceMemory != VK_NULL_HANDLE)
            {
                vkFreeMemory(device, m_DeviceMemory, nullptr);
                m_DeviceMemory = VK_NULL_HANDLE;
            }
        }
        
        //Delete me
        delete this;
    }
}
