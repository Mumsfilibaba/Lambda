#include "LambdaPch.h"
#include "VKNSwapChain.h"
#include "VKNTexture.h"
#include "VKNUtilities.h"
#include "VKNDevice.h"
#include "VKNConversions.inl"

namespace Lambda
{
	//------------
	//VKNSwapChain
	//------------

	VKNSwapChain::VKNSwapChain(const VKNSwapChainDesc& desc)
		: m_SwapChain(VK_NULL_HANDLE),
		m_Format(),
		m_Extent(),
		m_PresentationMode(),
		m_ImageCount(0),
		m_CurrentBufferIndex(0),
		m_Buffers()
	{
		Init(desc);
	}


	void VKNSwapChain::Init(const VKNSwapChainDesc& desc)
	{
        //Get the swapchain capabilities from the adapter
		VKNDevice& device					= VKNDevice::Get();
        SwapChainCapabilities cap			= QuerySwapChainSupport(device.GetPhysicalDevice(), device.GetSurface());
        QueueFamilyIndices familyIndices	= device.GetQueueFamilyIndices();
        
		//Choose a swapchain format
		m_Format = cap.Formats[0];
		for (const auto& availableFormat : cap.Formats)
		{
			if (availableFormat.format == desc.Format.format && availableFormat.colorSpace == desc.Format.colorSpace)
			{
				m_Format = availableFormat;
				break;
			}
		}
    
		LOG_DEBUG_INFO("Vulkan: Chosen SwapChain format '%s'\n", VkFormatToString(m_Format.format));

		//Choose a presentationmode
		m_PresentationMode = VK_PRESENT_MODE_FIFO_KHR;
		for (const auto& availablePresentMode : cap.PresentModes)
		{
            if (availablePresentMode == desc.PresentationMode)
            {
                m_PresentationMode = availablePresentMode;
                break;
            }
			else if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
			{
				m_PresentationMode = availablePresentMode;
				break;
			}
			else if (availablePresentMode == VK_PRESENT_MODE_IMMEDIATE_KHR)
			{
				m_PresentationMode = availablePresentMode;
			}
		}

		LOG_DEBUG_INFO("Vulkan: Chosen SwapChain PresentationMode '%s'\n", VkPresentatModeToString(m_PresentationMode));

		//Setup swapchain images
        m_ImageCount = desc.ImageCount;
        if (m_ImageCount < cap.Capabilities.minImageCount)
        {
            m_ImageCount = cap.Capabilities.minImageCount + 1;
        }
		if (cap.Capabilities.maxImageCount > 0 && m_ImageCount > cap.Capabilities.maxImageCount)
		{
			m_ImageCount = cap.Capabilities.maxImageCount;
		}

		LOG_DEBUG_INFO("Vulkan: Number of images in SwapChain '%u'\n", m_ImageCount);
        
        InitSwapChain(desc.Extent);
	}
    
    
    void VKNSwapChain::InitSwapChain(VkExtent2D extent)
    {
		VKNDevice& device			= VKNDevice::Get();
		SwapChainCapabilities cap	= QuerySwapChainSupport(device.GetPhysicalDevice(), device.GetSurface());
        
        //Choose swapchain extent (Size)
        VkExtent2D newExtent;
        if (cap.Capabilities.currentExtent.width	!= std::numeric_limits<uint32_t>::max() ||
			cap.Capabilities.currentExtent.height	!= std::numeric_limits<uint32_t>::max())
        {
            newExtent = cap.Capabilities.currentExtent;
        }
        else
        {
            newExtent.width = std::max(cap.Capabilities.minImageExtent.width, std::min(cap.Capabilities.maxImageExtent.width, extent.width));
            newExtent.height = std::max(cap.Capabilities.minImageExtent.height, std::min(cap.Capabilities.maxImageExtent.height, extent.height));
        }
        
        LOG_DEBUG_INFO("Vulkan: Chosen SwapChain size w: %u h: %u\n", newExtent.width, newExtent.height);
    
        //Setup swapchain
        VkSwapchainCreateInfoKHR info = {};
        info.sType                = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        info.pNext                = nullptr;
        info.surface              = device.GetSurface();
        info.minImageCount        = m_ImageCount;
        info.imageFormat          = m_Format.format;
        info.imageColorSpace      = m_Format.colorSpace;
        info.imageExtent          = newExtent;
        info.imageArrayLayers     = 1;
        info.imageUsage           = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT; //Use as color attachment and clear
        info.preTransform         = cap.Capabilities.currentTransform;
        info.compositeAlpha       = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        info.presentMode          = m_PresentationMode;
        info.clipped              = VK_TRUE;
        info.oldSwapchain         = VK_NULL_HANDLE;
        
        //Is the swapchain going to be used by more than one queue
		QueueFamilyIndices familyIndices = device.GetQueueFamilyIndices();
        if (familyIndices.GraphicsFamily != familyIndices.PresentFamily)
        {
            //If the graphics- and presentqueue is not the same then we have to specify the different queues
            uint32 queueFamilyIndices[] = { uint32(familyIndices.GraphicsFamily), uint32(familyIndices.PresentFamily) };
            info.imageSharingMode        = VK_SHARING_MODE_CONCURRENT;
            info.queueFamilyIndexCount   = 2;
            info.pQueueFamilyIndices     = queueFamilyIndices;
        }
        else
        {
            //Graphics- and presentqueue are the same
            info.imageSharingMode        = VK_SHARING_MODE_EXCLUSIVE;
            info.queueFamilyIndexCount   = 0;
            info.pQueueFamilyIndices     = nullptr;
        }

        if (vkCreateSwapchainKHR(device.GetDevice(), &info, nullptr, &m_SwapChain) != VK_SUCCESS)
        {
            LOG_DEBUG_ERROR("Vulkan: Failed to create SwapChain\n");
            
            m_SwapChain = VK_NULL_HANDLE;
            return;
        }
        else
        {
            LOG_DEBUG_INFO("Vulkan: Created SwapChain\n");
            m_Extent = newExtent;
        }
        
        //Get SwapChain images
        uint32 imageCount = 0;
        vkGetSwapchainImagesKHR(device.GetDevice(), m_SwapChain, &imageCount, nullptr);
        m_ImageCount = imageCount;
        
        //Init textures
        std::vector<VkImage> textures(imageCount);
        vkGetSwapchainImagesKHR(device.GetDevice(), m_SwapChain, &imageCount, textures.data());
        for (uint32 i = 0; i < imageCount; i++)
        {
            TextureDesc desc = {};
            desc.Type				= TEXTURE_TYPE_2D;
            desc.pResolveResource	= nullptr;
            desc.Flags				= TEXTURE_FLAGS_RENDER_TARGET;
            desc.Format				= ConvertVkFormat(m_Format.format);
            desc.Width				= m_Extent.width;
            desc.Height				= m_Extent.height;
            desc.Depth				= 1;
            desc.ArraySize			= 1;
            desc.MipLevels			= 1;
            desc.SampleCount		= 1;
            desc.Usage				= RESOURCE_USAGE_DEFAULT;

            m_Buffers.push_back(DBG_NEW VKNTexture(textures[i], desc));
        }
        
        //Aquire the first swapchain image
        //AquireNextImage(signalSemaphore);

        LOG_DEBUG_INFO("Vulkan: Created ImageViews\n");
    }


