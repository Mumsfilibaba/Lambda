#include "LambdaPch.h"
#include "Types.h"
#include "VKNSwapChain.h"
#include "VKNTexture.h"
#include "VKNUtilities.h"
#include "VKNDevice.h"
#include "VKNDeviceContext.h"
#include "VKNConversions.inl"
#if defined(LAMBDA_PLAT_MACOS)
    #include <GLFW/glfw3.h>
#endif

namespace Lambda
{
	//------------
	//VKNSwapChain
	//------------

	VKNSwapChain::VKNSwapChain(VKNDevice* pDevice, const SwapChainDesc& desc)
		: SwapChainBase<VKNDevice>(pDevice, desc),
		m_Context(nullptr),
		m_VkSwapChain(VK_NULL_HANDLE),
        m_VkFormat(),
        m_PresentationMode(),
        m_SampleBuffer(nullptr),
        m_DepthStencilBuffer(nullptr),
		m_CurrentBufferIndex(0),
		m_SemaphoreIndex(0),
		m_Buffers(),
		m_FullscreenState(false)
	{
		//Add ref to context
		m_Context = m_pDevice->GetVKNImmediateContext();

		//Init
		this->AddRef();
		Init(desc);
	}


    VKNSwapChain::~VKNSwapChain()
    {
        //Release resources
        ReleaseResources();
        
        //Destroy semaphores and fences
        for (uint32 i = 0; i < m_Desc.BufferCount; i++)
        {
			if (m_ImageFences[i] != VK_NULL_HANDLE)
				m_pDevice->SafeReleaseVulkanResource<VkFence>(m_ImageFences[i]);
			if (m_ImageSemaphores[i] != VK_NULL_HANDLE)
				m_pDevice->SafeReleaseVulkanResource<VkSemaphore>(m_ImageSemaphores[i]);
            if (m_RenderSemaphores[i] != VK_NULL_HANDLE)
				m_pDevice->SafeReleaseVulkanResource<VkSemaphore>(m_RenderSemaphores[i]);
        }
        
        //Destroy surface
        if (m_Surface != VK_NULL_HANDLE)
        {
            vkDestroySurfaceKHR(m_pDevice->GetVkInstance(), m_Surface, nullptr);
            m_Surface = VK_NULL_HANDLE;
        }
    }


