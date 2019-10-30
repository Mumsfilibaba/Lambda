#include "LambdaPch.h"
#include "VKNDevice.h"
#include "VKNQuery.h"
#include "VKNBuffer.h"
#include "VKNShader.h"
#include "VKNTexture.h"
#include "VKNUtilities.h"
#include "VKNSwapChain.h"
#include "VKNSamplerState.h"
#include "VKNDeviceContext.h"
#include "VKNPipelineState.h"
#include "VKNRenderPassCache.h"
#include "VKNFramebufferCache.h"
#include "VKNConversions.inl"
#if defined(LAMBDA_PLAT_MACOS)
    #include <GLFW/glfw3.h>
#endif

namespace Lambda
{   
	//---------
	//VKNDevice
	//---------

	PFN_vkSetDebugUtilsObjectNameEXT	VKNDevice::vkSetDebugUtilsObjectNameEXT		= nullptr;
	PFN_vkCreateDebugUtilsMessengerEXT	VKNDevice::vkCreateDebugUtilsMessengerEXT	= nullptr;
	PFN_vkDestroyDebugUtilsMessengerEXT	VKNDevice::vkDestroyDebugUtilsMessengerEXT	= nullptr;

    VKNDevice::VKNDevice(const DeviceDesc& desc)
        : DeviceBase(desc),
		m_GraphicsQueue(VK_NULL_HANDLE),
        m_PresentationQueue(VK_NULL_HANDLE),
		m_pDeviceAllocator(nullptr),
		m_pDynamicMemoryAllocator(nullptr),
		m_pFramebufferCache(nullptr),
		m_pRenderPassCache(nullptr),
		m_pImmediateContext(nullptr),
		m_Instance(VK_NULL_HANDLE),
		m_DebugMessenger(VK_NULL_HANDLE),
		m_Device(VK_NULL_HANDLE),
		m_PhysicalDevice(VK_NULL_HANDLE),
		m_FamiliyIndices(),
		m_PhysicalDeviceProperties()
    {   
        //Add a ref to the refcounter
        this->AddRef();
        Init(desc);
    }
    
    
    VKNDevice::~VKNDevice()
    {
		//Idle device before destroying
		WaitUntilIdle();

		//Release default resources
		SafeRelease(m_pDefaultVertexBuffer);
		SafeRelease(m_pDefaultIndexBuffer);
		SafeRelease(m_pDefaultConstantBuffer);
		SafeRelease(m_pDefaultTexture);
		SafeRelease(m_pDefaultSamplerState);

		//Release context
		SafeRelease(m_pImmediateContext);
        
		//Release all renderpasses
		if (m_pRenderPassCache)
			m_pRenderPassCache->ReleaseAll();

		//Release all framebuffers
		if (m_pFramebufferCache)
			m_pFramebufferCache->ReleaseAll();

		SafeDelete(m_pDynamicMemoryAllocator);
		SafeDelete(m_pSafeReleaseManager);
		SafeDelete(m_pRenderPassCache);
		SafeDelete(m_pFramebufferCache);
		SafeDelete(m_pDeviceAllocator);

		if (m_Device != VK_NULL_HANDLE)
		{
			vkDestroyDevice(m_Device, nullptr);
			m_Device = VK_NULL_HANDLE;
		}
		if (m_DebugMessenger != VK_NULL_HANDLE)
		{
			vkDestroyDebugUtilsMessengerEXT(m_Instance, m_DebugMessenger, nullptr);
			m_DebugMessenger = VK_NULL_HANDLE;
		}
		if (m_Instance != VK_NULL_HANDLE)
		{
			vkDestroyInstance(m_Instance, nullptr);
			m_Instance = VK_NULL_HANDLE;
		}
    }
    
    
    void VKNDevice::Init(const DeviceDesc& desc)
    {
		VkApplicationInfo applicationInfo = {};
		applicationInfo.sType               = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		applicationInfo.pNext               = nullptr;
		applicationInfo.pApplicationName    = "Lambda Engine";
		applicationInfo.applicationVersion  = VK_MAKE_VERSION(1, 0, 0);
        applicationInfo.pEngineName         = "Lambda Engine";
		applicationInfo.engineVersion       = VK_MAKE_VERSION(1, 0, 0);
		applicationInfo.apiVersion          = VK_API_VERSION_1_1;
        
		//Get all available layers
		uint32_t layerCount = 0;
		vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

		std::vector<VkLayerProperties> availableLayers(layerCount);
		vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

		if (!availableLayers.empty())
		{
			LOG_DEBUG_INFO("[Vulkan] Available Instance-Layers (count=%d):\n", availableLayers.size());
			for (const auto& layer : availableLayers)
			{
				const char* name = layer.layerName;
				LOG_DEBUG_INFO("   Instance-Layer '%s'\n", name);
			}
		}
		else
		{
			LOG_DEBUG_ERROR("Vulkan: No available Instance-Layers\n");
		}
        
        //Get required layers
        std::vector<const char*> requiredLayers = GetRequiredValidationLayers(desc.Flags & DEVICE_FLAG_DEBUG);
        if (!requiredLayers.empty())
        {
            LOG_DEBUG_INFO("[Vulkan] Required Instance-Layers:\n");
            for (const auto& layer : requiredLayers)
                LOG_DEBUG_INFO("   Instance-Layer '%s'\n", layer);
        }

		//Get optional layers
		std::vector<const char*> optionalLayers = GetOptionalValidationLayers(desc.Flags & DEVICE_FLAG_DEBUG);
		if (!optionalLayers.empty())
		{
			LOG_DEBUG_INFO("[Vulkan] Optional Instance-Layers:\n");
			for (const auto& layer : optionalLayers)
				LOG_DEBUG_INFO("   Instance-Layer '%s'\n", layer);
		}

		//Add optional layers. if we found a optional layer among the available ones we add it
		for (const char* layerName : optionalLayers)
		{
			for (const auto& layer : availableLayers)
			{
				if (strcmp(layerName, layer.layerName) == 0)
				{
					requiredLayers.push_back(layerName);
					break;
				}
			}
		}


        //Get all the available extensions
        uint32 availableExtensionsCount = 0;
        vkEnumerateInstanceExtensionProperties(nullptr, &availableExtensionsCount, nullptr);

        std::vector<VkExtensionProperties> availableInstanceExtensions(availableExtensionsCount);
        vkEnumerateInstanceExtensionProperties(nullptr, &availableExtensionsCount, availableInstanceExtensions.data());

        if (!availableInstanceExtensions.empty())
        {
            LOG_DEBUG_INFO("[Vulkan] Available Instance-Extensions (count=%d):\n", availableInstanceExtensions.size());
            for (const auto& extension : availableInstanceExtensions)
            {
                const char* name = extension.extensionName;
                LOG_DEBUG_INFO("   Instance-Extension '%s'\n", name);
            }
        }
        else
        {
            LOG_DEBUG_ERROR("Vulkan: No available Instance-Extensions\n");
        }
        

		//Get extensions
		std::vector<const char*> instanceExtensions = GetRequiredInstanceExtensions(desc.Flags & DEVICE_FLAG_DEBUG);
		if (!instanceExtensions.empty())
		{
			LOG_DEBUG_INFO("[Vulkan] Required Instance-Extensions:\n");
			for (const auto& extension : instanceExtensions)
				LOG_DEBUG_INFO("   Instance-Extension '%s'\n", extension);
		}

		//Get extensions
		std::vector<const char*> optionalInstanceExtensions = GetOptionalInstanceExtensions(desc.Flags & DEVICE_FLAG_DEBUG);
		if (!optionalInstanceExtensions.empty())
		{
			LOG_DEBUG_INFO("[Vulkan] Optional Instance-Extensions:\n");
			for (const auto& extension : optionalInstanceExtensions)
				LOG_DEBUG_INFO("   Instance-Extension '%s'\n", extension);
		}

		//Add optional extensions. if we found a optional extension among the available ones we add it
		for (const char* extensionName : optionalInstanceExtensions)
		{
			for (const auto& extension : availableInstanceExtensions)
			{
				if (strcmp(extensionName, extension.extensionName) == 0)
				{
					instanceExtensions.push_back(extensionName);
					break;
				}
			}
		}


		VkDebugUtilsMessengerCreateInfoEXT dInfo;
		InitDebugMessengerCreateInfo(&dInfo);
        
		//Create instance
		VkInstanceCreateInfo instanceInfo = {};
		instanceInfo.sType						= VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		instanceInfo.pNext						= (desc.Flags & DEVICE_FLAG_DEBUG) ? (VkDebugUtilsMessengerCreateInfoEXT*)&dInfo : nullptr;
		instanceInfo.flags						= 0;
		instanceInfo.pApplicationInfo			= &applicationInfo;
        instanceInfo.enabledLayerCount			= uint32(requiredLayers.size());
        instanceInfo.ppEnabledLayerNames		= requiredLayers.data();
		instanceInfo.enabledExtensionCount		= uint32(instanceExtensions.size());
		instanceInfo.ppEnabledExtensionNames	= instanceExtensions.data();
		VkResult res = vkCreateInstance(&instanceInfo, nullptr, &m_Instance);
		if (res != VK_SUCCESS)
		{
			LOG_DEBUG_ERROR("Vulkan: Failed to create Vulkan Instance. Error %d\n", res);

			m_Instance = VK_NULL_HANDLE;
			return;
		}
		else
		{
			LOG_SYSTEM_PRINT("Vulkan: Created Vulkan instance\n");
		}


		//Get instance functions
		vkSetDebugUtilsObjectNameEXT = (PFN_vkSetDebugUtilsObjectNameEXT)vkGetInstanceProcAddr(m_Instance, "vkSetDebugUtilsObjectNameEXT");
		if (!vkSetDebugUtilsObjectNameEXT)
		{
			LOG_DEBUG_ERROR("Vulkan: Failed to retrive 'vkSetDebugUtilsObjectNameEXT'\n");
		}
		vkCreateDebugUtilsMessengerEXT = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(m_Instance, "vkCreateDebugUtilsMessengerEXT");
		if (!vkCreateDebugUtilsMessengerEXT)
		{
			LOG_DEBUG_ERROR("Vulkan: Failed to retrive 'vkCreateDebugUtilsMessengerEXT'\n");
		}
		vkDestroyDebugUtilsMessengerEXT = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(m_Instance, "vkDestroyDebugUtilsMessengerEXT");
		if (!vkDestroyDebugUtilsMessengerEXT)
		{
			LOG_DEBUG_ERROR("Vulkan: Failed to retrive 'vkDestroyDebugUtilsMessengerEXT'\n");
		}


		//If not the debugflag is set we do not want to create the messenger
		if (desc.Flags & DEVICE_FLAG_DEBUG)
		{
			//Create debugmessenger
			VkDebugUtilsMessengerCreateInfoEXT info;
			InitDebugMessengerCreateInfo(&info);

			if (vkCreateDebugUtilsMessengerEXT(m_Instance, &info, nullptr, &m_DebugMessenger) != VK_SUCCESS)
			{
				LOG_DEBUG_ERROR("Vulkan: Failed to create debugmessenger, maybe the extension is not present?\n");

				m_DebugMessenger = VK_NULL_HANDLE;
				return;
			}
			else
			{
				LOG_DEBUG_INFO("Vulkan: Created Debugmessenger\n");
			}
		}

        
		//Choose an physical device (GPU) 
		m_PhysicalDevice = QueryPhyscialDevice();
		if (m_PhysicalDevice == VK_NULL_HANDLE)
		{
			LOG_DEBUG_ERROR("Vulkan: Failed to find a suitable GPU\n");
			return;
		}
		else
		{
			vkGetPhysicalDeviceProperties(m_PhysicalDevice, &m_PhysicalDeviceProperties);
			m_FamiliyIndices = FindQueueFamilies(m_PhysicalDevice);

			//Get memory available on the physical device
			VkPhysicalDeviceMemoryProperties memoryProperties = {};
			vkGetPhysicalDeviceMemoryProperties(m_PhysicalDevice, &memoryProperties);

			uint64 hostVRAM		= 0;
			uint64 deviceVRAM	= 0;
			for (uint32 i = 0; i < memoryProperties.memoryHeapCount; i++)
			{
				if (memoryProperties.memoryHeaps[i].flags & VK_MEMORY_HEAP_DEVICE_LOCAL_BIT)
					deviceVRAM	+= memoryProperties.memoryHeaps[i].size;
				else
					hostVRAM	+= memoryProperties.memoryHeaps[i].size;
			}

			hostVRAM = hostVRAM / (1024 * 1024);
			deviceVRAM = deviceVRAM / (1024 * 1024);
			LOG_SYSTEM_PRINT("Vulkan: Selected GPU '%s'\n", m_PhysicalDeviceProperties.deviceName);
			LOG_SYSTEM_PRINT("        Host VRAM:    %llu MB\n", hostVRAM);
			LOG_SYSTEM_PRINT("        Device VRAM:  %llu MB\n", deviceVRAM);
			LOG_SYSTEM_PRINT("        Total VRAM:   %llu MB\n", hostVRAM + deviceVRAM);
		}

		//Find the queuefamily indices for the adapter that we have chosen
		std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
		std::set<int32> uniqueQueueFamilies =
		{
			m_FamiliyIndices.GraphicsFamily,
			m_FamiliyIndices.PresentFamily
		};


		//We set the queues to have the highest priority (0.0f)
		float priority = 0.0f;
		for (int32 queueFamiliy : uniqueQueueFamilies)
		{
			VkDeviceQueueCreateInfo queueInfo = {};
			queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queueInfo.pNext = nullptr;
			queueInfo.flags = 0;
			queueInfo.pQueuePriorities = &priority;
			queueInfo.queueFamilyIndex = queueFamiliy;
			queueInfo.queueCount = 1;

			queueCreateInfos.push_back(queueInfo);
		}

		//Device features we want to enable
		VkPhysicalDeviceFeatures deviceFeatures = {};
		deviceFeatures.samplerAnisotropy = VK_TRUE;

		//Get all the available extensions
		uint32 extensionCount;
		vkEnumerateDeviceExtensionProperties(m_PhysicalDevice, nullptr, &extensionCount, nullptr);

		std::vector<VkExtensionProperties> availableDeviceExtensions(extensionCount);
		vkEnumerateDeviceExtensionProperties(m_PhysicalDevice, nullptr, &extensionCount, availableDeviceExtensions.data());
		
		if (!availableDeviceExtensions.empty())
		{
			LOG_DEBUG_INFO("[Vulkan] Available Device-Extensions (count=%d):\n", availableDeviceExtensions.size());
			for (const auto& extension : availableDeviceExtensions)
			{
				LOG_DEBUG_INFO("   Device-Extension '%s'\n", extension.extensionName);
			}
		}


		//Get the required extension for device
		std::vector<const char*> deviceExtensions = GetRequiredDeviceExtensions();
		if (!deviceExtensions.empty())
		{
			LOG_DEBUG_INFO("[Vulkan] Required Device-Extensions:\n");
			for (const auto& extension : deviceExtensions)
			{
				LOG_DEBUG_INFO("   Device-Extension '%s'\n", extension);
			}
		}


		//Get the optional extensions for the device
		std::vector<const char*> optionalDeviceExtensions = GetOptionalDeviceExtensions();
		if (!optionalDeviceExtensions.empty())
		{
			LOG_DEBUG_INFO("[Vulkan] Optional Device-Extensions:\n");
			for (const auto& extension : optionalDeviceExtensions)
			{
				LOG_DEBUG_INFO("   Device-Extension '%s'\n", extension);
			}
		}


		//Add optional extensions. if we found a optional extension among the available ones we add it
		for (const char* extensionName : optionalDeviceExtensions)
		{
			for (const auto& extension : availableDeviceExtensions)
			{
				if (strcmp(extensionName, extension.extensionName) == 0)
				{
					deviceExtensions.push_back(extensionName);
					break;
				}
			}
		}


		//Create device
		VkDeviceCreateInfo info = {};
		info.sType						= VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		info.pNext						= nullptr;
		info.flags						= 0;
		info.enabledLayerCount			= uint32(requiredLayers.size());
		info.ppEnabledLayerNames		= requiredLayers.data(); //Same as for the instance
		info.enabledExtensionCount		= uint32(deviceExtensions.size());
		info.ppEnabledExtensionNames	= deviceExtensions.data();
		info.pEnabledFeatures			= &deviceFeatures;
		info.queueCreateInfoCount		= uint32(queueCreateInfos.size());
		info.pQueueCreateInfos			= queueCreateInfos.data();

		if (vkCreateDevice(m_PhysicalDevice, &info, nullptr, &m_Device) != VK_SUCCESS)
		{
			LOG_DEBUG_ERROR("Vulkan: Failed to create device\n");

			m_Device = VK_NULL_HANDLE;
			return;
		}
		else
		{
			LOG_SYSTEM_PRINT("Vulkan: Created device and retrived queues\n");
		}


		//Setup properties structure
		m_Properties.Api = GRAPHICS_API_VULKAN;
		strcpy(m_Properties.AdapterString, m_PhysicalDeviceProperties.deviceName);
		if (m_PhysicalDeviceProperties.vendorID == VENDOR_ID_NVIDIA)
			strcpy(m_Properties.VendorString, "NVIDIA");
		else if (m_PhysicalDeviceProperties.vendorID == VENDOR_ID_AMD)
			strcpy(m_Properties.VendorString, "AMD");
		else if (m_PhysicalDeviceProperties.vendorID == VENDOR_ID_INTEL)
			strcpy(m_Properties.VendorString, "INTEL");
		else if (m_PhysicalDeviceProperties.vendorID == VENDOR_ID_ARM)
			strcpy(m_Properties.VendorString, "ARM");
		else if (m_PhysicalDeviceProperties.vendorID == VENDOR_ID_QUALCOMM)
			strcpy(m_Properties.VendorString, "Qualcomm");
		else if (m_PhysicalDeviceProperties.vendorID == VENDOR_ID_IMGTEC)
			strcpy(m_Properties.VendorString, "ImgTec");



		//Setup features structure and get device functions
		if (std::find(deviceExtensions.begin(), deviceExtensions.end(), VK_NV_RAY_TRACING_EXTENSION_NAME) != deviceExtensions.end())
			m_Features.HardwareRayTracing = true;
		
		if (std::find(deviceExtensions.begin(), deviceExtensions.end(), VK_NV_MESH_SHADER_EXTENSION_NAME) != deviceExtensions.end())
			m_Features.MeshShaders = true;
		
		if (std::find(deviceExtensions.begin(), deviceExtensions.end(), VK_NV_SHADING_RATE_IMAGE_EXTENSION_NAME) != deviceExtensions.end())
			m_Features.VariableRateShading = true;
	

		//Get queues
		vkGetDeviceQueue(m_Device, m_FamiliyIndices.GraphicsFamily, 0, &m_GraphicsQueue);
		vkGetDeviceQueue(m_Device, m_FamiliyIndices.PresentFamily, 0, &m_PresentationQueue);
        
		//Create allocator
		m_pDeviceAllocator = DBG_NEW VKNDeviceAllocator(this);
		//Create dynamic memory allocator
		m_pDynamicMemoryAllocator = DBG_NEW VKNDynamicMemoryAllocator(this);
		//Create SafeReleaseManager
		m_pSafeReleaseManager = DBG_NEW VKNSafeReleaseManager(this);
		//Create framebuffercache
		m_pFramebufferCache = DBG_NEW VKNFramebufferCache(this);
		//Create renderpasscache
		m_pRenderPassCache = DBG_NEW VKNRenderPassCache(this);        
      
        //Init internal commandlist, used for copying from staging buffers to final resource etc.
        m_pImmediateContext = DBG_NEW VKNDeviceContext(this, DEVICE_CONTEXT_TYPE_IMMEDIATE);
        m_pImmediateContext->SetName("Graphics Device ImmediateContext");

		//Create default resources
		{
			BufferDesc vertexBufferDesc = {};
			vertexBufferDesc.pName			= "Default VertexBuffer";
			vertexBufferDesc.Flags			= BUFFER_FLAGS_VERTEX_BUFFER;
			vertexBufferDesc.Usage			= USAGE_DEFAULT;
			vertexBufferDesc.SizeInBytes	= sizeof(float) * 9;
			vertexBufferDesc.StrideInBytes	= sizeof(float) * 3;

			float vertices[9] =
			{
				0.0f, 0.5f, 0.0f,
				0.5f, -0.5f, 0.0f,
				-0.5f, -0.5f, 0.0f,
			};

			ResourceData vertexData = {};
			vertexData.pData		= vertices;
			vertexData.SizeInBytes	= sizeof(vertices);
			m_pDefaultVertexBuffer = DBG_NEW VKNBuffer(this, &vertexData, vertexBufferDesc);
		}


		{
			BufferDesc indexBufferDesc = {};
			indexBufferDesc.pName			= "Default IndexBuffer";
			indexBufferDesc.Flags			= BUFFER_FLAGS_INDEX_BUFFER;
			indexBufferDesc.Usage			= USAGE_DEFAULT;
			indexBufferDesc.SizeInBytes		= sizeof(uint32) * 3;
			indexBufferDesc.StrideInBytes	= sizeof(uint32);

			uint32 indices[3] =
			{
				0, 1, 2,
			};

			ResourceData indexData = {};
			indexData.pData			= indices;
			indexData.SizeInBytes	= sizeof(indices);
			m_pDefaultIndexBuffer = DBG_NEW VKNBuffer(this, &indexData, indexBufferDesc);
		}


		{
			BufferDesc constantBufferDesc = {};
			constantBufferDesc.pName			= "Default ConstantBuffer";
			constantBufferDesc.Flags			= BUFFER_FLAGS_CONSTANT_BUFFER;
			constantBufferDesc.Usage			= USAGE_DEFAULT;
			constantBufferDesc.SizeInBytes		= sizeof(float) * 4;
			constantBufferDesc.StrideInBytes	= sizeof(float);

			float data[4] =
			{
				1.0f, 1.0f, 1.0f, 1.0f
			};

			ResourceData constantData = {};
			constantData.pData			= data;
			constantData.SizeInBytes	= sizeof(data);
			m_pDefaultConstantBuffer = DBG_NEW VKNBuffer(this, &constantData, constantBufferDesc);
		}


		{
			SamplerStateDesc samplerStateDesc = {};
			samplerStateDesc.pName		= "Default SamplerState";
			samplerStateDesc.AdressMode	= SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
			samplerStateDesc.Anisotropy	= 1.0f;
			samplerStateDesc.MinMipLOD	= 0.0f;
			samplerStateDesc.MaxMipLOD	= FLT_MAX;
			samplerStateDesc.MipLODBias	= 0.0f;
			m_pDefaultSamplerState = DBG_NEW VKNSamplerState(this, samplerStateDesc);
		}


		{
			TextureDesc textureDesc = {};
			textureDesc.pName		= "Default Texture";
			textureDesc.Type		= TEXTURE_TYPE_2D;
			textureDesc.Flags		= TEXTURE_FLAGS_SHADER_RESOURCE;
			textureDesc.Width		= 4;
			textureDesc.Height		= 4;
			textureDesc.Depth		= 1;
			textureDesc.ArraySize	= 1;
			textureDesc.MipLevels	= 1;
			textureDesc.SampleCount = 1;
			textureDesc.Usage		= USAGE_DEFAULT;
			textureDesc.Format		= FORMAT_R8G8B8A8_UNORM;

			uint8 pixels[4 * 4 * 4];
			memset(pixels, 255, sizeof(pixels));

			ResourceData textureData = {};
			textureData.pData		= pixels;
			textureData.SizeInBytes = sizeof(pixels);
			m_pDefaultTexture = DBG_NEW VKNTexture(this, &textureData, textureDesc);

			TextureTransitionBarrier barrier = {};
			barrier.pTexture	= m_pDefaultTexture;
			barrier.AfterState	= RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
			barrier.MipLevel	= LAMBDA_ALL_MIP_LEVELS;
			m_pImmediateContext->TransitionTextureStates(&barrier, 1);
		}
    }
    
    
    void VKNDevice::CreateDefferedContext(IDeviceContext** ppDefferedContext)
    {
		LAMBDA_ASSERT(ppDefferedContext != nullptr);
        (*ppDefferedContext) = DBG_NEW VKNDeviceContext(this, DEVICE_CONTEXT_TYPE_DEFFERED);
    }
    
    
    void VKNDevice::CreateBuffer(IBuffer** ppBuffer, const ResourceData* pInitalData, const BufferDesc& desc)
    {
		LAMBDA_ASSERT(ppBuffer != nullptr);
		(*ppBuffer) = DBG_NEW VKNBuffer(this, pInitalData, desc);
    }
    
    
    void VKNDevice::CreateTexture(ITexture** ppTexture, const ResourceData* pInitalData, const TextureDesc& desc)
    {
		LAMBDA_ASSERT(ppTexture != nullptr);
		(*ppTexture) = DBG_NEW VKNTexture(this, pInitalData, desc);
    }
    
    
    void VKNDevice::CreateShader(IShader** ppShader, const ShaderDesc& desc)
    {
		LAMBDA_ASSERT(ppShader != nullptr);
        (*ppShader) = DBG_NEW VKNShader(this, desc);
    }
    
    
    void VKNDevice::CreateSamplerState(ISamplerState** ppSamplerState, const SamplerStateDesc& desc)
    {
		LAMBDA_ASSERT(ppSamplerState != nullptr);
        (*ppSamplerState) = DBG_NEW VKNSamplerState(this, desc);
    }
    
    
    void VKNDevice::CreatePipelineState(IPipelineState** ppPipelineState, const PipelineStateDesc& desc)
    {
		LAMBDA_ASSERT(ppPipelineState != nullptr);
        (*ppPipelineState) = DBG_NEW VKNPipelineState(this, desc);
    }

    
    void VKNDevice::CreateQuery(Lambda::IQuery** ppQuery, const QueryDesc& desc)
    {
        LAMBDA_ASSERT(ppQuery != nullptr);
        (*ppQuery) = DBG_NEW VKNQuery(this, desc);
    }
    

