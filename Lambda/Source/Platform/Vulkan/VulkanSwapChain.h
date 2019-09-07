#pragma once
#include "Defines.h"
#include "Types.h"
#include <vector>
#include "VulkanUtilities.h"

namespace Lambda
{
	class ITexture;
	class VulkanTexture;


	struct VulkanSwapChainDesc
	{
		VkSurfaceKHR Surface        = VK_NULL_HANDLE;
		VkPhysicalDevice Adapter    = VK_NULL_HANDLE;
		VkSemaphore	SignalSemaphore = VK_NULL_HANDLE;
        VkExtent2D Extent           = { 0, 0 };
		uint32 ImageCount           = 0;
        VkSurfaceFormatKHR Format;
        VkPresentModeKHR PresentationMode;
	};


	class VulkanSwapChain
	{
	public:
		LAMBDA_NO_COPY(VulkanSwapChain);

		VulkanSwapChain(VkDevice device, const VulkanSwapChainDesc& desc);
		~VulkanSwapChain() = default;

		uint32 GetWidth() const;
		uint32 GetHeight() const;
        uint32 GetBufferCount() const;
        uint32 GetCurrentBackBufferIndex() const;
		VkFormat GetFormat() const;
		ITexture* GetCurrentBuffer() const;

		void ResizeBuffers(VkDevice device, VkSemaphore signalSemaphore, uint32 width, uint32 height);
		void AquireNextImage(VkDevice device, VkSemaphore signalSemaphore);
		void Present(VkQueue presentQueue, VkSemaphore waitSemaphore);

		void Release(VkDevice device);
		void Destroy(VkDevice device);

	private:
		void Init(VkDevice device, const VulkanSwapChainDesc& desc);
        void InitSwapChain(VkDevice device, VkSemaphore signalSemaphore, VkExtent2D extent);

	private:
        VkPhysicalDevice m_Adapter;
        VkSurfaceKHR m_Surface;
		VkSwapchainKHR m_SwapChain;
		VkSurfaceFormatKHR m_Format;
		VkExtent2D m_Extent;
        VkPresentModeKHR m_PresentationMode;
        QueueFamilyIndices m_FamilyIndices;
        SwapChainCapabilities m_Cap;
		uint32 m_ImageCount;
		mutable uint32 m_CurrentBufferIndex;
		std::vector<VulkanTexture*> m_Buffers;
	};
}
