#pragma once
#include "IObject.h"
#include <vector>
#include "VKNUtilities.h"

namespace Lambda
{
	class ITexture;
    class VKNDevice;
	class VKNTexture;

	//----------------
	//VKNSwapChainDesc
	//----------------

	struct VKNSwapChainDesc
	{
        VkExtent2D			Extent          = { 0, 0 };
		uint32				ImageCount      = 0;
        VkSurfaceFormatKHR	Format;
        bool                VerticalSync    = true;
	};

	//------------
	//VKNSwapChain
	//------------

	class VKNSwapChain
	{
	public:
		LAMBDA_NO_COPY(VKNSwapChain);

		VKNSwapChain(VKNDevice* pDevice, const VKNSwapChainDesc& desc);
		~VKNSwapChain() = default;

		void ResizeBuffers(uint32 width, uint32 height);
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
        void InitSwapChain(VkExtent2D extent);
	private:
		VKNDevice*					m_pDevice;
		VkSwapchainKHR				m_SwapChain;
		VkSurfaceFormatKHR			m_Format;
		VkExtent2D					m_Extent;
        VkPresentModeKHR			m_PresentationMode;
		uint32						m_ImageCount;
		mutable uint32				m_CurrentBufferIndex;
		mutable std::vector<AutoRef<VKNTexture>> m_Buffers;
	};
}
