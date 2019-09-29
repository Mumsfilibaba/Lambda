#include "LambdaPch.h"
#include "VKNTexture.h"
#include "VKNDevice.h"
#include "VKNFramebuffer.h"
#include "VKNUtilities.h"
#include "VKNConversions.inl"

namespace Lambda
{
	//----------
	//VKNTexture
	//----------

    VKNTexture::VKNTexture(IVKNAllocator* pAllocator, const TextureDesc& desc)
        : m_pAllocator(pAllocator),
		m_Memory(),
        m_IsOwner(false),
		m_Image(VK_NULL_HANDLE),
        m_ImageView(VK_NULL_HANDLE),
        m_AspectFlags(0),
        m_Desc(),
        m_ResourceState(VK_IMAGE_LAYOUT_UNDEFINED)
    {
        Init(desc);
    }
    
    
    VKNTexture::VKNTexture(VkImage image, const TextureDesc& desc)
		: m_pAllocator(nullptr),
		m_Memory(),
		m_IsOwner(false),
		m_Image(VK_NULL_HANDLE),
		m_ImageView(VK_NULL_HANDLE),
		m_AspectFlags(0),
		m_Desc(),
		m_ResourceState(VK_IMAGE_LAYOUT_UNDEFINED)
    {
		LAMBDA_ASSERT(image != VK_NULL_HANDLE);
        InitFromResource(image, desc);
    }
    
    
    void VKNTexture::InitFromResource(VkImage image, const TextureDesc& desc)
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
        CreateImageView();
        
    }
    
    
    void VKNTexture::Init(const TextureDesc& desc)
    {
		LAMBDA_ASSERT(m_pAllocator != nullptr);

        //Number of samples (MSAA)
        VkSampleCountFlagBits sampleCount = ConvertSampleCount(desc.SampleCount);
        
        VKNDevice& device = VKNDevice::Get();
		VkSampleCountFlagBits highestSampleCount = device.GetHighestSampleCount();
        if (sampleCount > highestSampleCount)
        {
            LOG_DEBUG_ERROR("Vulkan: TextureDesc::SampleCount (=%u) is higher than the maximum of the device (=%u)\n", sampleCount, highestSampleCount);
            return;
        }
        
        //Set miplevels
        uint32 mipLevels = desc.MipLevels;
        if (desc.Flags & TEXTURE_FLAGS_GENEATE_MIPS)
        {
            mipLevels = uint32(std::floor(std::log2(std::max(desc.Width, desc.Height))));
        }
        

		//Setup
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

		//Create image
        if (vkCreateImage(device.GetDevice(), &info, nullptr, &m_Image) != VK_SUCCESS)
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
		VkMemoryRequirements memoryRequirements = {};
		vkGetImageMemoryRequirements(device.GetDevice(), m_Image, &memoryRequirements);
		if (m_pAllocator->Allocate(m_Memory, memoryRequirements, desc.Usage))
		{
			vkBindImageMemory(device.GetDevice(), m_Image, m_Memory.DeviceMemory, m_Memory.DeviceMemoryOffset);
		}
        else
        {
            LOG_DEBUG_ERROR("Vulkan: Failed to allocate memory for texture\n");
        }
        
        //Create the view
        CreateImageView();
    }
    
    
    void VKNTexture::CreateImageView()
    {
        //Create image views
        VkImageViewCreateInfo viewInfo = {};
        viewInfo.sType      = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.pNext      = nullptr;
        viewInfo.flags      = 0;
        viewInfo.image      = m_Image;
        viewInfo.format     = GetFormat();
        if (m_Desc.Type == TEXTURE_TYPE_2D)
        {
            viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        }
        else
        {
            LOG_DEBUG_ERROR("Vulkan: Unknown Texture-Type\n");
            return;
        }
        viewInfo.subresourceRange.aspectMask        = m_AspectFlags;
        viewInfo.subresourceRange.baseMipLevel      = 0;
        viewInfo.subresourceRange.levelCount        = m_Desc.MipLevels;
        viewInfo.subresourceRange.baseArrayLayer    = 0;
        viewInfo.subresourceRange.layerCount        = 1;
        
		VKNDevice& device = VKNDevice::Get();
        if (vkCreateImageView(device.GetDevice(), &viewInfo, nullptr, &m_ImageView) != VK_SUCCESS)
        {
            LOG_DEBUG_ERROR("Vulkan: Failed to create image view\n");
        }
        else
        {
            LOG_DEBUG_INFO("Vulkan: Created image view. Handle=%p\n", m_ImageView);
        }
    }
    
    
    void* VKNTexture::GetNativeHandle() const
    {
        return reinterpret_cast<void*>(m_Image);
    }
    
    
	TextureDesc VKNTexture::GetDesc() const
    {
        return m_Desc;
    }
    
    
	void VKNTexture::Release(VkDevice device)
	{
		LAMBDA_ASSERT(device != VK_NULL_HANDLE);

		//Remove associated framebuffer if there is any
		VKNFramebufferCache::Get().ReleaseAllContainingTexture(device, this);

		if (m_ImageView != VK_NULL_HANDLE)
		{
			vkDestroyImageView(device, m_ImageView, nullptr);
			m_ImageView = VK_NULL_HANDLE;
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


	void VKNTexture::Destroy(VkDevice device)
    {
		LOG_DEBUG_INFO("Vulkan: Destroying Texture2D '%p'\n", this);
		
		Release(device);
        delete this;
    }
}
