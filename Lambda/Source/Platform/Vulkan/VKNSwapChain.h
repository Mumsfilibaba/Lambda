#pragma once
#include "Defines.h"
#include "Types.h"
#include <vector>
#include "VKNUtilities.h"

namespace Lambda
{
	class ITexture;
	class VKNTexture;
    class VKNGraphicsDevice;

	//----------------
	//VKNSwapChainDesc
	//----------------

	struct VKNSwapChainDesc
	{
		VkSemaphore			SignalSemaphore	= VK_NULL_HANDLE;
        VkExtent2D			Extent          = { 0, 0 };
		uint32				ImageCount      = 0;
        VkSurfaceFormatKHR	Format;
        VkPresentModeKHR	PresentationMode;
	};

	//------------
	//VKNSwapChain
	//------------

	class VKNSwapChain
	{
	public:
		LAMBDA_NO_COPY(VKNSwapChain);

		VKNSwapChain(const VKNSwapChainDesc& desc);
		~VKNSwapChain() = default;

		void ResizeBuffers(VkSemaphore signalSemaphore, uint32 width, uint32 height);
		void AquireNextImage(VkSemaphore signalSemaphore);
		void Present(VkQueue presentQueue, VkSemaphore waitSemaphore);

		void Release(VkDevice device);
		void Destroy(VkDevice device);

		uint32		GetWidth() const;
		uint32		GetHeight() const;
        uint32		GetBufferCount() const;
        uint32		GetBackBufferIndex() const;
		VkFormat	GetFormat() const;
		ITexture*	GetCurrentBuffer() const;

	private:
		void Init(const VKNSwapChainDesc& desc);
        void InitSwapChain(VkSemaphore signalSemaphore, VkExtent2D extent);

	private:
		VkSwapchainKHR				m_SwapChain;
		VkSurfaceFormatKHR			m_Format;
		VkExtent2D					m_Extent;
        VkPresentModeKHR			m_PresentationMode;
		uint32						m_ImageCount;
		mutable uint32				m_CurrentBufferIndex;
		std::vector<VKNTexture*>	m_Buffers;
	};
}
