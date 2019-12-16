#pragma once
#include "Graphics/Core/CSwapChainBase.h"
#include <vector>
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

	class VKNSwapChain final : public CSwapChainBase<VKNDevice>
	{
	public:
		LAMBDA_NO_COPY(VKNSwapChain);

		VKNSwapChain(VKNDevice* pVkDevice, const SSwapChainDesc& desc);
		~VKNSwapChain();

		virtual void ResizeBuffers(uint32 width, uint32 height) override final;
		virtual bool SetFullscreenState(bool fullscreenState) override final;
		virtual bool GetFullscreenState() const override final;
		virtual ITexture* GetBuffer() override final;
        virtual ITexture* GetDepthBuffer() override final;
        virtual void Present() override final;
        virtual void* GetNativeHandle() const override final;
		
		VkResult AquireNextImage();

        
        _forceinline uint32 GetBackBufferIndex() const
        {
            return m_CurrentBufferIndex;
        }
        
        
        _forceinline VkFormat GetVkFormat() const
        {
            return m_VkFormat.format;
        }
	private:
		void Init(const SSwapChainDesc& desc);
        void InitSwapChain(VkExtent2D extent);
        void ReleaseResources();
		void RecreateSwapChain();
	private:
		mutable std::vector<AutoRef<VKNTexture>> m_Buffers;
        std::vector<VkSemaphore> m_RenderSemaphores;
        std::vector<VkSemaphore> m_ImageSemaphores;
		VkSurfaceFormatKHR m_VkFormat;
		AutoRef<VKNDeviceContext> m_Context;
        AutoRef<VKNTexture> m_SampleBuffer;
        AutoRef<VKNTexture> m_DepthStencilBuffer;
        VkSurfaceKHR m_VkSurface;
		VkSwapchainKHR m_VkSwapChain;
        VkPresentModeKHR m_VkPresentMode;
		mutable uint32 m_CurrentBufferIndex;
        mutable uint32 m_SemaphoreIndex;
	};
}