	void VKNSwapChain::Init(const SwapChainDesc& desc)
	{
        //Create window surface
#if defined(LAMBDA_PLAT_MACOS)
        //Create surface for macOS
        GLFWwindow* pGLFWWindow = reinterpret_cast<GLFWwindow*>(desc.pWindowHandle);
        if (glfwCreateWindowSurface(m_pDevice->GetVkInstance(), pGLFWWindow, nullptr, &m_Surface) != VK_SUCCESS)
            m_Surface = VK_NULL_HANDLE;
#elif defined(LAMBDA_PLAT_WINDOWS)	
		//Create a surface for windows
        VkWin32SurfaceCreateInfoKHR info = {};
        info.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
        info.pNext = nullptr;
        info.flags = 0;
        info.hwnd = reinterpret_cast<HWND>(desc.pWindowHandle);
        info.hinstance = GetModuleHandle(nullptr);
		if (vkCreateWin32SurfaceKHR(m_pDevice->GetVkInstance(), &info, nullptr, &m_Surface) != VK_SUCCESS)
		{
            m_Surface = VK_NULL_HANDLE;
		}
#endif

		//Did we successfully create a surface?
        if (m_Surface == VK_NULL_HANDLE)
        {
            LOG_DEBUG_ERROR("Vulkan: Failed to create surface for SwapChain\n");
            return;
        }
        else
        {
            LOG_DEBUG_INFO("Vulkan: Created Surface\n");
        }

        //Check for presentationsupport
        VkBool32 presentSupport = false;
        QueueFamilyIndices familyIndices = m_pDevice->GetQueueFamilyIndices();
        vkGetPhysicalDeviceSurfaceSupportKHR(m_pDevice->GetVkPhysicalDevice(), familyIndices.PresentFamily, m_Surface, &presentSupport);
        if (!presentSupport)
        {
            LOG_DEBUG_ERROR("Vulkan: Current queuefamily does not support presentation\n");
            return;
        }
          
        //Setup semaphore structure
        VkSemaphoreCreateInfo semaphoreInfo = {};
        semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
        semaphoreInfo.pNext = nullptr;
        semaphoreInfo.flags = 0;
        
		//Setup fences
		VkFenceCreateInfo fenceInfo = {};
		fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceInfo.pNext = nullptr;
		fenceInfo.flags = 0;

        //Create semaphores and fences
		m_ImageFences.resize(desc.BufferCount);
		m_SubmittedImageFences.resize(desc.BufferCount);
		m_ImageSemaphores.resize(desc.BufferCount);
        m_RenderSemaphores.resize(desc.BufferCount);
        for (uint32 i = 0; i < desc.BufferCount; i++)
        {
            if (vkCreateSemaphore(m_pDevice->GetVkDevice(), &semaphoreInfo, nullptr, &m_ImageSemaphores[i]) != VK_SUCCESS ||
                vkCreateSemaphore(m_pDevice->GetVkDevice(), &semaphoreInfo, nullptr, &m_RenderSemaphores[i]) != VK_SUCCESS)
            {
                LOG_DEBUG_ERROR("Vulkan: Failed to create Semaphore\n");
                return;
            }
            else
            {
                m_pDevice->SetVulkanObjectName(VK_OBJECT_TYPE_SEMAPHORE, (uint64)m_ImageSemaphores[i],  m_Name + " ImageSemaphore[" +  std::to_string(i) + "]");
				LOG_DEBUG_INFO("Vulkan: Created Semaphore %p\n", m_ImageSemaphores[i]);

                m_pDevice->SetVulkanObjectName(VK_OBJECT_TYPE_SEMAPHORE, (uint64)m_RenderSemaphores[i], m_Name + "RenderSemaphore[" + std::to_string(i) + "]");
				LOG_DEBUG_INFO("Vulkan: Created Semaphore %p\n", m_RenderSemaphores[i]);
            }

			//Create fence
			if (vkCreateFence(m_pDevice->GetVkDevice(), &fenceInfo, nullptr, &m_ImageFences[i]) != VK_SUCCESS)
			{
				LOG_DEBUG_ERROR("Vulkan: Failed to create fence\n");
				return;
			}
			else
			{
				//Set fence name
				m_pDevice->SetVulkanObjectName(VK_OBJECT_TYPE_FENCE, (uint64)m_ImageFences[i], m_Name + "Fence[" + std::to_string(i) + "]");
				m_SubmittedImageFences[i] = false;

				LOG_DEBUG_INFO("Vulkan: Created fence '%p'\n", m_ImageFences[i]);
			}
        }


        //Get the swapchain capabilities from the adapter
        SwapChainCapabilities cap = QuerySwapChainSupport(m_pDevice->GetVkPhysicalDevice(), m_Surface);
        
		//Find the swapchain format we want
        VkFormat lookingFor = ConvertFormat(desc.BufferFormat);
		m_VkFormat = { VK_FORMAT_UNDEFINED, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };
		for (const auto& availableFormat : cap.Formats)
		{
			if (availableFormat.format == lookingFor && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
			{
                m_VkFormat	= availableFormat;
				break;
			}
		}
    
        //Did we find it?
        if (m_VkFormat.format != VK_FORMAT_UNDEFINED)
        {
            LOG_DEBUG_INFO("Vulkan: Chosen SwapChain format '%s'\n", VkFormatToString(m_VkFormat.format));
        }
        else
        {
            LOG_DEBUG_ERROR("Vulkan: Format %s is not supported on. Following formats is supported for Creating a SwapChain\n", VkFormatToString(lookingFor));
            for (const auto& availableFormat : cap.Formats)
                LOG_DEBUG_ERROR("    %s\n", VkFormatToString(availableFormat.format));
            
            return;
        }

		//Choose a presentationmode
		m_PresentationMode = VK_PRESENT_MODE_FIFO_KHR;
        if (!desc.VerticalSync)
        {
			//Search for the mailbox mode
			for (const auto& availablePresentMode : cap.PresentModes)
			{
				if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
				{
					m_PresentationMode = availablePresentMode;
					break;
				}
			}

			//If mailbox is not available we choose immediete
			if (m_PresentationMode == VK_PRESENT_MODE_FIFO_KHR)
			{
				for (const auto& availablePresentMode : cap.PresentModes)
				{
					if (availablePresentMode == VK_PRESENT_MODE_IMMEDIATE_KHR)
					{
						m_PresentationMode = availablePresentMode;
						break;
					}
				}
			}
        }
       
		LOG_DEBUG_INFO("Vulkan: Chosen SwapChain PresentationMode '%s'\n", VkPresentatModeToString(m_PresentationMode));

		//Setup swapchain images
        if (desc.BufferCount >= cap.Capabilities.minImageCount && desc.BufferCount <= cap.Capabilities.maxImageCount)
        {
			//Set desc
			m_Desc = desc;

			//Continue to init swapchain
            LOG_DEBUG_INFO("Vulkan: Number of buffers in SwapChain '%u'\n", desc.BufferCount);
            this->InitSwapChain({ desc.BufferWidth, desc.BufferHeight });
        }
        else
        {
            LOG_DEBUG_ERROR("Vulkan: Number of buffers(=%u) is not supported. MinBuffers=%u MaxBuffers=%u\n", desc.BufferCount, cap.Capabilities.minImageCount, cap.Capabilities.maxImageCount);
        }
	}
    
    
    void VKNSwapChain::InitSwapChain(VkExtent2D extent)
    {
		SwapChainCapabilities cap = QuerySwapChainSupport(m_pDevice->GetVkPhysicalDevice(), m_Surface);
        
        //Choose swapchain extent (Size)
        VkExtent2D newExtent;
        if (cap.Capabilities.currentExtent.width  != std::numeric_limits<uint32_t>::max() ||
			cap.Capabilities.currentExtent.height != std::numeric_limits<uint32_t>::max() ||
			extent.width == 0 || extent.height == 0)
        {
            newExtent = cap.Capabilities.currentExtent;
        }
        else
        {
            newExtent.width	 = std::max(cap.Capabilities.minImageExtent.width, std::min(cap.Capabilities.maxImageExtent.width,	extent.width));
            newExtent.height = std::max(cap.Capabilities.minImageExtent.height, std::min(cap.Capabilities.maxImageExtent.height, extent.height));
        }
		newExtent.width		= std::max(newExtent.width,	 1u);
		newExtent.height	= std::max(newExtent.height, 1u);
		m_Desc.BufferWidth	= newExtent.width;
		m_Desc.BufferHeight	= newExtent.height;

        LOG_DEBUG_INFO("Vulkan: Chosen SwapChain size w: %u h: %u\n", newExtent.width, newExtent.height);
    
		VkSurfaceFullScreenExclusiveWin32InfoEXT fullscreenWIN32Info = {};
		fullscreenWIN32Info.sType = VK_STRUCTURE_TYPE_SURFACE_FULL_SCREEN_EXCLUSIVE_WIN32_INFO_EXT;
		fullscreenWIN32Info.pNext = nullptr;
		fullscreenWIN32Info.hmonitor = MonitorFromWindow(reinterpret_cast<HWND>(m_Desc.pWindowHandle), 0);

		VkSurfaceFullScreenExclusiveInfoEXT fullscreenInfo = {};
		fullscreenInfo.sType = VK_STRUCTURE_TYPE_SURFACE_FULL_SCREEN_EXCLUSIVE_INFO_EXT;
		fullscreenInfo.pNext = &fullscreenWIN32Info;
		fullscreenInfo.fullScreenExclusive = VK_FULL_SCREEN_EXCLUSIVE_APPLICATION_CONTROLLED_EXT;

        //Setup swapchain
        VkSwapchainCreateInfoKHR info = {};
        info.sType            = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        info.pNext            = &fullscreenInfo;
        info.surface          = m_Surface;
        info.minImageCount    = m_Desc.BufferCount;
        info.imageFormat      = m_VkFormat.format;
        info.imageColorSpace  = m_VkFormat.colorSpace;
        info.imageExtent      = newExtent;
        info.imageArrayLayers = 1;
        info.imageUsage       = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT; //Use as color attachment and clear
        info.preTransform     = cap.Capabilities.currentTransform;
        info.compositeAlpha   = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        info.presentMode      = m_PresentationMode;
        info.clipped          = VK_TRUE;
        info.oldSwapchain     = VK_NULL_HANDLE;
    
        //Is the swapchain going to be used by more than one queue
		QueueFamilyIndices familyIndices = m_pDevice->GetQueueFamilyIndices();
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
        //Create swapchain
        if (vkCreateSwapchainKHR(m_pDevice->GetVkDevice(), &info, nullptr, &m_VkSwapChain) != VK_SUCCESS)
        {
            LOG_DEBUG_ERROR("Vulkan: Failed to create SwapChain\n");
            m_VkSwapChain = VK_NULL_HANDLE;
            return;
        }
        else
        {
            LOG_DEBUG_INFO("Vulkan: Created SwapChain\n");
            m_Desc.BufferWidth  = newExtent.width;
            m_Desc.BufferHeight = newExtent.height;
			
			//Reset semaphore index
			m_SemaphoreIndex = 0;
        }
        

        //Get SwapChain images
        uint32 imageCount = 0;
        vkGetSwapchainImagesKHR(m_pDevice->GetVkDevice(), m_VkSwapChain, &imageCount, nullptr);
        m_Desc.BufferCount = imageCount;
        
        //Init buffers
        std::vector<VkImage> textures(imageCount);
        vkGetSwapchainImagesKHR(m_pDevice->GetVkDevice(), m_VkSwapChain, &imageCount, textures.data());
        for (uint32 i = 0; i < imageCount; i++)
        {
            TextureDesc desc = {};
            desc.Type		 = TEXTURE_TYPE_2D;
            desc.Flags		 = TEXTURE_FLAGS_RENDER_TARGET;
            desc.Format		 = m_Desc.BufferFormat;
            desc.Width		 = m_Desc.BufferWidth;
            desc.Height		 = m_Desc.BufferHeight;
            desc.Depth		 = 1;
            desc.ArraySize	 = 1;
            desc.MipLevels	 = 1;
            desc.SampleCount = 1;
            desc.Usage		 = RESOURCE_USAGE_DEFAULT;

            m_Buffers.push_back(AutoRef(DBG_NEW VKNTexture(m_pDevice, textures[i], desc)));
        }
        
        LOG_DEBUG_INFO("Vulkan: Created Backbuffers\n");
        
        //Create depthbuffer
        TextureDesc depthBufferDesc = {};
        depthBufferDesc.Flags       = TEXTURE_FLAGS_DEPTH_STENCIL;
        depthBufferDesc.Type        = TEXTURE_TYPE_2D;
        depthBufferDesc.Usage       = RESOURCE_USAGE_DEFAULT;
        depthBufferDesc.ArraySize   = 1;
        depthBufferDesc.Width       = m_Desc.BufferWidth;
        depthBufferDesc.Height      = m_Desc.BufferHeight;
        depthBufferDesc.Format      = m_Desc.DepthBufferFormat;
        depthBufferDesc.SampleCount = uint32(m_Desc.BufferSampleCount);
        depthBufferDesc.MipLevels   = 1;
        depthBufferDesc.Depth       = 1;
        m_pDevice->CreateTexture((ITexture**)m_DepthStencilBuffer.GetAdressOf(), nullptr, depthBufferDesc);

        //Create MSAA-buffer
        if (m_Desc.BufferSampleCount > 1)
        {
            TextureDesc msaaBufferDesc = {};
            msaaBufferDesc.Type        = TEXTURE_TYPE_2D;
            msaaBufferDesc.Usage       = RESOURCE_USAGE_DEFAULT;
            msaaBufferDesc.Flags       = TEXTURE_FLAGS_RENDER_TARGET;
            msaaBufferDesc.ArraySize   = 1;
            msaaBufferDesc.Width       = m_Desc.BufferWidth;
            msaaBufferDesc.Height      = m_Desc.BufferHeight;
            msaaBufferDesc.Format      = m_Desc.BufferFormat;
            msaaBufferDesc.SampleCount = uint32(m_Desc.BufferSampleCount);
            msaaBufferDesc.MipLevels   = 1;
            msaaBufferDesc.Depth       = 1;
            m_pDevice->CreateTexture((ITexture**)m_SampleBuffer.GetAdressOf(), nullptr, msaaBufferDesc);
        }
        
        //Aquire next image
        AquireNextImage();
    }


	void VKNSwapChain::AquireNextImage()
	{       
		//Check if next image has been aquired, in case of high framerates we may exceed the number of created buffers
		/*uint32 nextImageIndex = (m_SemaphoreIndex + 1) % m_Desc.BufferCount;
		if (m_SubmittedImageFences[nextImageIndex])
		{
			//Wait for next image to be aquired
			VkFence nextImageFence = m_ImageFences[nextImageIndex];
			if (vkGetFenceStatus(m_pDevice->GetVkDevice(), nextImageFence) == VK_NOT_READY)
			{
				VkResult result = vkWaitForFences(m_pDevice->GetVkDevice(), 1, &nextImageFence, VK_TRUE, UINT64_MAX);
				LAMBDA_ASSERT(result == VK_SUCCESS);
			}
			//Reset this fence
			vkResetFences(m_pDevice->GetVkDevice(), 1, &nextImageFence);

			//This fence is not submitted anymore
			m_SubmittedImageFences[nextImageIndex] = false;
		}*/

		//Aquire next image
		VkFence signalFence			= m_ImageFences[m_SemaphoreIndex];
        VkSemaphore signalSemaphore = m_ImageSemaphores[m_SemaphoreIndex];
		if (vkAcquireNextImageKHR(m_pDevice->GetVkDevice(), m_VkSwapChain, UINT64_MAX, signalSemaphore, VK_NULL_HANDLE, &m_CurrentBufferIndex) == VK_SUCCESS)
        {
            //Add semaphores so the syncronization gets correct
            m_Context->AddWaitSemaphore(signalSemaphore, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT);
			//this fence is now submited
			//m_SubmittedImageFences[m_SemaphoreIndex] = true;
        }
		else
		{
			//m_SubmittedImageFences[m_SemaphoreIndex] = false;
		}
	}


	ITexture* VKNSwapChain::GetBuffer()
	{       
		return (m_Desc.BufferSampleCount > 1) ? m_SampleBuffer.Get() : m_Buffers[m_CurrentBufferIndex].Get();
	}


    ITexture* VKNSwapChain::GetDepthBuffer()
    {
        return m_DepthStencilBuffer.Get();
    }


	void VKNSwapChain::ResizeBuffers(uint32 width, uint32 height)
    {
		if (width == m_Desc.BufferWidth && height == m_Desc.BufferHeight)
			return;

		//Syncronize the GPU so no operations are in flight when recreating swapchain
		m_pDevice->WaitUntilIdle();

        //Relase the old resources
		ReleaseResources();
        
        //Create swapchain again
        VkExtent2D extent = { width, height };
        InitSwapChain(extent);
		
		LOG_DEBUG_INFO("VKNSwapChain: RESIZED w: %d h: %d\n", width, height);
	}


	bool VKNSwapChain::SetFullscreenState(bool fullscreenState)
	{
		return false;
	}

	
	bool VKNSwapChain::GetFullscreenState() const
	{
		return false;
	}


	void VKNSwapChain::Present()
	{
		//If we are using MSAA we need to resolve the resource
		if (m_Desc.BufferSampleCount > 1)
			m_Context->ResolveTexture(m_Buffers[m_CurrentBufferIndex].Get(), 0, m_SampleBuffer.Get(), 0);

		//Transition resource
		m_Context->TransitionTexture(m_Buffers[m_CurrentBufferIndex].Get(), VK_IMAGE_LAYOUT_PRESENT_SRC_KHR, VK_REMAINING_MIP_LEVELS);

		//Add semaphores so the syncronization gets correct
		m_Context->AddSignalSemaphore(m_RenderSemaphores[m_SemaphoreIndex]);
		//LOG_DEBUG_INFO("Signal Semaphore %p\n", m_RenderSemaphores[m_FrameIndex]);

		//Flush context
		m_Context->Flush();

		//Present
		VkSemaphore waitSemaphores[] = { m_RenderSemaphores[m_SemaphoreIndex] };
		VkPresentInfoKHR info = {};
		info.sType				= VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		info.pNext				= nullptr;
		info.waitSemaphoreCount = 1;
		info.pWaitSemaphores	= waitSemaphores;
		info.swapchainCount		= 1;
		info.pSwapchains		= &m_VkSwapChain;
		info.pImageIndices		= &m_CurrentBufferIndex;
		info.pResults			= nullptr;
		VkResult result = m_pDevice->Present(&info);
		if (result == VK_SUBOPTIMAL_KHR || result == VK_ERROR_OUT_OF_DATE_KHR)
		{
			LOG_DEBUG_WARNING("Vulkan: Suboptimal SwapChain result='%s'\n", result == VK_SUBOPTIMAL_KHR ? "VK_SUBOPTIMAL_KHR" : "VK_ERROR_OUT_OF_DATE_KHR");
			//InitSwapChain({ 0, 0 });
		}
        else if (result == VK_SUCCESS)
        {
			//Increase frameindex and get next image
			m_SemaphoreIndex = (m_SemaphoreIndex + 1) % m_Desc.BufferCount;
            //Aquire next image
            AquireNextImage();
        }
        else
        {
            LOG_DEBUG_ERROR("Vulkan: Present Failed\n");
        }

		//LOG_DEBUG_INFO("Waiting Semaphore %p\n", m_RenderSemaphores[m_FrameIndex]);

		//Finish device frame
		m_pDevice->FinishFrame();
	}

	
	void* VKNSwapChain::GetNativeHandle() const
	{
		return reinterpret_cast<void*>(m_VkSwapChain);
	}


	void VKNSwapChain::ReleaseResources()
	{
        //Release DepthStencil- and SampleBuffer
        m_DepthStencilBuffer.Release();
        m_SampleBuffer.Release();
        
		//Release backbuffers
		m_Buffers.clear();

        //Destroy swapchain
		if (m_VkSwapChain != VK_NULL_HANDLE)
		{
			vkDestroySwapchainKHR(m_pDevice->GetVkDevice(), m_VkSwapChain, nullptr);
			m_VkSwapChain = VK_NULL_HANDLE;
		}
	}
}
