#include "LambdaPch.h"
#include "VKNTexture.h"
#include "VKNDevice.h"
#include "VKNDeviceContext.h"
#include "VKNFramebufferCache.h"
#include "VKNUtilities.h"
#include "VKNConversions.inl"

namespace Lambda
{
	//----------
	//VKNTexture
	//----------

    VKNTexture::VKNTexture(VKNDevice* pDevice, const ResourceData* pInitalData, const TextureDesc& desc)
        : TextureBase<VKNDevice>(pDevice, desc),
		m_Memory(),
        m_IsOwner(false),
		m_VkImage(VK_NULL_HANDLE),
        m_ImageView(VK_NULL_HANDLE),
        m_VkAspectFlags(0)
    {
		//Add a ref to the refcounter
		this->AddRef();
        Init(pInitalData, desc);
    }
    
    
    VKNTexture::VKNTexture(VKNDevice* pDevice, VkImage image, const TextureDesc& desc)
		: TextureBase<VKNDevice>(pDevice, desc),
		m_Memory(),
		m_IsOwner(false),
		m_VkImage(VK_NULL_HANDLE),
		m_ImageView(VK_NULL_HANDLE),
		m_VkAspectFlags(0)
    {
		LAMBDA_ASSERT(image != VK_NULL_HANDLE);
		
		//Add a ref to the refcounter
		this->AddRef();
        InitFromResource(image, desc);
    }
    