	void VKNSwapChain::AquireNextImage(VkSemaphore signalSemaphore)
	{
		//LOG_DEBUG_INFO("Vulkan: vkAcquireNextImageKHR with semaphore %x\n", signalSemaphore);
		
		VKNDevice& device = VKNDevice::Get();
		vkAcquireNextImageKHR(device.GetDevice(), m_SwapChain, 0xffffffffffffffff, signalSemaphore, VK_NULL_HANDLE, &m_CurrentBufferIndex);
	}


	VkFormat VKNSwapChain::GetFormat() const
	{
		return m_Format.format;
	}


	uint32 VKNSwapChain::GetWidth() const
	{
		return m_Extent.width;
	}


	uint32 VKNSwapChain::GetHeight() const
	{
		return m_Extent.height;
	}


	uint32 VKNSwapChain::GetBufferCount() const
	{
		return uint32(m_Buffers.size());
	}


	uint32 VKNSwapChain::GetBackBufferIndex() const
	{
		return m_CurrentBufferIndex;
	}


	ITexture* VKNSwapChain::GetCurrentBuffer() const
	{
		return m_Buffers[m_CurrentBufferIndex];
	}


	void VKNSwapChain::ResizeBuffers(uint32 width, uint32 height)
    {
        //Relase the old resources
		VKNDevice& device = VKNDevice::Get();
		Release(device.GetDevice());
        
        //Create swapchain again
        VkExtent2D extent = { width, height };
        InitSwapChain(extent);
	}


	void VKNSwapChain::Present(VkQueue presentQueue, VkSemaphore waitSemaphore)
	{
		VkSemaphore waitSemaphores[] = { waitSemaphore };
		VkPresentInfoKHR info = {};
		info.sType				= VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		info.pNext				= nullptr;
		info.waitSemaphoreCount = 1;
		info.pWaitSemaphores	= waitSemaphores;
		info.swapchainCount		= 1;
		info.pSwapchains		= &m_SwapChain;
		info.pImageIndices		= &m_CurrentBufferIndex;
		info.pResults			= nullptr;

		vkQueuePresentKHR(presentQueue, &info);
	}


	void VKNSwapChain::Release(VkDevice device)
	{
		//Release backbuffers
		for (size_t i = 0; i < m_Buffers.size(); i++)
		{
			if (m_Buffers[i])
			{
				m_Buffers[i]->Destroy(device);
				m_Buffers[i] = nullptr;
			}
		}

		m_Buffers.clear();

		if (m_SwapChain != VK_NULL_HANDLE)
		{
			vkDestroySwapchainKHR(device, m_SwapChain, nullptr);
			m_SwapChain = VK_NULL_HANDLE;
		}
	}


	void VKNSwapChain::Destroy(VkDevice device)
	{
		LAMBDA_ASSERT(device != VK_NULL_HANDLE);

		Release(device);
		delete this;
	}
}
