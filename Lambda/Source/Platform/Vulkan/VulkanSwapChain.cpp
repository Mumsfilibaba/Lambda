#include "LambdaPch.h"
#include <algorithm>
#include "VulkanSwapChain.h"
#include "VulkanTexture2D.h"
#include "VulkanHelperStructs.h"
#include "VulkanHelpers.inl"

namespace Lambda
{
	VulkanSwapChain::VulkanSwapChain(VkDevice device, const VulkanSwapChainDesc& desc)
		: m_SwapChain(VK_NULL_HANDLE),
		m_Format(),
		m_Size(),
		m_ImageCount(0),
		m_CurrentFrame(0),
		m_CurrentBufferIndex(0),
		m_Buffers()
	{
		assert(device != VK_NULL_HANDLE);

		Init(device, desc);
	}


	void VulkanSwapChain::Init(VkDevice device, const VulkanSwapChainDesc& desc)
	{
		assert(desc.Adapter != VK_NULL_HANDLE);
		assert(desc.Surface != VK_NULL_HANDLE);

		//Get the swapchain capabilities from the adapter
		SwapChainCapabilities cap = QuerySwapChainSupport(desc.Adapter, desc.Surface);

		//Choose a swapchain format
		VkSurfaceFormatKHR format = cap.Formats[0];
		for (const auto& availableFormat : cap.Formats)
		{
			if (availableFormat.format == VK_FORMAT_B8G8R8A8_UNORM && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
			{
				format = availableFormat;
				break;
			}
		}

		LOG_DEBUG_INFO("Vulkan: Chosen SwapChain format '%s'\n", VkFormatToString(format.format));

		//Choose a presentationmode
		VkPresentModeKHR presentationMode = VK_PRESENT_MODE_FIFO_KHR;
		for (const auto& availablePresentMode : cap.PresentModes)
		{
			if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
			{
				presentationMode = availablePresentMode;
				break;
			}
			else if (availablePresentMode == VK_PRESENT_MODE_IMMEDIATE_KHR)
			{
				presentationMode = availablePresentMode;
			}
		}

		LOG_DEBUG_INFO("Vulkan: Chosen SwapChain PresentationMode '%s'\n", VkPresentatModeToString(presentationMode));

		//Choose swapchain extent (Size)
		VkExtent2D extent;
		if (cap.Capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
		{
			extent = cap.Capabilities.currentExtent;
		}
		else
		{
			VkExtent2D actualExtent = { desc.Extent.width, desc.Extent.height };

			actualExtent.width = std::max(cap.Capabilities.minImageExtent.width, std::min(cap.Capabilities.maxImageExtent.width, actualExtent.width));
			actualExtent.height = std::max(cap.Capabilities.minImageExtent.height, std::min(cap.Capabilities.maxImageExtent.height, actualExtent.height));

			extent = actualExtent;
		}

		LOG_DEBUG_INFO("Vulkan: Chosen SwapChain Extent w: %u h: %u\n", extent.width, extent.height);

		//Setup swapchain images
		uint32 imageCount = cap.Capabilities.minImageCount + 1;
		if (cap.Capabilities.maxImageCount > 0 && imageCount > cap.Capabilities.maxImageCount)
		{
			imageCount = cap.Capabilities.maxImageCount;
		}

		LOG_DEBUG_INFO("Vulkan: Number of images in SwapChain '%u'\n", imageCount);


		//Setup swapchain
		VkSwapchainCreateInfoKHR info = {};
		info.sType				= VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		info.pNext				= nullptr;
		info.surface			= desc.Surface;
		info.minImageCount		= imageCount;
		info.imageFormat		= format.format;
		info.imageColorSpace	= format.colorSpace;
		info.imageExtent		= extent;
		info.imageArrayLayers	= 1;
		info.imageUsage			= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT; //Use as color attachment and clear
		info.preTransform		= cap.Capabilities.currentTransform;
		info.compositeAlpha		= VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		info.presentMode		= presentationMode;
		info.clipped			= VK_TRUE;
		info.oldSwapchain		= VK_NULL_HANDLE;

		//Is the swapchain going to be used by more than one queue
		QueueFamilyIndices indices = FindQueueFamilies(desc.Adapter, desc.Surface);
		if (indices.GraphicsFamily != indices.PresentFamily)
		{
			//If the graphics- and presentqueue is not the same then we have to specify the different queues
			uint32 queueFamilyIndices[] = { uint32(indices.GraphicsFamily), uint32(indices.PresentFamily) };
			info.imageSharingMode		= VK_SHARING_MODE_CONCURRENT;
			info.queueFamilyIndexCount	= 2;
			info.pQueueFamilyIndices	= queueFamilyIndices;
		}
		else
		{
			//Graphics- and presentqueue are the same
			info.imageSharingMode		= VK_SHARING_MODE_EXCLUSIVE;
			info.queueFamilyIndexCount	= 0;
			info.pQueueFamilyIndices	= nullptr;
		}

		//Create swapchain
		if (vkCreateSwapchainKHR(device, &info, nullptr, &m_SwapChain) != VK_SUCCESS)
		{
			LOG_DEBUG_ERROR("Vulkan: Failed to create SwapChain\n");

			m_SwapChain = VK_NULL_HANDLE;
			return;
		}
		else
		{
			LOG_DEBUG_INFO("Vulkan: Created SwapChain\n");

			//Save the extent and formats
			m_Format	= format.format;
			m_Size		= extent;
		}

		//Get SwapChain images
		imageCount = 0;
		vkGetSwapchainImagesKHR(device, m_SwapChain, &imageCount, nullptr);

		//Set imagecount
		m_ImageCount = imageCount;

		std::vector<VkImage> textures(imageCount);
		vkGetSwapchainImagesKHR(device, m_SwapChain, &imageCount, textures.data());

		//Init textures
		for (uint32 i = 0; i < imageCount; i++)
		{
			VulkanTextureDesc desc = {};
			desc.ArraySize = 1;
			desc.AspectFlags = VK_IMAGE_ASPECT_COLOR_BIT;
			desc.ClearValue.color.float32[0] = 0.0f;
			desc.ClearValue.color.float32[1] = 0.0f;
			desc.ClearValue.color.float32[2] = 0.0f;
			desc.ClearValue.color.float32[3] = 0.0f;
			desc.Depth = false;
			desc.Extent = m_Size;
			desc.Format = m_Format;
			desc.Image = textures[i];
			desc.MipLevels = 1;
			desc.Samples = VK_SAMPLE_COUNT_1_BIT;
			desc.UsageFlags = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

			//Create texture
			m_Buffers.push_back(DBG_NEW VulkanTexture2D(device, desc));
		}

		//Aquire the first swapchain image
		AquireNextImage(device, desc.SignalSemaphore);

		LOG_DEBUG_INFO("Vulkan: Created ImageViews\n");
	}


	void VulkanSwapChain::AquireNextImage(VkDevice device, VkSemaphore signalSemaphore)
	{
		//LOG_DEBUG_INFO("Vulkan: vkAcquireNextImageKHR with semaphore %x\n", signalSemaphore);
		vkAcquireNextImageKHR(device, m_SwapChain, 0xffffffffffffffff, signalSemaphore, VK_NULL_HANDLE, &m_CurrentBufferIndex);
	}


	VkFormat VulkanSwapChain::GetFormat() const
	{
		return m_Format;
	}


	uint32 VulkanSwapChain::GetWidth() const
	{
		return m_Size.width;
	}


	uint32 VulkanSwapChain::GetHeight() const
	{
		return m_Size.height;
	}


	uint32 VulkanSwapChain::GetCurrentBackBufferIndex() const
	{
		return m_CurrentBufferIndex;
	}


	ITexture2D* VulkanSwapChain::GetCurrentBuffer() const
	{
		return m_Buffers[m_CurrentBufferIndex];
	}


	void VulkanSwapChain::ResizeBuffers(uint32 width, uint32 height)
	{
	}


	void VulkanSwapChain::Present(VkDevice device, VkQueue presentQueue, VkSemaphore waitSemaphore)
	{
		VkSemaphore waitSemaphores[] = { waitSemaphore };

		//Setup presentinfo
		VkPresentInfoKHR info = {};
		info.sType				= VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		info.pNext				= nullptr;
		info.waitSemaphoreCount = 1;
		info.pWaitSemaphores	= waitSemaphores;
		info.swapchainCount		= 1;
		info.pSwapchains		= &m_SwapChain;
		info.pImageIndices		= &m_CurrentBufferIndex;
		info.pResults			= nullptr;

		//Present
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

		//Clear backbuffer array
		m_Buffers.clear();

		//Release swapchain
		if (m_SwapChain != VK_NULL_HANDLE)
		{
			vkDestroySwapchainKHR(device, m_SwapChain, nullptr);
			m_SwapChain = VK_NULL_HANDLE;
		}
	}


	void VulkanSwapChain::Destroy(VkDevice device)
	{
		//Release resources
		Release(device);

		//Delete
		delete this;
	}
}