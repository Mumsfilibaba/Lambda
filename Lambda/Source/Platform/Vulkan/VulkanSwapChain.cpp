#include "LambdaPch.h"
#include <algorithm>
#include "VulkanSwapChain.h"
#include "VulkanTexture.h"
#include "VulkanUtilities.h"
#include "VulkanGraphicsDevice.h"
#include "VulkanConversions.inl"

namespace Lambda
{
	VulkanSwapChain::VulkanSwapChain(const VulkanGraphicsDevice* pVkDevice, const VulkanSwapChainDesc& desc)
		: m_SwapChain(VK_NULL_HANDLE),
		m_Format(),
		m_Extent(),
		m_ImageCount(0),
		m_CurrentBufferIndex(0),
		m_Buffers()
	{
		assert(pVkDevice != nullptr);
		Init(pVkDevice, desc);
	}


	void VulkanSwapChain::Init(const VulkanGraphicsDevice* pVkDevice, const VulkanSwapChainDesc& desc)
	{
		assert(desc.Adapter != VK_NULL_HANDLE);
		assert(desc.Surface != VK_NULL_HANDLE);

        //Get the swapchain capabilities from the adapter
        m_Cap = QuerySwapChainSupport(desc.Adapter, desc.Surface);
        m_FamilyIndices = FindQueueFamilies(desc.Adapter, desc.Surface);
        
		//Choose a swapchain format
		m_Format = m_Cap.Formats[0];
		for (const auto& availableFormat : m_Cap.Formats)
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
		for (const auto& availablePresentMode : m_Cap.PresentModes)
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
        if (m_ImageCount < m_Cap.Capabilities.minImageCount)
        {
            m_ImageCount = m_Cap.Capabilities.minImageCount + 1;
        }
		if (m_Cap.Capabilities.maxImageCount > 0 && m_ImageCount > m_Cap.Capabilities.maxImageCount)
		{
			m_ImageCount = m_Cap.Capabilities.maxImageCount;
		}

		LOG_DEBUG_INFO("Vulkan: Number of images in SwapChain '%u'\n", m_ImageCount);
        
        m_Adapter = desc.Adapter;
        m_Surface = desc.Surface;

        InitSwapChain(pVkDevice, desc.SignalSemaphore, desc.Extent);
	}
    
    
    void VulkanSwapChain::InitSwapChain(const VulkanGraphicsDevice* pVkDevice, VkSemaphore signalSemaphore, VkExtent2D extent)
    {
        m_Cap = QuerySwapChainSupport(m_Adapter, m_Surface);
        
        //Choose swapchain extent (Size)
        VkExtent2D newExtent;
        if (m_Cap.Capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max() ||
            m_Cap.Capabilities.currentExtent.height != std::numeric_limits<uint32_t>::max())
        {
            newExtent = m_Cap.Capabilities.currentExtent;
        }
        else
        {
            newExtent.width = std::max(m_Cap.Capabilities.minImageExtent.width, std::min(m_Cap.Capabilities.maxImageExtent.width, extent.width));
            newExtent.height = std::max(m_Cap.Capabilities.minImageExtent.height, std::min(m_Cap.Capabilities.maxImageExtent.height, extent.height));
        }
        
        LOG_DEBUG_INFO("Vulkan: Chosen SwapChain size w: %u h: %u\n", newExtent.width, newExtent.height);
    
        //Setup swapchain
        VkSwapchainCreateInfoKHR info = {};
        info.sType                = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        info.pNext                = nullptr;
        info.surface              = m_Surface;
        info.minImageCount        = m_ImageCount;
        info.imageFormat          = m_Format.format;
        info.imageColorSpace      = m_Format.colorSpace;
        info.imageExtent          = newExtent;
        info.imageArrayLayers     = 1;
        info.imageUsage           = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT; //Use as color attachment and clear
        info.preTransform         = m_Cap.Capabilities.currentTransform;
        info.compositeAlpha       = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        info.presentMode          = m_PresentationMode;
        info.clipped              = VK_TRUE;
        info.oldSwapchain         = VK_NULL_HANDLE;
        
        //Is the swapchain going to be used by more than one queue
        if (m_FamilyIndices.GraphicsFamily != m_FamilyIndices.PresentFamily)
        {
            //If the graphics- and presentqueue is not the same then we have to specify the different queues
            uint32 queueFamilyIndices[] = { uint32(m_FamilyIndices.GraphicsFamily), uint32(m_FamilyIndices.PresentFamily) };
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

        VkDevice device = reinterpret_cast<VkDevice>(pVkDevice->GetNativeHandle());
        if (vkCreateSwapchainKHR(device, &info, nullptr, &m_SwapChain) != VK_SUCCESS)
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
        vkGetSwapchainImagesKHR(device, m_SwapChain, &imageCount, nullptr);
        m_ImageCount = imageCount;
        
        
        //Init textures
        std::vector<VkImage> textures(imageCount);
        vkGetSwapchainImagesKHR(device, m_SwapChain, &imageCount, textures.data());
        for (uint32 i = 0; i < imageCount; i++)
        {
            TextureDesc desc = {};
            desc.Type = TEXTURE_TYPE_2D;
            desc.pResolveResource = nullptr;
            desc.Flags = TEXTURE_FLAGS_RENDER_TARGET;
            desc.Format = ConvertVkFormat(m_Format.format);
            desc.Width = m_Extent.width;
            desc.Height = m_Extent.height;
            desc.Depth = 1;
            desc.ArraySize = 1;
            desc.MipLevels = 1;
            desc.SampleCount = 1;
            desc.Usage = RESOURCE_USAGE_DEFAULT;
            m_Buffers.push_back(DBG_NEW VulkanTexture(pVkDevice, textures[i], desc));
        }
        
        //Aquire the first swapchain image
        AquireNextImage(device, signalSemaphore);
        LOG_DEBUG_INFO("Vulkan: Created ImageViews\n");
    }


	void VulkanSwapChain::AquireNextImage(VkDevice device, VkSemaphore signalSemaphore)
	{
		//LOG_DEBUG_INFO("Vulkan: vkAcquireNextImageKHR with semaphore %x\n", signalSemaphore);
		vkAcquireNextImageKHR(device, m_SwapChain, 0xffffffffffffffff, signalSemaphore, VK_NULL_HANDLE, &m_CurrentBufferIndex);
	}


	VkFormat VulkanSwapChain::GetFormat() const
	{
		return m_Format.format;
	}


	uint32 VulkanSwapChain::GetWidth() const
	{
		return m_Extent.width;
	}


	uint32 VulkanSwapChain::GetHeight() const
	{
		return m_Extent.height;
	}


	uint32 VulkanSwapChain::GetBufferCount() const
	{
		return uint32(m_Buffers.size());
	}


	uint32 VulkanSwapChain::GetBackBufferIndex() const
	{
		return m_CurrentBufferIndex;
	}


	ITexture* VulkanSwapChain::GetCurrentBuffer() const
	{
		return m_Buffers[m_CurrentBufferIndex];
	}


	void VulkanSwapChain::ResizeBuffers(const VulkanGraphicsDevice* pVkDevice, VkSemaphore signalSemaphore, uint32 width, uint32 height)
    {
        //Relase the old resources
        VkDevice device = reinterpret_cast<VkDevice>(pVkDevice->GetNativeHandle());
        Release(device);
        
        //Create swapchain again
        VkExtent2D extent = { width, height };
        InitSwapChain(pVkDevice, signalSemaphore, extent);
	}


	void VulkanSwapChain::Present(VkQueue presentQueue, VkSemaphore waitSemaphore)
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


	void VulkanSwapChain::Release(VkDevice device)
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


	void VulkanSwapChain::Destroy(VkDevice device)
	{
		assert(device != VK_NULL_HANDLE);

		Release(device);
		delete this;
	}
}
