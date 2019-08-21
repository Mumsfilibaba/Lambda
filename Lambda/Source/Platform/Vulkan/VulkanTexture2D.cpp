#include "LambdaPch.h"
#include "VulkanTexture2D.h"
#include "VulkanFramebuffer.h"
#include "VulkanConversions.inl"
#include "VulkanUtilities.h"

namespace Lambda
{
    VulkanTexture2D::VulkanTexture2D(VkDevice device, const VulkanTextureDesc& desc)
        : m_Texture(VK_NULL_HANDLE),
        m_View(VK_NULL_HANDLE),
        m_DeviceMemory(VK_NULL_HANDLE),
        m_AspectFlags(0),
        m_CurrentResourceState(VK_IMAGE_LAYOUT_UNDEFINED),
        m_Desc(),
        m_IsOwner(false)
    {
        InitFromResource(device, desc);
    }
    
    
    VulkanTexture2D::VulkanTexture2D(VkDevice device, VkPhysicalDevice adapter, const Texture2DDesc& desc)
        : m_Texture(VK_NULL_HANDLE),
        m_View(VK_NULL_HANDLE),
        m_DeviceMemory(VK_NULL_HANDLE),
        m_AspectFlags(0),
        m_CurrentResourceState(VK_IMAGE_LAYOUT_UNDEFINED),
        m_Desc(),
        m_IsOwner(false)
    {
        Init(device, adapter, desc);
    }
    
    
    void VulkanTexture2D::InitFromResource(VkDevice device, const VulkanTextureDesc& desc)
    {
        //Set texture
        m_Texture = desc.Image;
        
        //Set description
        m_Desc.Width        = desc.Extent.width;
        m_Desc.Height       = desc.Extent.height;
        m_Desc.MipLevels    = desc.MipLevels;
        m_Desc.Format       = ConvertVkFormat(desc.Format);
        m_Desc.ArraySize    = desc.ArraySize;
        
        //Should the depth or the color value be used?
        if (desc.Depth)
        {
            m_Desc.ClearValue.Depth = desc.ClearValue.depthStencil.depth;
            m_Desc.ClearValue.Stencil = desc.ClearValue.depthStencil.stencil;
        }
        else
        {
            memcpy(m_Desc.ClearValue.Color, desc.ClearValue.color.float32, sizeof(float) * 4);
        }
        
        //Set aspect flags
        m_AspectFlags = desc.AspectFlags;
        
        //Set usage
        m_Desc.Usage = RESOURCE_USAGE_DEFAULT;
        
        //Set flags
        if (desc.UsageFlags | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT)
            m_Desc.Flags |= TEXTURE_FLAGS_RENDER_TARGET;
        if (desc.UsageFlags | VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT)
            m_Desc.Flags |= TEXTURE_FLAGS_DEPTH_STENCIL;
        if (desc.UsageFlags | VK_IMAGE_USAGE_SAMPLED_BIT)
            m_Desc.Flags |= TEXTURE_FLAGS_SHADER_RESOURCE;
        
        //Set samples
        if (desc.Samples == VK_SAMPLE_COUNT_1_BIT)
            m_Desc.SampleCount = 1;
        if (desc.Samples == VK_SAMPLE_COUNT_2_BIT)
            m_Desc.SampleCount = 2;
        if (desc.Samples == VK_SAMPLE_COUNT_4_BIT)
            m_Desc.SampleCount = 4;
        if (desc.Samples == VK_SAMPLE_COUNT_8_BIT)
            m_Desc.SampleCount = 8;
        if (desc.Samples == VK_SAMPLE_COUNT_16_BIT)
            m_Desc.SampleCount = 16;
        
        //We are not owners of this image
        m_IsOwner = false;
        
        //Set vkformat
        m_Format = desc.Format;
        
        //Create view
        CreateImageView(device);
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

        //Set special usage
        if (desc.Flags & TEXTURE_FLAGS_SHADER_RESOURCE)
        {
            //Set usage
            info.usage |= VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
            
            //Set aspect flag
            m_AspectFlags |= VK_IMAGE_ASPECT_COLOR_BIT;
        }
        if (desc.Flags & TEXTURE_FLAGS_DEPTH_STENCIL)
        {
            //Set usage
            info.usage |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
            
            //Set aspect flag
            if (info.format == VK_FORMAT_D24_UNORM_S8_UINT || info.format == VK_FORMAT_D32_SFLOAT_S8_UINT)
                m_AspectFlags |= VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
            else
                m_AspectFlags |= VK_IMAGE_ASPECT_DEPTH_BIT;
        }

        
        //Create image
        if (vkCreateImage(device, &info, nullptr, &m_Texture) != VK_SUCCESS)
        {
            LOG_DEBUG_ERROR("Vulkan: Failed to create image\n");
            return;
        }
        else
        {

            LOG_DEBUG_INFO("Vulkan: Created image. w=%u, h=%u, format=%s, adress=%p\n", desc.Width, desc.Height, VkFormatToString(info.format), m_Texture);
            
            //Set that we have created the texture and not external memory
            m_IsOwner = true;
            
            //Set desc
            m_Desc = desc;
            m_Format = info.format;
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
            
            //Create view
            CreateImageView(device);
        }
    }
    
    
    void VulkanTexture2D::CreateImageView(VkDevice device)
    {
        //Create image views
        VkImageViewCreateInfo viewInfo = {};
        viewInfo.sType      = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.pNext      = nullptr;
        viewInfo.flags      = 0;
        viewInfo.image      = m_Texture;
        viewInfo.viewType   = VK_IMAGE_VIEW_TYPE_2D;
        viewInfo.format     = ConvertResourceFormat(m_Desc.Format);
        viewInfo.subresourceRange.aspectMask        = m_AspectFlags;
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
        VulkanFramebufferCache::ReleaseAllContainingTexture(device, this);
        
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
