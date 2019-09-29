#include "LambdaPch.h"
#include "Events/WindowEvent.h"
#include "VKNGraphicsDevice.h"
#include "VKNDevice.h"
#include "VKNShader.h"
#include "VKNPipelineState.h"
#include "VKNTexture.h"
#include "VKNSamplerState.h"
#include "VKNCommandList.h"
#include "VKNFramebuffer.h"
#include "VKNBuffer.h"
#include "VKNSwapChain.h"
#include "VKNRenderPass.h"
#include "VKNQuery.h"
#include "VKNPipelineResourceState.h"
#include "VKNUtilities.h"
#include "VKNConversions.inl"

namespace Lambda
{   
	//-----------------
	//VKNGraphicsDevice
	//-----------------

    VKNGraphicsDevice::VKNGraphicsDevice(const GraphicsDeviceDesc& desc)
        : m_pDevice(nullptr),
        m_GraphicsQueue(VK_NULL_HANDLE),
        m_PresentationQueue(VK_NULL_HANDLE),
		m_Fences(),
        m_RenderSemaphores(),
		m_ImageSemaphores(),
		m_pBufferManager(nullptr),
		m_pFramebufferCache(nullptr),
		m_pSwapChain(nullptr),
		m_pDepthStencil(nullptr),
		m_pMSAABuffer(nullptr),
		m_pCommandList(nullptr),
		m_pDeviceAllocator(nullptr),
		m_CurrentFrame(0)
    {       
		LAMBDA_ASSERT(s_pInstance == nullptr);
        s_pInstance = this;
        
        Init(desc);
    }
    
    
    VKNGraphicsDevice::~VKNGraphicsDevice()
    {
        ICommandList* pDirectCommandList = m_pCommandList;
        DestroyCommandList(&pDirectCommandList);
        
		GraphicsDeviceDesc desc = m_pDevice->GetDesc();
        for (uint32 i = 0; i < desc.BackBufferCount; i++)
        {
			if (m_ImageSemaphores[i] != VK_NULL_HANDLE)
			{
				vkDestroySemaphore(m_pDevice->GetDevice(), m_ImageSemaphores[i], nullptr);
				m_ImageSemaphores[i] = VK_NULL_HANDLE;
			}
            if (m_RenderSemaphores[i] != VK_NULL_HANDLE)
            {
                vkDestroySemaphore(m_pDevice->GetDevice(), m_RenderSemaphores[i], nullptr);
                m_RenderSemaphores[i] = VK_NULL_HANDLE;
            }
            if (m_Fences[i] != VK_NULL_HANDLE)
            {
                vkDestroyFence(m_pDevice->GetDevice(), m_Fences[i], nullptr);
                m_Fences[i] = VK_NULL_HANDLE;
            }
        }        

		//Release all framebuffers
		if (m_pFramebufferCache)
		{
			m_pFramebufferCache->ReleaseAll(m_pDevice->GetDevice());
		}

		//Destroy window framebuffer
		m_pSwapChain->Destroy(m_pDevice->GetDevice());
        ReleaseDepthStencil();
		ReleaseMSAABuffer();

		SafeDelete(m_pFramebufferCache);
		SafeDelete(m_pBufferManager);
		SafeDelete(m_pDescriptorPoolManager);

		//Release all memory
		if (m_pDeviceAllocator)
		{
			m_pDeviceAllocator->Destroy(m_pDevice->GetDevice());
			m_pDeviceAllocator = nullptr;
		}

		if (m_pDevice)
		{
			m_pDevice->Destroy();
			m_pDevice = nullptr;
		}

		//Set global instance to nullptr
		if (s_pInstance == this)
			s_pInstance = nullptr;
    }
    
    
    void VKNGraphicsDevice::Init(const GraphicsDeviceDesc& desc)
    {
		LAMBDA_ASSERT(desc.pWindow != nullptr);
       
		//Create device
		m_pDevice = DBG_NEW VKNDevice(desc);
		VkPhysicalDeviceProperties props = m_pDevice->GetPhysicalDeviceProperties();
		strcpy(m_Properties.AdapterString, props.deviceName);
		if (props.vendorID == VENDOR_ID_NVIDIA)
			strcpy(m_Properties.VendorString, "NVIDIA");
		else if (props.vendorID == VENDOR_ID_AMD)
			strcpy(m_Properties.VendorString, "AMD");
		else if (props.vendorID == VENDOR_ID_INTEL)
			strcpy(m_Properties.VendorString, "INTEL");
		else if (props.vendorID == VENDOR_ID_ARM)
			strcpy(m_Properties.VendorString, "ARM");
		else if (props.vendorID == VENDOR_ID_QUALCOMM)
			strcpy(m_Properties.VendorString, "Qualcomm");
		else if (props.vendorID == VENDOR_ID_IMGTEC)
			strcpy(m_Properties.VendorString, "ImgTec");

		//Get queues
		QueueFamilyIndices familiyIndices = m_pDevice->GetQueueFamilyIndices();
		vkGetDeviceQueue(m_pDevice->GetDevice(), familiyIndices.GraphicsFamily, 0, &m_GraphicsQueue);
		vkGetDeviceQueue(m_pDevice->GetDevice(), familiyIndices.PresentFamily, 0, &m_PresentationQueue);

        //Setup semaphore structure
        VkSemaphoreCreateInfo semaphoreInfo = {};
        semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
        semaphoreInfo.pNext = nullptr;
        semaphoreInfo.flags = 0;
        
        //Setup fence struct
        VkFenceCreateInfo fenceInfo = {};
        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        
        //Create sync-objects
        m_Fences.resize(desc.BackBufferCount);
        m_ImageSemaphores.resize(desc.BackBufferCount);
        m_RenderSemaphores.resize(desc.BackBufferCount);
        for (uint32 i = 0; i < desc.BackBufferCount; i++)
        {
            //Create semaphores
            if (vkCreateSemaphore(m_pDevice->GetDevice(), &semaphoreInfo, nullptr, &m_ImageSemaphores[i]) != VK_SUCCESS ||
                vkCreateSemaphore(m_pDevice->GetDevice(), &semaphoreInfo, nullptr, &m_RenderSemaphores[i]) != VK_SUCCESS)
            {
                LOG_DEBUG_ERROR("Vulkan: Failed to create Semaphore\n");
                return;
            }
            else
            {
                m_pDevice->SetVulkanObjectName(VK_OBJECT_TYPE_SEMAPHORE, (uint64)m_ImageSemaphores[i], "ImageSemaphore[" +  std::to_string(i) + "]");
                m_pDevice->SetVulkanObjectName(VK_OBJECT_TYPE_SEMAPHORE, (uint64)m_RenderSemaphores[i], "RenderSemaphore[" + std::to_string(i) + "]");
            }
            
            //Create fence
            if (vkCreateFence(m_pDevice->GetDevice(), &fenceInfo, nullptr, &m_Fences[i]) != VK_SUCCESS)
            {
                LOG_DEBUG_ERROR("Vulkan: Failed to create fence\n");
                return;
            }
            else
            {
				m_pDevice->SetVulkanObjectName(VK_OBJECT_TYPE_FENCE, (uint64)m_Fences[i], "Fence[" +  std::to_string(i) + "]");
            }
        }
        
        LOG_DEBUG_INFO("Vulkan: Created Semaphores and fences\n");
        
		//Create allocator
		m_pDeviceAllocator			= DBG_NEW VKNAllocator();
		//Create descriptorpoolmanager
		m_pDescriptorPoolManager	= DBG_NEW VKNDescriptorPoolManager();
		//Create dynamic buffer manager
		m_pBufferManager		= DBG_NEW VKNBufferManager();
		//Create framebuffercache
		m_pFramebufferCache			= DBG_NEW VKNFramebufferCache();
        
        //Create swapchain
        VKNSwapChainDesc swapChainInfo = {};
        swapChainInfo.VerticalSync       = desc.VerticalSync;
        swapChainInfo.Format.format      = VK_FORMAT_B8G8R8A8_UNORM;
        swapChainInfo.Format.colorSpace  = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
        swapChainInfo.Extent             = { desc.pWindow->GetWidth(), desc.pWindow->GetHeight() };
        swapChainInfo.ImageCount         = desc.BackBufferCount;
        m_pSwapChain = DBG_NEW VKNSwapChain(swapChainInfo);

		//If we are using MSAA we need to create a seperate texture
		if (desc.SampleCount > VK_SAMPLE_COUNT_1_BIT)
		{
			if (!CreateMSAABuffer())
			{
				LOG_DEBUG_INFO("Vulkan: Failed to create MSAABuffer\n");
				return;
			}
		}

        //Create depthbuffer
        if (!CreateDepthStencil())
        {
            LOG_DEBUG_INFO("Vulkan: Failed to create DepthBuffer\n");
            return;
        }
         
        //Init internal commandlist, used for copying from staging buffers to final resource etc.
		ICommandList* pList = nullptr;
        CreateCommandList(&pList, COMMAND_LIST_TYPE_GRAPHICS);
		if (pList)
		{
			m_pCommandList = reinterpret_cast<VKNCommandList*>(pList);
			m_pCommandList->SetName("Graphics Device Internal CommandList");
		}
    }
    
    
    bool VKNGraphicsDevice::CreateDepthStencil()
    {
		GraphicsDeviceDesc deviceDesc = m_pDevice->GetDesc();

        TextureDesc depthBufferDesc			= {};
        depthBufferDesc.pResolveResource	= nullptr;
        depthBufferDesc.Flags				= TEXTURE_FLAGS_DEPTH_STENCIL;
        depthBufferDesc.Type				= TEXTURE_TYPE_2D;
        depthBufferDesc.Usage				= RESOURCE_USAGE_DEFAULT;
        depthBufferDesc.ArraySize			= 1;
        depthBufferDesc.Width				= m_pSwapChain->GetWidth();
        depthBufferDesc.Height				= m_pSwapChain->GetHeight();
        depthBufferDesc.Format				= FORMAT_D24_UNORM_S8_UINT;
        depthBufferDesc.SampleCount			= uint32(deviceDesc.SampleCount);
        depthBufferDesc.MipLevels			= 1;
		depthBufferDesc.Depth				= 1;
        
        m_pDepthStencil = DBG_NEW VKNTexture(m_pDeviceAllocator, depthBufferDesc);
        return true;
    }