    IDeviceContext* VKNDevice::GetImmediateContext() const
    {
        m_pImmediateContext->AddRef();
        return m_pImmediateContext;
    }
    

    void VKNDevice::ExecuteCommandBuffer(VkSubmitInfo* pInfo, uint32 numBuffers, VkFence fence) const
    {
        if (vkQueueSubmit(m_GraphicsQueue, numBuffers, pInfo, fence) != VK_SUCCESS)
        {
            LOG_DEBUG_ERROR("Vulkan: Failed to submit CommandBuffers\n");
        }
		else
		{
			//Finish device frame
			FinishFrame();
		}
    }

	
	VkResult VKNDevice::Present(VkPresentInfoKHR* pInfo)
	{
		return vkQueuePresentKHR(m_PresentationQueue, pInfo);
	}


	VKNDeviceContext* VKNDevice::GetVKNImmediateContext() const
	{
		m_pImmediateContext->AddRef();
		return m_pImmediateContext;
	}
    
    
    void VKNDevice::FinishFrame() const
    {
        //Cleanup memory
		m_pDeviceAllocator->EmptyGarbageMemory();
		m_pDynamicMemoryAllocator->EmptyGarbageMemory();
		m_pSafeReleaseManager->EmptyResources();
    }
    
    
    void VKNDevice::WaitUntilIdle() const
    {
        LOG_DEBUG_INFO("VKNDevice::WaitUntilIdle\n");
        vkDeviceWaitIdle(m_Device);
    }


