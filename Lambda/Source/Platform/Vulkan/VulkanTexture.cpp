#include "LambdaPch.h"
#include "VulkanTexture.h"
#include "VulkanGraphicsDevice.h"
#include "VulkanFramebuffer.h"
#include "VulkanUtilities.h"
#include "VulkanConversions.inl"

namespace Lambda
{
    VulkanTexture::VulkanTexture(const VulkanGraphicsDevice* pVkDevice, IVulkanAllocator* pAllocator, const TextureDesc& desc)
        : m_pAllocator(pAllocator),
		m_Memory(),
		m_Image(VK_NULL_HANDLE),
        m_View(VK_NULL_HANDLE),
        m_AspectFlags(0),
        m_IsOwner(false),
        m_Desc(),
        m_CurrentResourceState(VK_IMAGE_LAYOUT_UNDEFINED)
    {
		LAMBDA_ASSERT(pVkDevice != nullptr);
        Init(pVkDevice, desc);
    }
    
    
    VulkanTexture::VulkanTexture(const VulkanGraphicsDevice* pVkDevice, VkImage image, const TextureDesc& desc)
        : m_pAllocator(nullptr),
		m_Memory(),
		m_Image(VK_NULL_HANDLE),
        m_View(VK_NULL_HANDLE),
        m_AspectFlags(0),
        m_IsOwner(false),
        m_Desc(),
        m_CurrentResourceState(VK_IMAGE_LAYOUT_UNDEFINED)
    {
		LAMBDA_ASSERT(pVkDevice != nullptr);
		LAMBDA_ASSERT(image != VK_NULL_HANDLE);
        InitFromResource(pVkDevice, image, desc);
    }
    
    
    void VulkanTexture::InitFromResource(const VulkanGraphicsDevice* pVkDevice, VkImage image, const TextureDesc& desc)
    {
        //Init data
        m_Desc = desc;
        m_Image = image;
        m_IsOwner = false;
        
        //Set aspectflags
        if (desc.Flags & TEXTURE_FLAGS_SHADER_RESOURCE || desc.Flags & TEXTURE_FLAGS_RENDER_TARGET)
        {
            m_AspectFlags |= VK_IMAGE_ASPECT_COLOR_BIT;
        }
        if (desc.Flags & TEXTURE_FLAGS_DEPTH_STENCIL)
        {
            if (desc.Format == FORMAT_D24_UNORM_S8_UINT || desc.Format == FORMAT_D32_FLOAT_S8X24_UINT)
            {
                m_AspectFlags |= VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
            }
            else
            {
                m_AspectFlags |= VK_IMAGE_ASPECT_DEPTH_BIT;
            }
        }
        
        //Create view
        VkDevice device = reinterpret_cast<VkDevice>(pVkDevice->GetNativeHandle());
        CreateImageView(device);
        
    }
    
    
    void VulkanTexture::Init(const VulkanGraphicsDevice* pVkDevice, const TextureDesc& desc)
    {
		LAMBDA_ASSERT(m_pAllocator != nullptr);

        //Set miplevels
        uint32 mipLevels = desc.MipLevels;
        if (desc.Flags & TEXTURE_FLAGS_GENEATE_MIPS)
        {
            mipLevels = uint32(std::floor(std::log2(std::max(desc.Width, desc.Height))));
        }
        
		//Create image
        VkImageCreateInfo info = {};
        info.sType                  = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        info.pNext                  = nullptr;
        info.flags                  = 0;

		//Set type
		if (desc.Type == TEXTURE_TYPE_2D)
		{
			info.imageType = VK_IMAGE_TYPE_2D;
		}
		else
		{
			LOG_DEBUG_ERROR("Vulkan: Unknown Texture-Type\n");
			return;
		}
        
		info.extent.width           = desc.Width;
        info.extent.height          = desc.Height;
        info.extent.depth           = desc.Depth;
        info.mipLevels              = mipLevels;
        info.arrayLayers            = desc.ArraySize;
        info.pQueueFamilyIndices    = nullptr;
        info.queueFamilyIndexCount  = 0;
        info.format                 = ConvertResourceFormat(desc.Format);
        info.tiling                 = VK_IMAGE_TILING_OPTIMAL;
        info.initialLayout          = VK_IMAGE_LAYOUT_UNDEFINED;
        info.sharingMode            = VK_SHARING_MODE_EXCLUSIVE;
        info.samples                = ConvertSampleCount(desc.SampleCount);


        //Set special usage
        if (desc.Flags & TEXTURE_FLAGS_GENEATE_MIPS)
        {
            info.usage |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
        }
        if (desc.Flags & TEXTURE_FLAGS_SHADER_RESOURCE)
        {
            info.usage |= VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
            m_AspectFlags |= VK_IMAGE_ASPECT_COLOR_BIT;
        }
		if (desc.Flags & TEXTURE_FLAGS_RENDER_TARGET)
		{
			info.usage |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT;
			m_AspectFlags |= VK_IMAGE_ASPECT_COLOR_BIT;
		}
        if (desc.Flags & TEXTURE_FLAGS_DEPTH_STENCIL)
        {
            info.usage |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
			if (info.format == VK_FORMAT_D24_UNORM_S8_UINT || info.format == VK_FORMAT_D32_SFLOAT_S8_UINT)
			{
                m_AspectFlags |= VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
			}
			else
			{
                m_AspectFlags |= VK_IMAGE_ASPECT_DEPTH_BIT;
			}
        }


        VkDevice device = reinterpret_cast<VkDevice>(pVkDevice->GetNativeHandle());
        if (vkCreateImage(device, &info, nullptr, &m_Image) != VK_SUCCESS)
        {
            LOG_DEBUG_ERROR("Vulkan: Failed to create image\n");
            return;
        }
        else
        {

            LOG_DEBUG_INFO("Vulkan: Created image. w=%u, h=%u, format=%s, adress=%p\n", desc.Width, desc.Height, VkFormatToString(info.format), m_Image);
            m_Desc = desc;
            m_Desc.MipLevels = mipLevels;

            //Set that we have created the texture and not external memory
            m_IsOwner = true;
        }
        

		//Allocate memory
		VkMemoryRequirements memRequirements = {};
		vkGetImageMemoryRequirements(device, m_Image, &memRequirements);

		//Set memoryproperty based on resource usage
		VkMemoryPropertyFlags properties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
		if (desc.Usage == RESOURCE_USAGE_DYNAMIC)
		{
			properties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
		}

		VkPhysicalDevice physicalDevice = pVkDevice->GetPhysicalDevice();
		uint32 memoryType = FindMemoryType(physicalDevice, memRequirements.memoryTypeBits, properties);
		m_Memory = m_pAllocator->Allocate(memRequirements.size, memRequirements.alignment, memoryType);
		if (m_Memory.Memory != VK_NULL_HANDLE)
		{
			vkBindImageMemory(device, m_Image, m_Memory.Memory, m_Memory.Offset);
		}

        
        //Create the view
        CreateImageView(device);
    }
    
    
    void VulkanTexture::CreateImageView(VkDevice device)
    {
        //Create image views
        VkImageViewCreateInfo viewInfo = {};
        viewInfo.sType      = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.pNext      = nullptr;
        viewInfo.flags      = 0;
        viewInfo.image      = m_Image;
        
        if (m_Desc.Type == TEXTURE_TYPE_2D)
        {
            viewInfo.viewType   = VK_IMAGE_VIEW_TYPE_2D;
        }
        
        viewInfo.format     = GetFormat();
        viewInfo.subresourceRange.aspectMask        = m_AspectFlags;
        viewInfo.subresourceRange.baseMipLevel      = 0;
        viewInfo.subresourceRange.levelCount        = m_Desc.MipLevels;
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
    
    
    void* VulkanTexture::GetNativeHandle() const
    {
        return reinterpret_cast<void*>(m_Image);
    }
    
    
	TextureDesc VulkanTexture::GetDesc() const
    {
        return m_Desc;
    }
    
    
	void VulkanTexture::Release(VkDevice device)
	{
		LAMBDA_ASSERT(device != VK_NULL_HANDLE);

		//Remove associated framebuffer if there is any
		VulkanFramebufferCache::ReleaseAllContainingTexture(device, this);

		if (m_View != VK_NULL_HANDLE)
		{
			vkDestroyImageView(device, m_View, nullptr);
			m_View = VK_NULL_HANDLE;
		}

		//Destroy if texture was created from init
		if (m_IsOwner)
		{
			if (m_Image != VK_NULL_HANDLE)
			{
				m_pAllocator->Deallocate(m_Memory);

				vkDestroyImage(device, m_Image, nullptr);
				m_Image = VK_NULL_HANDLE;
			}
		}
	}


	void VulkanTexture::Destroy(VkDevice device)
    {
		LOG_DEBUG_INFO("Vulkan: Destroying Texture2D '%p'\n", this);
		
		Release(device);
        delete this;
    }
}