	bool VKNGraphicsDevice::CreateMSAABuffer()
	{
		GraphicsDeviceDesc deviceDesc = m_pDevice->GetDesc();

		TextureDesc msaaBufferDesc		= {};
        msaaBufferDesc.pResolveResource = m_pSwapChain->GetCurrentBuffer();
		msaaBufferDesc.Type				= TEXTURE_TYPE_2D;
		msaaBufferDesc.Usage			= RESOURCE_USAGE_DEFAULT;
		msaaBufferDesc.Flags			= TEXTURE_FLAGS_RENDER_TARGET;
		msaaBufferDesc.ArraySize		= 1;
		msaaBufferDesc.Width			= m_pSwapChain->GetWidth();
		msaaBufferDesc.Height			= m_pSwapChain->GetHeight();
		msaaBufferDesc.Format			= FORMAT_B8G8R8A8_UNORM;
		msaaBufferDesc.SampleCount		= uint32(deviceDesc.SampleCount);
		msaaBufferDesc.MipLevels		= 1;
		msaaBufferDesc.Depth			= 1;

		m_pMSAABuffer = DBG_NEW VKNTexture(m_pDeviceAllocator, msaaBufferDesc);
		return true;
	}
    
    
    void VKNGraphicsDevice::ReleaseDepthStencil()
    {
        if (m_pDepthStencil != nullptr)
        {
            m_pDepthStencil->Destroy(m_pDevice->GetDevice());
            m_pDepthStencil = nullptr;
        }
    }


