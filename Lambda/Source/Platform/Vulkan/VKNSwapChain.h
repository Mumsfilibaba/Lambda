#pragma once
#include "Graphics/Core/SwapChainBase.h"
#include <vector>
#if defined(LAMBDA_PLAT_WINDOWS)
    #define VK_USE_PLATFORM_WIN32_KHR
#endif
#include "VKNDeviceContext.h"
#include "VKNUtilities.h"

namespace Lambda
{
	class ITexture;
    class VKNDevice;
	class VKNTexture;

	//------------
	//VKNSwapChain
	//------------

	class VKNSwapChain final : public SwapChainBase<VKNDevice>
	{
	public:
		LAMBDA_NO_COPY(VKNSwapChain);

		VKNSwapChain(VKNDevice* pDevice, const SwapChainDesc& desc);
		~VKNSwapChain();

		virtual void ResizeBuffers(uint32 width, uint32 height) override final;
		virtual bool SetFullscreenState(bool fullscreenState) override final;
		virtual bool GetFullscreenState() const override final;
		virtual ITexture* GetBuffer() override final;
        virtual ITexture* GetDepthBuffer() override final;
        virtual void Present() override final;
        virtual void* GetNativeHandle() const override final;
		
		void AquireNextImage();

        
        _forceinline uint32 GetBackBufferIndex() const
        {
            return m_CurrentBufferIndex;
        }
        
        
        _forceinline VkFormat GetVkFormat() const
        {
            return m_VkFormat.format;
        }
	private:
		void Init(const SwapChainDesc& desc);
        void InitSwapChain(VkExtent2D extent);
        void ReleaseResources();
	private:
		AutoRef<VKNDeviceContext> m_Context;
        VkSurfaceKHR        m_Surface;
		VkSwapchainKHR		m_VkSwapChain;
		VkSurfaceFormatKHR	m_VkFormat;
        VkPresentModeKHR	m_PresentationMode;
        AutoRef<VKNTexture> m_SampleBuffer;
        AutoRef<VKNTexture> m_DepthStencilBuffer;
		mutable uint32		m_CurrentBufferIndex;
        mutable uint32      m_SemaphoreIndex;
        std::vector<VkSemaphore> m_RenderSemaphores;
        std::vector<VkSemaphore> m_ImageSemaphores;
		mutable std::vector<AutoRef<VKNTexture>> m_Buffers;
	};
}