	VKNTexture::~VKNTexture()
	{
		LOG_DEBUG_INFO("Vulkan: Destroying Texture2D '%p'\n", this);

		//Remove the image from the global layout
		if (VKNResourceLayoutTracker::HasGlobalLayout(m_VkImage))
		{
			VKNResourceLayoutTracker::RemoveGlobalLayout(m_VkImage);
		}

		//Destroy view
		if (m_ImageView != VK_NULL_HANDLE)
		{
			//Release all framebuffers with this imageview
			VKNFramebufferCache& cache = VKNFramebufferCache::Get();
			cache.OnReleaseImageView(m_ImageView);

			m_pDevice->SafeReleaseVulkanResource<VkImageView>(m_ImageView);
		}

		//Destroy if texture was created from init
		if (m_IsOwner)
		{
			if (m_VkImage != VK_NULL_HANDLE)
			{
				m_pDevice->Deallocate(m_Memory);
				m_pDevice->SafeReleaseVulkanResource<VkImage>(m_VkImage);
			}
		}
	}

    
    void VKNTexture::InitFromResource(VkImage image, const TextureDesc& desc)
    {
        //Init data
        m_VkImage	= image;
        m_IsOwner	= false;
        
        //Set aspectflags
        if (desc.Flags & TEXTURE_FLAGS_SHADER_RESOURCE || desc.Flags & TEXTURE_FLAGS_RENDER_TARGET)
            m_VkAspectFlags |= VK_IMAGE_ASPECT_COLOR_BIT;

        if (desc.Flags & TEXTURE_FLAGS_DEPTH_STENCIL)
        {
            if (desc.Format == FORMAT_D24_UNORM_S8_UINT || desc.Format == FORMAT_D32_FLOAT_S8X24_UINT)
                m_VkAspectFlags |= VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
            else
                m_VkAspectFlags |= VK_IMAGE_ASPECT_DEPTH_BIT;
        }
        
        //Create view
        CreateImageView();
    }
    
    
    void VKNTexture::Init(const ResourceData* pInitalData, const TextureDesc& desc)
    {
        //Number of samples (MSAA)
        VkSampleCountFlagBits sampleCount = ConvertSampleCount(desc.SampleCount);
		VkSampleCountFlagBits highestSampleCount = m_pDevice->GetHighestSampleCount();
        if (sampleCount > highestSampleCount)
        {
            LOG_DEBUG_ERROR("Vulkan: TextureDesc::SampleCount (=%u) is higher than the maximum of the device (=%u)\n", sampleCount, highestSampleCount);
            return;
        }
        
        //Set miplevels
        uint32 mipLevels = desc.MipLevels;
        if (desc.Flags & TEXTURE_FLAGS_GENEATE_MIPS)
            mipLevels = uint32(std::floor(std::log2(std::max(desc.Width, desc.Height))));      

		//Setup
        VkImageCreateInfo info = {};
        info.sType  = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        info.pNext  = nullptr;
        info.flags  = 0;
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
        info.format                 = ConvertFormat(desc.Format);
        info.tiling                 = VK_IMAGE_TILING_OPTIMAL;
        info.initialLayout          = VK_IMAGE_LAYOUT_UNDEFINED;
        info.sharingMode            = VK_SHARING_MODE_EXCLUSIVE;
        info.samples                = ConvertSampleCount(desc.SampleCount);
        
		if (desc.Flags & TEXTURE_FLAGS_GENEATE_MIPS || info.samples > 1)
		{
			//VK_IMAGE_USAGE_TRANSFER_SRC_BIT - We we are using msaa, we should be able to resolve the image
			//VK_IMAGE_USAGE_TRANSFER_SRC_BIT - We need to blit from a previous mip so we have to be able to use it as a copy source
			info.usage |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
		}
        if (desc.Flags & TEXTURE_FLAGS_SHADER_RESOURCE)
        {
            m_VkAspectFlags |= VK_IMAGE_ASPECT_COLOR_BIT;
			//VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT - Texture, VK_IMAGE_USAGE_TRANSFER_DST_BIT - So we can load up inital data
            info.usage |= VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
        }
		if (desc.Flags & TEXTURE_FLAGS_RENDER_TARGET)
		{
			m_VkAspectFlags |= VK_IMAGE_ASPECT_COLOR_BIT;
			//VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT - Rendertarget, VK_IMAGE_USAGE_TRANSFER_DST_BIT -So we can clear the rendertarget
			info.usage |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
			
		}
        if (desc.Flags & TEXTURE_FLAGS_DEPTH_STENCIL)
        {
			//VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT - Depth, VK_IMAGE_USAGE_TRANSFER_DST_BIT -So we can clear the depthstencil
            info.usage |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
			
			//Set stencil aspect if format has a stencil-part
			if (info.format == VK_FORMAT_D24_UNORM_S8_UINT || info.format == VK_FORMAT_D32_SFLOAT_S8_UINT)
                m_VkAspectFlags |= VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
			else
                m_VkAspectFlags |= VK_IMAGE_ASPECT_DEPTH_BIT;
        }

		//Create image
        if (vkCreateImage(m_pDevice->GetVkDevice(), &info, nullptr, &m_VkImage) != VK_SUCCESS)
        {
            LOG_DEBUG_ERROR("Vulkan: Failed to create Image\n");
            return;
        }
        else
        {
            m_Desc.MipLevels = mipLevels;
            //Set that we have created the texture and not external memory
            m_IsOwner = true;

			if (m_Desc.pName)
			{
				LOG_DEBUG_INFO("Vulkan: Created Image '%p'. w=%u, h=%u, format=%s, name='%s'\n", m_VkImage, desc.Width, desc.Height, VkFormatToString(info.format), m_Desc.pName);
				m_pDevice->SetVulkanObjectName(VK_OBJECT_TYPE_IMAGE, (uint64)m_VkImage, std::string(m_Desc.pName));
			}
			else
			{
				LOG_DEBUG_INFO("Vulkan: Created Image '%p'. w=%u, h=%u, format=%s\n", m_VkImage, desc.Width, desc.Height, VkFormatToString(info.format));
			}
        }
        
		//Allocate memory
		if (!m_pDevice->AllocateImage(m_Memory, m_VkImage, m_Desc.Usage))
		{
			LOG_DEBUG_ERROR("Vulkan: Failed to allocate memory for Image '%p'\n", m_VkImage);
			return;
		}
        
		//Handle inital data
		VKNDeviceContext* pContext = m_pDevice->GetVKNImmediateContext();
		if (pInitalData)
		{
			LAMBDA_ASSERT(pInitalData->pData != nullptr && pInitalData->SizeInBytes != 0);

			//Create staging buffer
			VkBufferCreateInfo sbInfo = {};
			sbInfo.sType					= VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
			sbInfo.pNext					= nullptr;
			sbInfo.flags					= 0;
			sbInfo.usage					= VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
			sbInfo.size						= pInitalData->SizeInBytes;
			sbInfo.queueFamilyIndexCount	= 0;
			sbInfo.pQueueFamilyIndices		= nullptr;
			sbInfo.sharingMode				= VK_SHARING_MODE_EXCLUSIVE;
			
			VkBuffer stagingBuffer = VK_NULL_HANDLE;
			if (vkCreateBuffer(m_pDevice->GetVkDevice(), &sbInfo, nullptr, &stagingBuffer) != VK_SUCCESS)
			{
				LOG_DEBUG_ERROR("Vulkan: Failed to create Staging-Buffer\n");
				return;
			}
			else
			{
				LOG_DEBUG_INFO("Vulkan: Created Staging-Buffer '%p'\n", stagingBuffer);
			}

			//Allocate memory
			VKNAllocation stagingMemory = {};
			if (!m_pDevice->AllocateBuffer(stagingMemory, stagingBuffer, RESOURCE_USAGE_DYNAMIC))
			{
				LOG_DEBUG_ERROR("Vulkan: Failed to allocate memory for Staging-Buffer '%p'\n", stagingBuffer);
				return;
			}
			else
			{
				//Copy over data
				memcpy(stagingMemory.pHostMemory, pInitalData->pData, pInitalData->SizeInBytes);
			}
			
			//Copy buffer to image
			pContext->TransitionTexture(this, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_REMAINING_MIP_LEVELS);
			pContext->CopyBufferToImage(m_VkImage, 0, m_VkAspectFlags, m_Desc.Width, m_Desc.Height, m_Desc.Depth, stagingBuffer, 0, pInitalData->SizeInBytes);

			//Delete the stagingbuffer
			m_pDevice->Deallocate(stagingMemory);
			m_pDevice->SafeReleaseVulkanResource<VkBuffer>(stagingBuffer);
		}
		else
		{
			//A texture needs to be in a general layout
			pContext->TransitionTexture(this, VK_IMAGE_LAYOUT_GENERAL, VK_REMAINING_MIP_LEVELS);
		}

		//Release context
		pContext->Release();

        //Create the view
        CreateImageView();
    }
    
    
    void VKNTexture::CreateImageView()
    {
        //Create image views
        VkImageViewCreateInfo viewInfo = {};
        viewInfo.sType  = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.pNext  = nullptr;
        viewInfo.flags  = 0;
        viewInfo.image  = m_VkImage;
        viewInfo.format = GetVkFormat();
        if (m_Desc.Type == TEXTURE_TYPE_2D)
        {
            viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        }
        else
        {
            LOG_DEBUG_ERROR("Vulkan: Unknown Texture-Type\n");
            return;
        }
        viewInfo.subresourceRange.aspectMask        = m_VkAspectFlags;
        viewInfo.subresourceRange.baseMipLevel      = 0;
        viewInfo.subresourceRange.levelCount        = m_Desc.MipLevels;
        viewInfo.subresourceRange.baseArrayLayer    = 0;
        viewInfo.subresourceRange.layerCount        = 1;
        if (vkCreateImageView(m_pDevice->GetVkDevice(), &viewInfo, nullptr, &m_ImageView) != VK_SUCCESS)
        {
            LOG_DEBUG_ERROR("Vulkan: Failed to create ImageView\n");
        }
        else
        {
            LOG_DEBUG_INFO("Vulkan: Created ImageView '%p'\n", m_ImageView);
        }
    }


	void* VKNTexture::GetNativeHandle() const
    {
        return reinterpret_cast<void*>(m_VkImage);
    }
	
	
	void VKNTexture::SetName(const char* pName)
	{
		TTexture::SetName(pName);
		if (pName)
		{
			m_pDevice->SetVulkanObjectName(VK_OBJECT_TYPE_IMAGE, (uint64)m_VkImage, m_Name);
			m_Desc.pName = m_Name.c_str();
		}
	}
}