	void VKNGraphicsDevice::ReleaseMSAABuffer()
	{
		if (m_pMSAABuffer != nullptr)
		{
			m_pMSAABuffer->Destroy(m_pDevice->GetDevice());
			m_pMSAABuffer = nullptr;
		}
	}
    
    
    void VKNGraphicsDevice::CreateCommandList(ICommandList** ppList, CommandListType type)
    {
		LAMBDA_ASSERT(ppList != nullptr);
        (*ppList) = DBG_NEW VKNCommandList(m_pDeviceAllocator, type);
    }
    
    
    void VKNGraphicsDevice::CreateBuffer(IBuffer** ppBuffer, const ResourceData* pInitalData, const BufferDesc& desc)
    {
		LAMBDA_ASSERT(ppBuffer != nullptr);
        
        //Create buffer
        VKNBuffer* pVkBuffer = DBG_NEW VKNBuffer(m_pDeviceAllocator, desc);
        if (desc.Usage == RESOURCE_USAGE_DYNAMIC)
            m_pBufferManager->RegisterBuffer(pVkBuffer);
        
        //Upload inital data
        if (pInitalData)
        {
            LAMBDA_ASSERT(pInitalData->pData != nullptr && pInitalData->SizeInBytes != 0);
            
            if (desc.Usage == RESOURCE_USAGE_DYNAMIC)
            {
                //Upload directly to buffer if it is dynamic
                void* pMappedData = nullptr;
                
                pVkBuffer->Map(&pMappedData);
                memcpy(pMappedData, pInitalData->pData, pInitalData->SizeInBytes);
                pVkBuffer->Unmap();
            }
            else if (desc.Usage == RESOURCE_USAGE_DEFAULT)
            {
				//Setup copy with staging buffer if other usecase
				m_pCommandList->Reset();
                m_pCommandList->UpdateBuffer(pVkBuffer, pInitalData);
                m_pCommandList->Close();
            
                ICommandList* ppLists[] = { m_pCommandList };
                ExecuteCommandList(ppLists, 1);

                WaitForGPU();
            }
        }

        (*ppBuffer) = pVkBuffer;
    }
    
    
    void VKNGraphicsDevice::CreateTexture(ITexture** ppTexture, const ResourceData* pInitalData, const TextureDesc& desc)
    {
		LAMBDA_ASSERT(ppTexture != nullptr);
        
        //Create texture object
        VKNTexture* pVkTexture = DBG_NEW VKNTexture(m_pDeviceAllocator, desc);
        
        //Handle inital data
        if (pInitalData)
        {
            LAMBDA_ASSERT(pInitalData->pData != nullptr && pInitalData->SizeInBytes != 0);
            
            m_pCommandList->Reset();
            //Upload data
            m_pCommandList->UpdateTexture(pVkTexture, pInitalData, 0);

            //Generate mipmaps
            if (desc.Flags & TEXTURE_FLAGS_GENEATE_MIPS)
            {
                VkFormatProperties formatProperties = {};
                vkGetPhysicalDeviceFormatProperties(m_pDevice->GetPhysicalDevice(), pVkTexture->GetFormat(), &formatProperties);
                
                if (formatProperties.linearTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT)
                {
                    TextureDesc textureDesc = pVkTexture->GetDesc();
                    int32 mipWidth			= textureDesc.Width;
                    int32 mipHeight			= textureDesc.Height;
                    uint32 mipLevels		= textureDesc.MipLevels;
                    for (uint32 i = 1; i < mipLevels; i++)
                    {
                        m_pCommandList->TransitionTexture(pVkTexture, RESOURCE_STATE_COPY_SRC, i-1, 1);
                        m_pCommandList->BlitTexture(pVkTexture, mipWidth > 1 ? mipWidth / 2 : 1, mipHeight > 1 ? mipHeight / 2 : 1, i, pVkTexture, mipWidth, mipHeight, i-1);
                        m_pCommandList->TransitionTexture(pVkTexture, RESOURCE_STATE_COPY_DEST, i-1, 1);
                        
                        if (mipWidth > 1)
                            mipWidth /= 2;

                        if (mipHeight > 1)
                            mipHeight /= 2;
                    }
                }
                else
                {
                    LOG_DEBUG_ERROR("Vulkan: PhysicalDevice does not support mipmap generation for this format\n");
                }
            }
            
            //Execute and wait for GPU
            m_pCommandList->Close();
            
            ICommandList* ppLists[] = { m_pCommandList };
            ExecuteCommandList(ppLists, 1);

            WaitForGPU();
        }
        
        //Return texture
        (*ppTexture) = pVkTexture;
    }
    
    
    void VKNGraphicsDevice::CreateShader(IShader** ppShader, const ShaderDesc& desc)
    {
		LAMBDA_ASSERT(ppShader != nullptr);
        (*ppShader) = DBG_NEW VKNShader(desc);
    }
    
    
    void VKNGraphicsDevice::CreateSamplerState(ISamplerState** ppSamplerState, const SamplerStateDesc& desc)
    {
		LAMBDA_ASSERT(ppSamplerState != nullptr);
        (*ppSamplerState) = DBG_NEW VKNSamplerState(desc);
    }
    
    
    void VKNGraphicsDevice::CreateGraphicsPipelineState(IGraphicsPipelineState** ppPipelineState, const GraphicsPipelineStateDesc& desc)
    {
		LAMBDA_ASSERT(ppPipelineState != nullptr);
        (*ppPipelineState) = DBG_NEW VKNGraphicsPipelineState(desc);
    }