	bool VKNDevice::AllocateImage(VKNAllocation& allocation, VkImage image, Usage usage)
	{
		VkMemoryRequirements memoryRequirements = {};
		vkGetImageMemoryRequirements(m_Device, image, &memoryRequirements);
		if (m_pDeviceAllocator->Allocate(allocation, memoryRequirements, usage))
		{
			vkBindImageMemory(m_Device, image, allocation.DeviceMemory, allocation.DeviceMemoryOffset);
			return true;
		}
		else
		{
			LOG_DEBUG_ERROR("Vulkan: Failed to allocate memory for texture\n");
			return false;
		}
	}


	bool VKNDevice::AllocateBuffer(VKNAllocation& allocation, VkBuffer buffer, Usage usage)
	{
		VkMemoryRequirements memoryRequirements = {};
		vkGetBufferMemoryRequirements(m_Device, buffer, &memoryRequirements);
		if (m_pDeviceAllocator->Allocate(allocation, memoryRequirements, usage))
		{
			vkBindBufferMemory(m_Device, buffer, allocation.DeviceMemory, allocation.DeviceMemoryOffset);
			return true;
		}
		else
		{
			LOG_DEBUG_ERROR("Vulkan: Failed to allocate memory for buffer\n");
			return false;
		}
	}


	VkSampleCountFlagBits VKNDevice::GetHighestSampleCount() const
	{
		//Get max MSAA we can use on the device
		VkSampleCountFlags sampleCountFlags = std::min(m_PhysicalDeviceProperties.limits.framebufferStencilSampleCounts,
			std::min(m_PhysicalDeviceProperties.limits.framebufferColorSampleCounts, m_PhysicalDeviceProperties.limits.framebufferDepthSampleCounts));
		VkSampleCountFlagBits sampleCount = VK_SAMPLE_COUNT_1_BIT;

		if (sampleCountFlags & VK_SAMPLE_COUNT_64_BIT)		{ sampleCount = VK_SAMPLE_COUNT_64_BIT; }
		else if (sampleCountFlags & VK_SAMPLE_COUNT_32_BIT) { sampleCount = VK_SAMPLE_COUNT_32_BIT; }
		else if (sampleCountFlags & VK_SAMPLE_COUNT_16_BIT) { sampleCount = VK_SAMPLE_COUNT_16_BIT; }
		else if (sampleCountFlags & VK_SAMPLE_COUNT_8_BIT)	{ sampleCount = VK_SAMPLE_COUNT_8_BIT; }
		else if (sampleCountFlags & VK_SAMPLE_COUNT_4_BIT)	{ sampleCount = VK_SAMPLE_COUNT_4_BIT; }
		else if (sampleCountFlags & VK_SAMPLE_COUNT_2_BIT)	{ sampleCount = VK_SAMPLE_COUNT_2_BIT; }

		return sampleCount;
	}


