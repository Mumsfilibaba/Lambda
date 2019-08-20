#pragma once
#include "Defines.h"
#include "Types.h"
#include <vector>
#include <vulkan/vulkan.h>

namespace Lambda
{
	//Forward declarations
	class ITexture2D;
	class VulkanTexture2D;


	//Describe vulkanswapchain
	struct VulkanSwapChainDesc
	{
		VkSurfaceKHR Surface = VK_NULL_HANDLE;
		VkPhysicalDevice Adapter = VK_NULL_HANDLE;
		VkSemaphore	SignalSemaphore = VK_NULL_HANDLE;
		VkExtent2D Extent;
		uint32 ImageCount = 0;
	};


	//Class containing vulkanswapchain
	class VulkanSwapChain
	{
	public:
		LAMBDA_NO_COPY(VulkanSwapChain);

		VulkanSwapChain(VkDevice device, const VulkanSwapChainDesc& desc);
		~VulkanSwapChain() = default;

		uint32 GetWidth() const;
		uint32 GetHeight() const;
		uint32 GetCurrentBackBufferIndex() const;
		VkFormat GetFormat() const;
		ITexture2D* GetCurrentBuffer() const;

		void ResizeBuffers(uint32 width, uint32 height);
		void AquireNextImage(VkDevice device, VkSemaphore signalSemaphore);
		void Present(VkDevice device, VkQueue presentQueue, VkSemaphore waitSemaphore);

		void Release(VkDevice device);
		void Destroy(VkDevice device);

	private:
		void Init(VkDevice device, const VulkanSwapChainDesc& desc);

	private:
		VkSwapchainKHR m_SwapChain;
		VkFormat m_Format;
		VkExtent2D m_Size;
		uint32 m_ImageCount;
		uint32 m_CurrentFrame;
		mutable uint32 m_CurrentBufferIndex;
		std::vector<VulkanTexture2D*> m_Buffers;
	};
}