	void VKNGraphicsDevice::CreateRenderPass(IRenderPass** ppRenderPass, const RenderPassDesc& desc)
	{
		LAMBDA_ASSERT(ppRenderPass != nullptr);
		(*ppRenderPass) = DBG_NEW VKNRenderPass(desc);
	}


	void VKNGraphicsDevice::CreatePipelineResourceState(IPipelineResourceState** ppResourceState, const PipelineResourceStateDesc& desc)
	{
		LAMBDA_ASSERT(ppResourceState != nullptr);
		(*ppResourceState) = DBG_NEW VKNPipelineResourceState(desc);
	}

    
    void VKNGraphicsDevice::CreateQuery(Lambda::IQuery** ppQuery, const QueryDesc& desc)
    {
        LAMBDA_ASSERT(ppQuery != nullptr);
        (*ppQuery) = DBG_NEW VKNQuery(desc);
    }
    
    
    void VKNGraphicsDevice::DestroyCommandList(ICommandList** ppList)
    {
		LAMBDA_ASSERT(ppList != nullptr);
        
        //Delete list
        VKNCommandList* pVkList = reinterpret_cast<VKNCommandList*>(*ppList);
        if (pVkList != nullptr)
        {
            pVkList->Destroy(m_pDevice->GetDevice());
            *ppList = nullptr;
        }
    }
    
    
    void VKNGraphicsDevice::DestroyBuffer(IBuffer** ppBuffer)
    {
		LAMBDA_ASSERT(ppBuffer != nullptr);
        
        //Delete buffer
        VKNBuffer* pVkBuffer = reinterpret_cast<VKNBuffer*>(*ppBuffer);
        if (pVkBuffer != nullptr)
        {
			//If buffer is dynamic, unregister
			BufferDesc bufferDesc = pVkBuffer->GetDesc();
			if (bufferDesc.Usage == RESOURCE_USAGE_DYNAMIC)
			{
				m_pBufferManager->UnregisterBuffer(pVkBuffer);
			}

			//Then destroy buffer
            pVkBuffer->Destroy(m_pDevice->GetDevice());
            *ppBuffer = nullptr;
            
            LOG_DEBUG_INFO("Vulkan: Destroyed Buffer\n");
        }
    }
    
    
    void VKNGraphicsDevice::DestroyTexture(ITexture** ppTexture)
    {
		LAMBDA_ASSERT(ppTexture != nullptr);
        
        //Delete texture
        VKNTexture* pVkTexture = reinterpret_cast<VKNTexture*>(*ppTexture);
        if (pVkTexture != nullptr)
        {
            pVkTexture->Destroy(m_pDevice->GetDevice());
            *ppTexture = nullptr;
            
            LOG_DEBUG_INFO("Vulkan: Destroyed Texture2D\n");
        }
    }
    
    
    void VKNGraphicsDevice::DestroyShader(IShader** ppShader)
    {
		LAMBDA_ASSERT(ppShader != nullptr);
        
        //Delete shader
        VKNShader* pVkShader = reinterpret_cast<VKNShader*>(*ppShader);
        if (pVkShader != nullptr)
        {
            pVkShader->Destroy(m_pDevice->GetDevice());
            *ppShader = nullptr;
            
            LOG_DEBUG_INFO("Vulkan: Destroyed Shader\n");
        }
    }
    
    
    void VKNGraphicsDevice::DestroySamplerState(ISamplerState** ppSamplerState)
    {
		LAMBDA_ASSERT(ppSamplerState != nullptr);
        
        //Delete SamplerState
        VKNSamplerState* pVkSamplerState = reinterpret_cast<VKNSamplerState*>(*ppSamplerState);
        if (pVkSamplerState != nullptr)
        {
            pVkSamplerState->Destroy(m_pDevice->GetDevice());
            *ppSamplerState = nullptr;
            
            LOG_DEBUG_INFO("Vulkan: Destroyed SamplerState\n");
        }
    }
    
    
    void VKNGraphicsDevice::DestroyGraphicsPipelineState(IGraphicsPipelineState** ppPipelineState)
    {
		LAMBDA_ASSERT(ppPipelineState != nullptr);
        
        //Delete PipelineState
        VKNGraphicsPipelineState* pVkPipelineState = reinterpret_cast<VKNGraphicsPipelineState*>(*ppPipelineState);
        if (pVkPipelineState != nullptr)
        {
			pVkPipelineState->Destroy(m_pDevice->GetDevice());
            *ppPipelineState = nullptr;
            
            LOG_DEBUG_INFO("Vulkan: Destroyed PipelineState\n");
        }
    }