	void VKNDevice::SetVulkanObjectName(VkObjectType type, uint64 objectHandle, const std::string& name)
	{
		if (vkSetDebugUtilsObjectNameEXT)
		{
			//Set name on object
			VkDebugUtilsObjectNameInfoEXT info = {};
			info.sType          = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
			info.pNext          = nullptr;
			info.objectType     = type;
			info.pObjectName    = name.c_str();
			info.objectHandle   = objectHandle;

			if (vkSetDebugUtilsObjectNameEXT(m_Device, &info) != VK_SUCCESS)
			{
				LOG_DEBUG_ERROR("Vulkan: Failed to set name '%s'\n", info.pObjectName);
			}
		}
	}


	void VKNDevice::InitDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo)
	{
		pCreateInfo->sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		pCreateInfo->pNext = nullptr;
		pCreateInfo->flags = 0;
		pCreateInfo->messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		pCreateInfo->messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		pCreateInfo->pfnUserCallback = VKNDevice::VulkanDebugCallback;
		pCreateInfo->pUserData = nullptr;
	}


	VkPhysicalDevice VKNDevice::QueryPhyscialDevice()
	{
		//Enumerate all adapters
		uint32 adapterCount = 0;
		vkEnumeratePhysicalDevices(m_Instance, &adapterCount, nullptr);
		if (adapterCount == 0)
		{
			LOG_DEBUG_ERROR("Vulkan: No vulkan-supported GPUs found\n");
			return VK_NULL_HANDLE;
		}

		std::vector<VkPhysicalDevice> adapters(adapterCount);
		vkEnumeratePhysicalDevices(m_Instance, &adapterCount, adapters.data());

#if defined(LAMBDA_DEBUG)
		LOG_DEBUG_INFO("Vulkan: Following of physcial devices found:\n");
		for (auto& dev : adapters)
		{
			VkPhysicalDeviceProperties properties = {};
			vkGetPhysicalDeviceProperties(dev, &properties);
			LOG_DEBUG_INFO("    %s\n", properties.deviceName);
		}
#endif

		//Find a suitable graphics card
		for (const auto& adapter : adapters)
		{
			if (PhysicalDeviceIsSuitable(adapter))
			{
				return adapter;
			}
		}

		return VK_NULL_HANDLE;
	}


	bool VKNDevice::PhysicalDeviceIsSuitable(VkPhysicalDevice adapter)
	{
		VkPhysicalDeviceProperties adapterProperties;
		vkGetPhysicalDeviceProperties(adapter, &adapterProperties);

		VkPhysicalDeviceFeatures adapterFeatures;
		vkGetPhysicalDeviceFeatures(adapter, &adapterFeatures);

		//Check for adapter features
		if (!adapterFeatures.samplerAnisotropy)
		{
			LOG_DEBUG_ERROR("Vulkan: Anisotropic filtering is not supported by adapter\n");
			return false;
		}

		//Find indices for queuefamilies
		QueueFamilyIndices indices = FindQueueFamilies(adapter);
		if (!indices.Valid())
		{
			LOG_DEBUG_ERROR("Vulkan: Failed to find a suitable queuefamilies\n");
			return false;
		}
		else
		{
			LOG_DEBUG_INFO("Vulkan: Using queueFamily-Index '%d' for graphics and '%d' for presentation\n", indices.GraphicsFamily, indices.PresentFamily);
		}

		//Check if required extension for device is supported
		std::vector<const char*> deviceExtensions = GetRequiredDeviceExtensions();

		uint32 extensionCount;
		vkEnumerateDeviceExtensionProperties(adapter, nullptr, &extensionCount, nullptr);

		std::vector<VkExtensionProperties> availableExtensions(extensionCount);
		vkEnumerateDeviceExtensionProperties(adapter, nullptr, &extensionCount, availableExtensions.data());

		for (const auto& extensionName : deviceExtensions)
		{
			bool extensionFound = false;
			for (const auto& extension : availableExtensions)
			{
				if (strcmp(extension.extensionName, extensionName) == 0)
				{
					extensionFound = true;
					break;
				}
			}

			if (!extensionFound)
			{
				LOG_DEBUG_ERROR("Vulkan: Adapter '%s' does not support the device extension '%s'\n", adapterProperties.deviceName, extensionName);
				return false;
			}
		}

		return true;
	}


	std::vector<const char*> VKNDevice::GetRequiredValidationLayers(bool debug)
	{
		std::vector<const char*> requiredLayers;
		if (debug)
		{
			requiredLayers.push_back("VK_LAYER_KHRONOS_validation");
		}

		return requiredLayers;
	}

	
	std::vector<const char*> VKNDevice::GetOptionalValidationLayers(bool debug)
	{
		std::vector<const char*> optionalLayers;
		if (debug)
		{
			//optionalLayers.push_back("VK_LAYER_RENDERDOC_Capture");
		}

		return optionalLayers;
	}


	std::vector<const char*> VKNDevice::GetRequiredDeviceExtensions()
	{
		std::vector<const char*> requiredExtensions;
		requiredExtensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
		requiredExtensions.push_back(VK_KHR_MAINTENANCE1_EXTENSION_NAME);

		return requiredExtensions;
	}


	std::vector<const char*> VKNDevice::GetOptionalDeviceExtensions()
	{
		std::vector<const char*> optionalExtensions;
		optionalExtensions.push_back(VK_NV_RAY_TRACING_EXTENSION_NAME);
		optionalExtensions.push_back(VK_NV_MESH_SHADER_EXTENSION_NAME);
		optionalExtensions.push_back(VK_NV_SHADING_RATE_IMAGE_EXTENSION_NAME);
		optionalExtensions.push_back(VK_EXT_FULL_SCREEN_EXCLUSIVE_EXTENSION_NAME);
		return optionalExtensions;
	}


	std::vector<const char*> VKNDevice::GetRequiredInstanceExtensions(bool debug)
	{
		std::vector<const char*> requiredExtensions;
#if defined(LAMBDA_PLAT_MACOS)
		//Get extensions on macOS
		uint32 extensionCount = 0;
		const char** ppExtensions = glfwGetRequiredInstanceExtensions(&extensionCount);
		requiredExtensions = std::vector<const char*>(ppExtensions, ppExtensions + extensionCount);
#elif defined(LAMBDA_PLAT_WINDOWS)
		//Get extensions on Windows
		requiredExtensions.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
		requiredExtensions.push_back("VK_KHR_win32_surface");
#endif

		//Add debug extensions to the required ones
		if (debug)
		{
			requiredExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
			requiredExtensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
		}

		return requiredExtensions;
	}

	
	std::vector<const char*> VKNDevice::GetOptionalInstanceExtensions(bool debug)
	{
		std::vector<const char*> optionalExtensions;
		optionalExtensions.push_back(VK_KHR_GET_SURFACE_CAPABILITIES_2_EXTENSION_NAME);
		return optionalExtensions;
	}


	VKAPI_ATTR VkBool32 VKAPI_CALL VKNDevice::VulkanDebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData)
	{
		//Get severity
		LogSeverity severity = LOG_SEVERITY_UNKNOWN;
		switch (messageSeverity)
		{
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:    severity = LOG_SEVERITY_INFO;    break;
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT: severity = LOG_SEVERITY_WARNING; break;
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:   severity = LOG_SEVERITY_ERROR;   break;
		default: return VK_FALSE;
		}

		//Get type of message
		const char* pTypeStr = nullptr;
		if (messageType == VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT)
			pTypeStr = "GENERAL";
		else if (messageType == VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT)
			pTypeStr = "VALIDATION";
		else if (messageType == VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT)
			pTypeStr = "PERFORMANCE";

		//Get objectinfo
		uint64 object = (pCallbackData->objectCount != 0) ? pCallbackData->pObjects->objectHandle : 0;
		const char* pObjectName = (pCallbackData->objectCount != 0) ? pCallbackData->pObjects->pObjectName : "";

		//Log
		Log::GetDebugLog().Print(severity, "[Vulkan Validation Layer - type=%s, object=0x%llx, objectname=%s] %s\n", pTypeStr, object, pObjectName, pCallbackData->pMessage);
		return VK_FALSE;
	}

    
    void* VKNDevice::GetNativeHandle() const
    {
        return reinterpret_cast<void*>(m_Device);
    }
}