	void VKNGraphicsDevice::DestroyRenderPass(IRenderPass** ppRenderPass)
	{
		LAMBDA_ASSERT(ppRenderPass != nullptr);

		//Delete PipelineState
		VKNRenderPass* pVkRenderPass = reinterpret_cast<VKNRenderPass*>(*ppRenderPass);
		if (pVkRenderPass != nullptr)
		{
			pVkRenderPass->Destroy(m_pDevice->GetDevice());
			*ppRenderPass = nullptr;

			LOG_DEBUG_INFO("Vulkan: Destroyed RenderPass\n");
		}
	}


	void VKNGraphicsDevice::DestroyResourceState(IPipelineResourceState** ppResourceState)
	{
		LAMBDA_ASSERT(ppResourceState != nullptr);

		//Delete ResourceState
		VKNPipelineResourceState* pVkResourceState = reinterpret_cast<VKNPipelineResourceState*>(*ppResourceState);
		if (pVkResourceState != nullptr)
		{
			pVkResourceState->Destroy(m_pDevice->GetDevice());
			*ppResourceState = nullptr;

			LOG_DEBUG_INFO("Vulkan: Destroyed ResourceState\n");
		}
	}
    
    
    void VKNGraphicsDevice::DestroyQuery(IQuery** ppQuery)
    {
        LAMBDA_ASSERT(ppQuery != nullptr);
        
        //Delete ResourceState
        VKNQuery* pVkQuery = reinterpret_cast<VKNQuery*>(*ppQuery);
        if (pVkQuery != nullptr)
        {
            pVkQuery->Destroy(m_pDevice->GetDevice());
            *ppQuery = nullptr;
            
            LOG_DEBUG_INFO("Vulkan: Destroyed Query\n");
        }
    }
    
    
    void VKNGraphicsDevice::Destroy() const
    {
		delete this;
    }
    
    
    void VKNGraphicsDevice::ExecuteCommandList(ICommandList* const * ppLists, uint32 numLists) const
    {
        //Retrive commandbuffers
        std::vector<VkCommandBuffer> buffers;
        for (uint32 i = 0; i < numLists; i++)
		{
            buffers.push_back(reinterpret_cast<VkCommandBuffer>(ppLists[i]->GetNativeHandle()));
		}

        //submit commandbuffers
        VkSubmitInfo submitInfo = {};
        submitInfo.sType				= VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.pNext				= nullptr;
        submitInfo.waitSemaphoreCount	= 0;
        submitInfo.pWaitSemaphores		= nullptr;
        submitInfo.pWaitDstStageMask	= nullptr;
        submitInfo.commandBufferCount	= numLists;
        submitInfo.pCommandBuffers		= buffers.data();
        submitInfo.signalSemaphoreCount = 0;
        submitInfo.pSignalSemaphores	= nullptr;

        if (vkQueueSubmit(m_GraphicsQueue, 1, &submitInfo, VK_NULL_HANDLE) != VK_SUCCESS)
        {
            LOG_DEBUG_ERROR("Vulkan: Failed to submit CommandBuffers\n");
        }
    }


	void VKNGraphicsDevice::PresentBegin() const
	{
		m_pSwapChain->AquireNextImage(m_ImageSemaphores[m_CurrentFrame]);

		//if we use MSAA we want to set a texture that we can resolve onto, in this case the current backbuffer since we render to the window
		if (m_pMSAABuffer)
		{
			VKNTexture* pResolveResource = reinterpret_cast<VKNTexture*>(m_pSwapChain->GetCurrentBuffer());
			m_pMSAABuffer->SetResolveResource(pResolveResource);
		}
	}

    
	void VKNGraphicsDevice::PresentEnd(ICommandList* const* ppLists, uint32 numLists) const
	{
		//LOG_DEBUG_INFO("Vulkan: VKNGraphicsDevice::ExecuteCommandListAndPresent  Frame '%d' - WaitSemaphore='%x', SignalSemaphore='%x'\n", m_CurrentFrame, m_ImageSemaphores[m_CurrentFrame], m_RenderSemaphores[m_CurrentFrame]);

		//Retrive commandbuffers
		std::vector<VkCommandBuffer> buffers;
		for (uint32 i = 0; i < numLists; i++)
		{
			buffers.push_back(reinterpret_cast<VkCommandBuffer>(ppLists[i]->GetNativeHandle()));
		}

		//Setup "syncobjects"
		VkSemaphore waitSemaphores[]		= { m_ImageSemaphores[m_CurrentFrame] };
		VkSemaphore signalSemaphores[]		= { m_RenderSemaphores[m_CurrentFrame] };
		VkPipelineStageFlags waitStages[]	= { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

		//submit commandbuffers
		VkSubmitInfo submitInfo = {};
		submitInfo.sType				= VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.pNext				= nullptr;
		submitInfo.waitSemaphoreCount	= 1;
		submitInfo.pWaitSemaphores		= waitSemaphores;
		submitInfo.pWaitDstStageMask	= waitStages;
		submitInfo.commandBufferCount	= numLists;
		submitInfo.pCommandBuffers		= buffers.data();
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores	= signalSemaphores;

		if (vkQueueSubmit(m_GraphicsQueue, 1, &submitInfo, m_Fences[m_CurrentFrame]) != VK_SUCCESS)
		{
			LOG_DEBUG_ERROR("Vulkan: Failed to submit CommandBuffers\n");
		}
		else
		{
			m_pSwapChain->Present(m_PresentationQueue, m_RenderSemaphores[m_CurrentFrame]);
			GPUWaitForFrame();
		}
	}
    
    
    void VKNGraphicsDevice::GPUWaitForFrame() const
    {
        //Wait for last frame
        vkWaitForFences(m_pDevice->GetDevice(), 1, &m_Fences[m_CurrentFrame], VK_TRUE, std::numeric_limits<uint64_t>::max());
        vkResetFences(m_pDevice->GetDevice(), 1, &m_Fences[m_CurrentFrame]);
        
		//Advance current frame counter
		GraphicsDeviceDesc desc = m_pDevice->GetDesc();
		m_CurrentFrame = (m_CurrentFrame + 1) % desc.BackBufferCount;

        //Cleanup memory
        m_pBufferManager->AdvanceFrame();
        m_pDeviceAllocator->EmptyGarbageMemory();
		//Cleanup descriptorpool
		m_pDescriptorPoolManager->Cleanup();
    }
    
    
    void VKNGraphicsDevice::WaitForGPU() const
    {
        //LOG_DEBUG_INFO("VKNGraphicsDevice::WaitForGPU\n");
        VKNDevice& device = VKNDevice::Get();
        vkDeviceWaitIdle(device.GetDevice());
    }

    
    ResourceFormat VKNGraphicsDevice::GetBackBufferFormat() const
    {
        return ConvertVkFormat(m_pSwapChain->GetFormat());
    }
    
    
    void* VKNGraphicsDevice::GetNativeHandle() const
    {
        return reinterpret_cast<void*>(m_pDevice->GetDevice());
    }

	
	DeviceProperties VKNGraphicsDevice::GetProperties() const
	{
		return m_Properties;
	}
    
    
    GraphicsDeviceDesc VKNGraphicsDevice::GetDesc() const
    {
        return m_pDevice->GetDesc();
    }
    
    
    ITexture* VKNGraphicsDevice::GetRenderTarget() const
    {
		GraphicsDeviceDesc deviceDesc = m_pDevice->GetDesc();
        return (deviceDesc.SampleCount > VK_SAMPLE_COUNT_1_BIT) ? m_pMSAABuffer : m_pSwapChain->GetCurrentBuffer();
    }
    
    
    ITexture* VKNGraphicsDevice::GetDepthStencil() const
    {
        return m_pDepthStencil;
    }
    
    
    uint32 VKNGraphicsDevice::GetBackBufferIndex() const
    {
        return m_pSwapChain->GetBackBufferIndex();
    }
    
    
    uint32 VKNGraphicsDevice::GetSwapChainWidth() const
    {
        return m_pSwapChain->GetWidth();
    }
    
    
    uint32 VKNGraphicsDevice::GetSwapChainHeight() const
    {
        return m_pSwapChain->GetHeight();
    }
    
    
    bool VKNGraphicsDevice::OnResize(const WindowResizeEvent& event)
    {
        //When we minimize or any other reason the size is zero
        //Do not resize if the size is the same as the current one
        if ((event.GetWidth() == 0 || event.GetHeight() == 0) ||
            (event.GetWidth() == m_pSwapChain->GetWidth() && event.GetHeight() == m_pSwapChain->GetHeight()))
        {
            return false;
        }
            
        //Syncronize the GPU so no operations are in flight when recreating swapchain
        WaitForGPU();
           
        m_pSwapChain->ResizeBuffers(event.GetWidth(), event.GetHeight());

		//Recreate depthstencil
        ReleaseDepthStencil();
        CreateDepthStencil();

		//Recreate MSAA buffer if MSAA is used
		GraphicsDeviceDesc deviceDesc = m_pDevice->GetDesc();
		if (deviceDesc.SampleCount > VK_SAMPLE_COUNT_1_BIT)
		{
			ReleaseMSAABuffer();
			CreateMSAABuffer();
		}
            
        LOG_DEBUG_INFO("VulkanGraphicsDevice: Window resized w: %d h: %d\n", event.GetWidth(), event.GetHeight());

        return false;
    }


	VKNGraphicsDevice& VKNGraphicsDevice::Get()
	{
		LAMBDA_ASSERT(s_pInstance != nullptr);
		
		VKNGraphicsDevice* pVkDevice = reinterpret_cast<VKNGraphicsDevice*>(s_pInstance);
		return *pVkDevice;
    }
}
