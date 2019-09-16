#include "LambdaPch.h"
#include "VulkanGraphicsDevice.h"
#include "VulkanShader.h"
#include "VulkanPipelineState.h"
#include "VulkanTexture.h"
#include "VulkanSamplerState.h"
#include "VulkanCommandList.h"
#include "VulkanFramebuffer.h"
#include "VulkanBuffer.h"
#include "VulkanSwapChain.h"
#include "VulkanRenderPass.h"
#include "VulkanPipelineResourceState.h"
#include "VulkanUploadBuffer.h"
#include "VulkanUtilities.h"
#include "VulkanConversions.inl"
#if defined(LAMBDA_PLAT_MACOS)
    #include <GLFW/glfw3.h>
#elif defined(LAMBDA_PLAT_WINDOWS)
	#if !defined(WIN32_LEAN_AND_MEAN)
		#define WIN32_LEAN_AND_MEAN 1
	#endif
	#include <Windows.h>
#endif

namespace Lambda
{   
	//--------------------
	//VulkanGraphicsDevice
	//--------------------

    PFN_vkSetDebugUtilsObjectNameEXT VulkanGraphicsDevice::SetDebugUtilsObjectNameEXT		= nullptr;
    PFN_vkCreateDebugUtilsMessengerEXT VulkanGraphicsDevice::CreateDebugUtilsMessengerEXT	= nullptr;
    PFN_vkDestroyDebugUtilsMessengerEXT VulkanGraphicsDevice::DestroyDebugUtilsMessengerEXT = nullptr;


    VulkanGraphicsDevice::VulkanGraphicsDevice(const GraphicsDeviceDesc& desc)
        : m_Instance(VK_NULL_HANDLE),
        m_DebugMessenger(VK_NULL_HANDLE),
        m_Device(VK_NULL_HANDLE),
        m_GraphicsQueue(VK_NULL_HANDLE),
        m_PresentationQueue(VK_NULL_HANDLE),
		m_Fences(),
        m_RenderSemaphores(),
		m_ImageSemaphores(),
		m_DeviceSettings(),
        m_FamiliyIndices(),
        m_PhysicalDevice(VK_NULL_HANDLE),
        m_PhysicalDeviceProperties(),
        m_Surface(VK_NULL_HANDLE),
		m_pDynamicBufferManager(nullptr),
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
    
    
    VulkanGraphicsDevice::~VulkanGraphicsDevice()
    {
        ICommandList* pDirectCommandList = m_pCommandList;
        DestroyCommandList(&pDirectCommandList);
               
        for (uint32 i = 0; i < m_Desc.BackBufferCount; i++)
        {
			if (m_ImageSemaphores[i] != VK_NULL_HANDLE)
			{
				vkDestroySemaphore(m_Device, m_ImageSemaphores[i], nullptr);
				m_ImageSemaphores[i] = VK_NULL_HANDLE;
			}
            if (m_RenderSemaphores[i] != VK_NULL_HANDLE)
            {
                vkDestroySemaphore(m_Device, m_RenderSemaphores[i], nullptr);
                m_RenderSemaphores[i] = VK_NULL_HANDLE;
            }
            if (m_Fences[i] != VK_NULL_HANDLE)
            {
                vkDestroyFence(m_Device, m_Fences[i], nullptr);
                m_Fences[i] = VK_NULL_HANDLE;
            }
        }        


		//Release all framebuffers
		if (m_pFramebufferCache)
		{
			m_pFramebufferCache->ReleaseAll(m_Device);
		}


		//Destroy window framebuffer
		m_pSwapChain->Destroy(m_Device);
        ReleaseDepthStencil();
		ReleaseMSAABuffer();


		SafeDelete(m_pFramebufferCache);
		SafeDelete(m_pDynamicBufferManager);


		//Release all memory
		if (m_pDeviceAllocator)
		{
			m_pDeviceAllocator->Destroy(m_Device);
			m_pDeviceAllocator = nullptr;
		}


        if (m_Device != VK_NULL_HANDLE)
        {
            vkDestroyDevice(m_Device, nullptr);
            m_Device = VK_NULL_HANDLE;
        }
        if (m_DebugMessenger != VK_NULL_HANDLE)
        {
            DestroyDebugUtilsMessengerEXT(m_Instance, m_DebugMessenger, nullptr);
            m_DebugMessenger = VK_NULL_HANDLE;
        }
        if (m_Surface != VK_NULL_HANDLE)
        {
            vkDestroySurfaceKHR(m_Instance, m_Surface, nullptr);
            m_Surface = VK_NULL_HANDLE;
        }
        if (m_Instance != VK_NULL_HANDLE)
        {
            vkDestroyInstance(m_Instance, nullptr);
            m_Instance = VK_NULL_HANDLE;
        }


		//Set global instance to nullptr
		if (s_pInstance == this)
			s_pInstance = nullptr;
    }
    
    
    void VulkanGraphicsDevice::Init(const GraphicsDeviceDesc& desc)
    {
		LAMBDA_ASSERT(desc.pWindow != nullptr);
        
		VkApplicationInfo applicationInfo = {};
		applicationInfo.sType				= VK_STRUCTURE_TYPE_APPLICATION_INFO;
		applicationInfo.pNext				= nullptr;
		applicationInfo.pApplicationName	= "Lambda Engine";
		applicationInfo.applicationVersion	= VK_MAKE_VERSION(1, 0, 0);
		applicationInfo.pEngineName			= "Lambda Engine";
		applicationInfo.engineVersion		= VK_MAKE_VERSION(1, 0, 0);
		applicationInfo.apiVersion			= VK_API_VERSION_1_0;


		//Get all the available extensions
		uint32 availableExtensionsCount = 0;
		vkEnumerateInstanceExtensionProperties(nullptr, &availableExtensionsCount, nullptr);

		std::vector<VkExtensionProperties> availableExtensions(availableExtensionsCount);
		vkEnumerateInstanceExtensionProperties(nullptr, &availableExtensionsCount, availableExtensions.data());

		if (availableExtensions.size() > 0)
		{
			LOG_DEBUG_INFO("[Vulkan] Available Instance-Extensions:\n");
			for (const auto& extension : availableExtensions)
			{
				const char* name = extension.extensionName;
				LOG_DEBUG_INFO("   Instance-Extension '%s'\n", name);
			}
		}
		else
		{
			LOG_DEBUG_ERROR("Vulkan: No available Instance-Extensions\n");
		}


		//Get all available layers
		uint32_t layerCount = 0;
		vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

		std::vector<VkLayerProperties> availableLayers(layerCount);
		vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

		if (availableLayers.size() > 0)
		{
			LOG_DEBUG_INFO("[Vulkan] Available Instance-Layers:\n");
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


		//Get extensions
		std::vector<const char*> requiredExtensions = GetRequiredInstanceExtensions(desc.Flags & GRAPHICS_CONTEXT_FLAG_DEBUG);
		if (requiredExtensions.size() > 0)
		{
			LOG_DEBUG_INFO("[Vulkan] Required Instance-Extensions:\n");
			for (const auto& extension : requiredExtensions)
				LOG_DEBUG_INFO("   Instance-Extension '%s'\n", extension);
		}


		//Get layers
		std::vector<const char*> requiredLayers = GetRequiredValidationLayers(desc.Flags & GRAPHICS_CONTEXT_FLAG_DEBUG);
		if (requiredLayers.size() > 0)
		{
			LOG_DEBUG_INFO("[Vulkan] Required Instance-Layers:\n");
			for (const auto& layer : requiredLayers)
				LOG_DEBUG_INFO("   Instance-Layer '%s'\n", layer);
		}


		//Check if all the required layers are available
		for (const char* layerName : requiredLayers)
		{
			bool layerFound = false;
			for (const auto& layer : availableLayers)
			{
				if (strcmp(layerName, layer.layerName) == 0)
				{
					layerFound = true;
					break;
				}
			}

			if (!layerFound)
			{
				LOG_DEBUG_ERROR("Vulkan: Required Instance-Validationlayer '%s' was not found on the system\n", layerName);
				return;
			}
		}


		//Check if all the required extensions are available
		for (const char* extensionName : requiredExtensions)
		{
			bool extensionFound = false;
			for (const auto& extension : availableExtensions)
			{
				if (strcmp(extensionName, extension.extensionName) == 0)
				{
					extensionFound = true;
					break;
				}
			}

			if (!extensionFound)
			{
				LOG_DEBUG_ERROR("Vulkan: Required Instance-Extension '%s' was not found on the system\n", extensionName);
				return;
			}
		}


		VkDebugUtilsMessengerCreateInfoEXT dInfo;
		InitDebugMessengerCreateInfo(&dInfo);

		//Create instance
		VkInstanceCreateInfo instanceInfo = {};
		instanceInfo.sType						= VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		instanceInfo.pNext						= (desc.Flags & GRAPHICS_CONTEXT_FLAG_DEBUG) ? (VkDebugUtilsMessengerCreateInfoEXT*)& dInfo : nullptr;
		instanceInfo.flags						= 0;
		instanceInfo.pApplicationInfo			= &applicationInfo;
		instanceInfo.enabledExtensionCount		= uint32(requiredExtensions.size());
		instanceInfo.ppEnabledExtensionNames	= requiredExtensions.data();
		instanceInfo.enabledLayerCount			= uint32(requiredLayers.size());
		instanceInfo.ppEnabledLayerNames		= requiredLayers.data();

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

			//Get instance functions
			SetDebugUtilsObjectNameEXT = (PFN_vkSetDebugUtilsObjectNameEXT)vkGetInstanceProcAddr(m_Instance, "vkSetDebugUtilsObjectNameEXT");
			if (!SetDebugUtilsObjectNameEXT)
			{
				LOG_DEBUG_ERROR("Vulkan: Failed to retrive 'vkSetDebugUtilsObjectNameEXT'\n");
			}
			CreateDebugUtilsMessengerEXT = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(m_Instance, "vkCreateDebugUtilsMessengerEXT");
			if (!CreateDebugUtilsMessengerEXT)
			{
				LOG_DEBUG_ERROR("Vulkan: Failed to retrive 'vkCreateDebugUtilsMessengerEXT'\n");
			}
			DestroyDebugUtilsMessengerEXT = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(m_Instance, "vkDestroyDebugUtilsMessengerEXT");
			if (!DestroyDebugUtilsMessengerEXT)
			{
				LOG_DEBUG_ERROR("Vulkan: Failed to retrive 'vkDestroyDebugUtilsMessengerEXT'\n");
			}
		}


		//If not the debugflag is set we do not want to create the messenger
		if (desc.Flags & GRAPHICS_CONTEXT_FLAG_DEBUG)
		{
			//Create debugmessenger
			VkDebugUtilsMessengerCreateInfoEXT info;
			InitDebugMessengerCreateInfo(&info);

			if (CreateDebugUtilsMessengerEXT(m_Instance, &info, nullptr, &m_DebugMessenger) != VK_SUCCESS)
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


        //Create window surface
		m_Surface = CreateSurface(desc.pWindow);
		if (m_Surface == VK_NULL_HANDLE)
		{
			LOG_DEBUG_ERROR("Vulkan: Failed to create surface\n");
			return;
		}
		else
		{
			LOG_SYSTEM_PRINT("Vulkan: Created surface\n");
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
			m_FamiliyIndices = FindQueueFamilies(m_PhysicalDevice, m_Surface);

			VkPhysicalDeviceMemoryProperties memoryProperties = {};
			vkGetPhysicalDeviceMemoryProperties(m_PhysicalDevice, &memoryProperties);

			uint64 vram = 0;
			for (uint32 i = 0; i < memoryProperties.memoryHeapCount; i++)
			{
				if (memoryProperties.memoryHeaps[i].flags & VK_MEMORY_HEAP_DEVICE_LOCAL_BIT)
					vram += memoryProperties.memoryHeaps[i].size;
			}

			vram = vram / (1024 * 1024);
			LOG_SYSTEM_PRINT("Vulkan: Selected GPU '%s'\n        VRAM: %llu MB\n", m_PhysicalDeviceProperties.deviceName, vram);
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
			queueInfo.sType				= VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queueInfo.pNext				= nullptr;
			queueInfo.flags				= 0;
			queueInfo.pQueuePriorities	= &priority;
			queueInfo.queueFamilyIndex	= queueFamiliy;
			queueInfo.queueCount		= 1;

			queueCreateInfos.push_back(queueInfo);
		}

		//Device features we want to enable
		VkPhysicalDeviceFeatures deviceFeatures = {};
		deviceFeatures.samplerAnisotropy = VK_TRUE;

		//Get the required extension for device
		std::vector<const char*> deviceExtensions = GetRequiredDeviceExtensions();
		if (deviceExtensions.size() > 1)
		{
			LOG_DEBUG_INFO("[Vulkan] Required Device-Extensions:\n");
			for (const auto& extension : deviceExtensions)
			{
				LOG_DEBUG_INFO("   Device-Extension '%s'\n", extension);
			}
		}


		//Get all the available extensions
		uint32 extensionCount;
		vkEnumerateDeviceExtensionProperties(m_PhysicalDevice, nullptr, &extensionCount, nullptr);

		std::vector<VkExtensionProperties> availableDeviceExtensions(extensionCount);
		vkEnumerateDeviceExtensionProperties(m_PhysicalDevice, nullptr, &extensionCount, availableDeviceExtensions.data());

		LOG_DEBUG_INFO("[Vulkan] Available Device-Extensions:\n");
		for (const auto& extension : availableDeviceExtensions)
		{
			LOG_DEBUG_INFO("   Device-Extension '%s'\n", extension.extensionName);
		}


		//Create device
		VkDeviceCreateInfo info = {};
		info.sType						= VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		info.pNext						= nullptr;
		info.flags						= 0;
		info.enabledExtensionCount		= uint32(deviceExtensions.size());
		info.ppEnabledExtensionNames	= deviceExtensions.data();
		info.enabledLayerCount			= uint32(requiredLayers.size());
		info.ppEnabledLayerNames		= requiredLayers.data(); //Same as for the instance
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
            m_Desc = desc;
		}


		//Get queues
		vkGetDeviceQueue(m_Device, m_FamiliyIndices.GraphicsFamily, 0, &m_GraphicsQueue);
		vkGetDeviceQueue(m_Device, m_FamiliyIndices.PresentFamily, 0, &m_PresentationQueue);


		//Get max MSAA we can use on the device
		VkSampleCountFlags		sampleCountFlags = std::min(m_PhysicalDeviceProperties.limits.framebufferStencilSampleCounts, 
			std::min(m_PhysicalDeviceProperties.limits.framebufferColorSampleCounts, m_PhysicalDeviceProperties.limits.framebufferDepthSampleCounts));
		VkSampleCountFlagBits	sampleCount;

		if		(sampleCountFlags & VK_SAMPLE_COUNT_64_BIT)	{ sampleCount = VK_SAMPLE_COUNT_64_BIT; }
		else if (sampleCountFlags & VK_SAMPLE_COUNT_32_BIT) { sampleCount = VK_SAMPLE_COUNT_32_BIT; }
		else if (sampleCountFlags & VK_SAMPLE_COUNT_16_BIT) { sampleCount = VK_SAMPLE_COUNT_16_BIT; }
		else if (sampleCountFlags & VK_SAMPLE_COUNT_8_BIT)	{ sampleCount = VK_SAMPLE_COUNT_8_BIT; }
		else if (sampleCountFlags & VK_SAMPLE_COUNT_4_BIT)	{ sampleCount = VK_SAMPLE_COUNT_4_BIT; }
		else if (sampleCountFlags & VK_SAMPLE_COUNT_2_BIT)	{ sampleCount = VK_SAMPLE_COUNT_2_BIT; }

		//Set devicesettings
		m_DeviceSettings.FramesAhead = desc.BackBufferCount;
		m_DeviceSettings.SampleCount = ConvertSampleCount(desc.SampleCount);
		if (m_DeviceSettings.SampleCount > sampleCount)
		{
			m_DeviceSettings.SampleCount = sampleCount;
			LOG_DEBUG_WARNING("Vulkan: SampleCount (= %u) is higher  than the device's maximum of %u. SampleCount was set to the device's maximum.\n", uint32(m_DeviceSettings.SampleCount), uint32(sampleCount));
		}


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
            if (vkCreateSemaphore(m_Device, &semaphoreInfo, nullptr, &m_ImageSemaphores[i]) != VK_SUCCESS ||
                vkCreateSemaphore(m_Device, &semaphoreInfo, nullptr, &m_RenderSemaphores[i]) != VK_SUCCESS)
            {
                LOG_DEBUG_ERROR("Vulkan: Failed to create Semaphore\n");
                return;
            }
            else
            {
                VulkanGraphicsDevice::SetVulkanObjectName(VK_OBJECT_TYPE_SEMAPHORE, (uint64)m_ImageSemaphores[i], "ImageSemaphore[" +  std::to_string(i) + "]");
                VulkanGraphicsDevice::SetVulkanObjectName(VK_OBJECT_TYPE_SEMAPHORE, (uint64)m_RenderSemaphores[i], "RenderSemaphore[" + std::to_string(i) + "]");
            }
            
            //Create fence
            if (vkCreateFence(m_Device, &fenceInfo, nullptr, &m_Fences[i]) != VK_SUCCESS)
            {
                LOG_DEBUG_ERROR("Vulkan: Failed to create fence\n");
                return;
            }
            else
            {
                VulkanGraphicsDevice::SetVulkanObjectName(VK_OBJECT_TYPE_FENCE, (uint64)m_Fences[i], "Fence[" +  std::to_string(i) + "]");
            }
        }
        
        LOG_DEBUG_INFO("Vulkan: Created Semaphores and fences\n");
        

		//Create allocator
		m_pDeviceAllocator = DBG_NEW VulkanDeviceAllocator();


		//Create dynamic buffer manager
		m_pDynamicBufferManager = DBG_NEW VulkanDynamicBufferManager();


		//Create framebuffercache
		m_pFramebufferCache = DBG_NEW VulkanFramebufferCache();

        
        //Create swapchain
        VulkanSwapChainDesc swapChainInfo = {};
        swapChainInfo.Adapter            = m_PhysicalDevice;
        swapChainInfo.Surface            = m_Surface;
        swapChainInfo.SignalSemaphore    = m_ImageSemaphores[m_CurrentFrame];
        swapChainInfo.PresentationMode   = VK_PRESENT_MODE_MAILBOX_KHR;
        swapChainInfo.Format.format      = VK_FORMAT_B8G8R8A8_UNORM;
        swapChainInfo.Format.colorSpace  = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
        swapChainInfo.Extent             = { desc.pWindow->GetWidth(), desc.pWindow->GetHeight() };
        swapChainInfo.ImageCount         = desc.BackBufferCount;
        m_pSwapChain = DBG_NEW VulkanSwapChain(m_Device, swapChainInfo);


		//If we are using MSAA we need to create a seperate texture
		if (m_DeviceSettings.SampleCount > VK_SAMPLE_COUNT_1_BIT)
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
			m_pCommandList = reinterpret_cast<VulkanCommandList*>(pList);
			m_pCommandList->SetName("Graphics Device Internal CommandList");
		}
    }
    
    
    void VulkanGraphicsDevice::InitDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo)
    {
        pCreateInfo->sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        pCreateInfo->pNext = nullptr;
        pCreateInfo->flags = 0;
        pCreateInfo->messageSeverity =  VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                                        VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                                        VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        pCreateInfo->messageType =  VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                                    VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                                    VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        pCreateInfo->pfnUserCallback = VulkanDebugCallback;
        pCreateInfo->pUserData = nullptr;
    }
    
    
    VkPhysicalDevice VulkanGraphicsDevice::QueryPhyscialDevice()
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
    
    
    bool VulkanGraphicsDevice::PhysicalDeviceIsSuitable(VkPhysicalDevice adapter)
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
        QueueFamilyIndices indices = FindQueueFamilies(adapter, m_Surface);
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
        

        //Check if the adapter and surface has the required support 
        SwapChainCapabilities swapChainInfo = QuerySwapChainSupport(adapter, m_Surface);
        if (!swapChainInfo.Valid())
        {
            LOG_DEBUG_ERROR("Vulkan: Adapter does not have valid SwapChain support\n");
            return false;
        }
        
        return true;
    }
    
    
    std::vector<const char*> VulkanGraphicsDevice::GetRequiredValidationLayers(bool debug)
    {
        std::vector<const char*> requiredLayers;
        if (debug)
        {
            requiredLayers.push_back("VK_LAYER_KHRONOS_validation");
        }

        return requiredLayers;
    }
    
    
    std::vector<const char*> VulkanGraphicsDevice::GetRequiredDeviceExtensions()
    {
        std::vector<const char*> requiredExtensions;
        requiredExtensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
        
		return requiredExtensions;
    }


	std::vector<const char*> VulkanGraphicsDevice::GetRequiredInstanceExtensions(bool debug)
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
		}

		return requiredExtensions;
	}
    
    
	VkSurfaceKHR VulkanGraphicsDevice::CreateSurface(IWindow* pWindow)
    {
#if defined(LAMBDA_PLAT_MACOS)
		VkSurfaceKHR surface = VK_NULL_HANDLE;
        
		GLFWwindow* pGLFWWindow = reinterpret_cast<GLFWwindow*>(pWindow->GetNativeHandle());
        if (glfwCreateWindowSurface(m_Instance, pGLFWWindow, nullptr, &surface) != VK_SUCCESS)
        {
            return VK_NULL_HANDLE;
        }
        else
        {
            return surface;
        }
#elif defined(LAMBDA_PLAT_WINDOWS)
		VkSurfaceKHR surface = VK_NULL_HANDLE;
		HWND hWnd = reinterpret_cast<HWND>(pWindow->GetNativeHandle());

		//Create a surface for windows
		VkWin32SurfaceCreateInfoKHR info = {};
		info.sType		= VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
		info.pNext		= nullptr;
		info.flags		= 0;
		info.hwnd		= hWnd;
		info.hinstance	= GetModuleHandle(nullptr);

		if (vkCreateWin32SurfaceKHR(m_Instance, &info, nullptr, &surface) != VK_SUCCESS)
		{
			return VK_NULL_HANDLE;
		}
		else
		{
			return surface;
		}
#endif
    }
    
    
    bool VulkanGraphicsDevice::CreateDepthStencil()
    {
        TextureDesc depthBufferDesc			= {};
        depthBufferDesc.pResolveResource	= nullptr;
        depthBufferDesc.Flags				= TEXTURE_FLAGS_DEPTH_STENCIL;
        depthBufferDesc.Type				= TEXTURE_TYPE_2D;
        depthBufferDesc.Usage				= RESOURCE_USAGE_DEFAULT;
        depthBufferDesc.ArraySize			= 1;
        depthBufferDesc.Width				= m_pSwapChain->GetWidth();
        depthBufferDesc.Height				= m_pSwapChain->GetHeight();
        depthBufferDesc.Format				= FORMAT_D24_UNORM_S8_UINT;
        depthBufferDesc.SampleCount			= uint32(m_DeviceSettings.SampleCount);
        depthBufferDesc.MipLevels			= 1;
		depthBufferDesc.Depth				= 1;
        
        m_pDepthStencil = DBG_NEW VulkanTexture(m_Device, m_pDeviceAllocator, depthBufferDesc);
        return true;
    }


	bool VulkanGraphicsDevice::CreateMSAABuffer()
	{
		TextureDesc msaaBufferDesc		= {};
        msaaBufferDesc.pResolveResource = m_pSwapChain->GetCurrentBuffer();
		msaaBufferDesc.Type				= TEXTURE_TYPE_2D;
		msaaBufferDesc.Usage			= RESOURCE_USAGE_DEFAULT;
		msaaBufferDesc.Flags			= TEXTURE_FLAGS_RENDER_TARGET;
		msaaBufferDesc.ArraySize		= 1;
		msaaBufferDesc.Width			= m_pSwapChain->GetWidth();
		msaaBufferDesc.Height			= m_pSwapChain->GetHeight();
		msaaBufferDesc.Format			= FORMAT_B8G8R8A8_UNORM;
		msaaBufferDesc.SampleCount		= uint32(m_DeviceSettings.SampleCount);
		msaaBufferDesc.MipLevels		= 1;
		msaaBufferDesc.Depth			= 1;

		m_pMSAABuffer = DBG_NEW VulkanTexture(m_Device, m_pDeviceAllocator, msaaBufferDesc);
		return true;
	}
    
    
    void VulkanGraphicsDevice::ReleaseDepthStencil()
    {
        if (m_pDepthStencil != nullptr)
        {
            m_pDepthStencil->Destroy(m_Device);
            m_pDepthStencil = nullptr;
        }
    }


	void VulkanGraphicsDevice::ReleaseMSAABuffer()
	{
		if (m_pMSAABuffer != nullptr)
		{
			m_pMSAABuffer->Destroy(m_Device);
			m_pMSAABuffer = nullptr;
		}
	}
    
    
    void VulkanGraphicsDevice::GetNextFrame() const
    {
        //Advance current frame counter
        m_CurrentFrame = (m_CurrentFrame + 1) % m_Desc.BackBufferCount;
		m_pDynamicBufferManager->MoveToNextFrame(m_CurrentFrame);

		//Cleanup memory
		m_pDeviceAllocator->CleanGarbageMemory(m_CurrentFrame);
    }
    
    
    void VulkanGraphicsDevice::CreateCommandList(ICommandList** ppList, CommandListType type)
    {
		LAMBDA_ASSERT(ppList != nullptr);
        (*ppList) = DBG_NEW VulkanCommandList(type);
    }
    
    
    void VulkanGraphicsDevice::CreateBuffer(IBuffer** ppBuffer, const ResourceData* pInitalData, const BufferDesc& desc)
    {
		LAMBDA_ASSERT(ppBuffer != nullptr);
        
        VulkanBuffer* pVkBuffer = DBG_NEW VulkanBuffer(m_pDeviceAllocator, desc);

        //Upload inital data
        if (pInitalData)
        {
            if (desc.Usage == RESOURCE_USAGE_DYNAMIC)
            {
                //Register dynamic buffer
                m_pDynamicBufferManager->RegisterBuffer(pVkBuffer);
                
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
    
    
    void VulkanGraphicsDevice::CreateTexture(ITexture** ppTexture, const ResourceData* pInitalData, const TextureDesc& desc)
    {
		LAMBDA_ASSERT(ppTexture != nullptr);
        
        //Create texture object
        VulkanTexture* pVkTexture = DBG_NEW VulkanTexture(m_Device, m_pDeviceAllocator, desc);
        
        //Handle inital data
        if (pInitalData)
        {
            m_pCommandList->Reset();
            //Upload data
            m_pCommandList->UpdateTexture(pVkTexture, pInitalData, 0);

            //Generate mipmaps
            if (desc.Flags & TEXTURE_FLAGS_GENEATE_MIPS)
            {
                VkFormatProperties formatProperties = {};
                vkGetPhysicalDeviceFormatProperties(m_PhysicalDevice, pVkTexture->GetFormat(), &formatProperties);
                
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
    
    
    void VulkanGraphicsDevice::CreateShader(IShader** ppShader, const ShaderDesc& desc)
    {
		LAMBDA_ASSERT(ppShader != nullptr);
        (*ppShader) = DBG_NEW VulkanShader(m_Device, desc);
    }
    
    
    void VulkanGraphicsDevice::CreateSamplerState(ISamplerState** ppSamplerState, const SamplerStateDesc& desc)
    {
		LAMBDA_ASSERT(ppSamplerState != nullptr);
        (*ppSamplerState) = DBG_NEW VulkanSamplerState(m_Device, desc);
    }
    
    
    void VulkanGraphicsDevice::CreateGraphicsPipelineState(IGraphicsPipelineState** ppPipelineState, const GraphicsPipelineStateDesc& desc)
    {
		LAMBDA_ASSERT(ppPipelineState != nullptr);
        (*ppPipelineState) = DBG_NEW VulkanGraphicsPipelineState(m_Device, desc);
    }


	void VulkanGraphicsDevice::CreateRenderPass(IRenderPass** ppRenderPass, const RenderPassDesc& desc)
	{
		LAMBDA_ASSERT(ppRenderPass != nullptr);
		(*ppRenderPass) = DBG_NEW VulkanRenderPass(m_Device, desc);
	}


	void VulkanGraphicsDevice::CreatePipelineResourceState(IPipelineResourceState** ppResourceState, const PipelineResourceStateDesc& desc)
	{
		LAMBDA_ASSERT(ppResourceState != nullptr);
		(*ppResourceState) = DBG_NEW VulkanPipelineResourceState(desc);
	}


	void VulkanGraphicsDevice::CreateUploadBuffer(VulkanUploadBuffer** ppUploadBuffer, uint64 sizeInBytes)
	{
		LAMBDA_ASSERT(ppUploadBuffer != nullptr);
		(*ppUploadBuffer) = DBG_NEW VulkanUploadBuffer(m_Device, m_pDeviceAllocator, sizeInBytes);
	}
    
    
    void VulkanGraphicsDevice::DestroyCommandList(ICommandList** ppList)
    {
		LAMBDA_ASSERT(ppList != nullptr);
        
        //Delete list
        VulkanCommandList* pVkList = reinterpret_cast<VulkanCommandList*>(*ppList);
        if (pVkList != nullptr)
        {
            pVkList->Destroy(m_Device);
            *ppList = nullptr;
        }
    }
    
    
    void VulkanGraphicsDevice::DestroyBuffer(IBuffer** ppBuffer)
    {
		LAMBDA_ASSERT(ppBuffer != nullptr);
        
        //Delete buffer
        VulkanBuffer* pVkBuffer = reinterpret_cast<VulkanBuffer*>(*ppBuffer);
        if (pVkBuffer != nullptr)
        {
			//If buffer is dynamic, unregister
			BufferDesc bufferDesc = pVkBuffer->GetDesc();
			if (bufferDesc.Usage == RESOURCE_USAGE_DYNAMIC)
			{
				m_pDynamicBufferManager->UnregisterBuffer(pVkBuffer);
			}

			//Then destroy buffer
            pVkBuffer->Destroy(m_Device);
            *ppBuffer = nullptr;
            
            LOG_DEBUG_INFO("Vulkan: Destroyed Buffer\n");
        }
    }
    
    
    void VulkanGraphicsDevice::DestroyTexture(ITexture** ppTexture)
    {
		LAMBDA_ASSERT(ppTexture != nullptr);
        
        //Delete texture
        VulkanTexture* pVkTexture = reinterpret_cast<VulkanTexture*>(*ppTexture);
        if (pVkTexture != nullptr)
        {
            pVkTexture->Destroy(m_Device);
            *ppTexture = nullptr;
            
            LOG_DEBUG_INFO("Vulkan: Destroyed Texture2D\n");
        }
    }
    
    
    void VulkanGraphicsDevice::DestroyShader(IShader** ppShader)
    {
		LAMBDA_ASSERT(ppShader != nullptr);
        
        //Delete shader
        VulkanShader* pVkShader = reinterpret_cast<VulkanShader*>(*ppShader);
        if (pVkShader != nullptr)
        {
            pVkShader->Destroy(m_Device);
            *ppShader = nullptr;
            
            LOG_DEBUG_INFO("Vulkan: Destroyed Shader\n");
        }
    }
    
    
    void VulkanGraphicsDevice::DestroySamplerState(ISamplerState** ppSamplerState)
    {
		LAMBDA_ASSERT(ppSamplerState != nullptr);
        
        //Delete SamplerState
        VulkanSamplerState* pVkSamplerState = reinterpret_cast<VulkanSamplerState*>(*ppSamplerState);
        if (pVkSamplerState != nullptr)
        {
            pVkSamplerState->Destroy(m_Device);
            *ppSamplerState = nullptr;
            
            LOG_DEBUG_INFO("Vulkan: Destroyed SamplerState\n");
        }
    }
    
    
    void VulkanGraphicsDevice::DestroyGraphicsPipelineState(IGraphicsPipelineState** ppPipelineState)
    {
		LAMBDA_ASSERT(ppPipelineState != nullptr);
        
        //Delete PipelineState
        VulkanGraphicsPipelineState* pVkPipelineState = reinterpret_cast<VulkanGraphicsPipelineState*>(*ppPipelineState);
        if (pVkPipelineState != nullptr)
        {
			pVkPipelineState->Destroy(m_Device);
            *ppPipelineState = nullptr;
            
            LOG_DEBUG_INFO("Vulkan: Destroyed PipelineState\n");
        }
    }


	void VulkanGraphicsDevice::DestroyRenderPass(IRenderPass** ppRenderPass)
	{
		LAMBDA_ASSERT(ppRenderPass != nullptr);

		//Delete PipelineState
		VulkanRenderPass* pVkRenderPass = reinterpret_cast<VulkanRenderPass*>(*ppRenderPass);
		if (pVkRenderPass != nullptr)
		{
			pVkRenderPass->Destroy(m_Device);
			*ppRenderPass = nullptr;

			LOG_DEBUG_INFO("Vulkan: Destroyed RenderPass\n");
		}
	}


	void VulkanGraphicsDevice::DestroyResourceState(IPipelineResourceState** ppResourceState)
	{
		LAMBDA_ASSERT(ppResourceState != nullptr);

		//Delete ResourceState
		VulkanPipelineResourceState* pVkResourceState = reinterpret_cast<VulkanPipelineResourceState*>(*ppResourceState);
		if (pVkResourceState != nullptr)
		{
			pVkResourceState->Destroy(m_Device);
			*ppResourceState = nullptr;

			LOG_DEBUG_INFO("Vulkan: Destroyed ResourceState\n");
		}
	}
    
    
    void VulkanGraphicsDevice::Destroy() const
    {
		delete this;
    }
    
    
    void VulkanGraphicsDevice::ExecuteCommandList(ICommandList* const * ppLists, uint32 numLists) const
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

    
	void VulkanGraphicsDevice::ExecuteCommandListAndPresent(ICommandList* const* ppLists, uint32 numLists) const
	{
		//LOG_DEBUG_INFO("Vulkan: VulkanGraphicsDevice::ExecuteCommandListAndPresent  Frame '%d' - WaitSemaphore='%x', SignalSemaphore='%x'\n", m_CurrentFrame, m_ImageSemaphores[m_CurrentFrame], m_RenderSemaphores[m_CurrentFrame]);

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
			Present();
		}
	}
    
    
    void VulkanGraphicsDevice::Present() const
    {
        //LOG_DEBUG_INFO("Vulkan: Present Frame '%d' - WaitSemaphore='%x', SignalSemaphore='%x'\n", m_CurrentFrame, m_RenderSemaphores[m_CurrentFrame], m_ImageSemaphores[m_CurrentFrame]);
		
		m_pSwapChain->Present(m_PresentationQueue, m_RenderSemaphores[m_CurrentFrame]);
        GPUWaitForFrame();

		m_pSwapChain->AquireNextImage(m_Device, m_ImageSemaphores[m_CurrentFrame]);
        
		//if we use MSAA we want to set a texture that we can resolve onto, in this case the current backbuffer since we render to the window
        if (m_pMSAABuffer)
        {
            VulkanTexture* pResolveResource = reinterpret_cast<VulkanTexture*>(m_pSwapChain->GetCurrentBuffer());
            m_pMSAABuffer->SetResolveResource(pResolveResource);
        }
    }
    
    
    void VulkanGraphicsDevice::GPUWaitForFrame() const
    {
        //Wait for last frame
        vkWaitForFences(m_Device, 1, &m_Fences[m_CurrentFrame], VK_TRUE, std::numeric_limits<uint64_t>::max());
        vkResetFences(m_Device, 1, &m_Fences[m_CurrentFrame]);
        
		//Move to next frame
        GetNextFrame();
    }
    
    
    void VulkanGraphicsDevice::WaitForGPU() const
    {
        //LOG_DEBUG_INFO("VulkanGraphicsDevice::WaitForGPU\n");
        vkQueueWaitIdle(m_GraphicsQueue);
    }

    
    ResourceFormat VulkanGraphicsDevice::GetBackBufferFormat() const
    {
        return ConvertVkFormat(m_pSwapChain->GetFormat());
    }
    
    
    void* VulkanGraphicsDevice::GetNativeHandle() const
    {
        return reinterpret_cast<void*>(m_Device);
    }
    
    
    GraphicsDeviceDesc VulkanGraphicsDevice::GetDesc() const
    {
        return m_Desc;
    }
    
    
    ITexture* VulkanGraphicsDevice::GetRenderTarget() const
    {
        return (m_DeviceSettings.SampleCount > VK_SAMPLE_COUNT_1_BIT) ? m_pMSAABuffer : m_pSwapChain->GetCurrentBuffer();
    }
    
    
    ITexture* VulkanGraphicsDevice::GetDepthStencil() const
    {
        return m_pDepthStencil;
    }
    
    
    uint32 VulkanGraphicsDevice::GetBackBufferIndex() const
    {
        return m_pSwapChain->GetBackBufferIndex();
    }
    
    
    uint32 VulkanGraphicsDevice::GetSwapChainWidth() const
    {
        return m_pSwapChain->GetWidth();
    }
    
    
    uint32 VulkanGraphicsDevice::GetSwapChainHeight() const
    {
        return m_pSwapChain->GetHeight();
    }
    
    
    bool VulkanGraphicsDevice::InternalOnEvent(const Event& event)
    {
        //Handle resize event
        if (event.Type == EVENT_TYPE_WINDOW_RESIZE)
        {
            //When we minimize or any other reason the size is zero
            //Do not resize if the size is the same as the current one
            if ((event.WindowResize.Width == 0 || event.WindowResize.Height == 0) || (event.WindowResize.Width == m_pSwapChain->GetWidth() && event.WindowResize.Height == m_pSwapChain->GetHeight()))
            {
                return false;
            }
            
            //Syncronize the GPU so no operations are in flight when recreating swapchain
            WaitForGPU();
           
            m_pSwapChain->ResizeBuffers(m_Device, m_ImageSemaphores[m_CurrentFrame], event.WindowResize.Width, event.WindowResize.Height);

			//Recreate depthstencil
            ReleaseDepthStencil();
            CreateDepthStencil();

			//Recreate MSAA buffer if MSAA is used
			if (m_DeviceSettings.SampleCount > VK_SAMPLE_COUNT_1_BIT)
			{
				ReleaseMSAABuffer();
				CreateMSAABuffer();
			}
            
            LOG_DEBUG_INFO("VulkanGraphicsDevice: Window resized w: %d h: %d\n", event.WindowResize.Width, event.WindowResize.Height);
        }
        
        return false;
    }
    
    
    void VulkanGraphicsDevice::SetVulkanObjectName(VkObjectType type, uint64 objectHandle, const std::string& name)
    {
        if (SetDebugUtilsObjectNameEXT)
        {
            //Set name on object
            VkDebugUtilsObjectNameInfoEXT info = {};
            info.sType			= VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
            info.pNext			= nullptr;
            info.objectType		= type;
            info.pObjectName	= name.c_str();
            info.objectHandle	= objectHandle;
            
            if (SetDebugUtilsObjectNameEXT(m_Device, &info) != VK_SUCCESS)
            {
                LOG_DEBUG_ERROR("Vulkan: Failed to set name '%s'\n", info.pObjectName);
            }
        }
    }


	VulkanGraphicsDevice& VulkanGraphicsDevice::GetInstance()
	{
		LAMBDA_ASSERT(s_pInstance != nullptr);
		
		VulkanGraphicsDevice* pVkDevice = reinterpret_cast<VulkanGraphicsDevice*>(s_pInstance);
		return *pVkDevice;
	}


	VKAPI_ATTR VkBool32 VKAPI_CALL VulkanGraphicsDevice::VulkanDebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData)
	{
		//Get severity
		LogSeverity severity = LOG_SEVERITY_UNKNOWN;
		switch (messageSeverity)
		{
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:      severity = LOG_SEVERITY_INFO;       break;
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:   severity = LOG_SEVERITY_WARNING;    break;
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:     severity = LOG_SEVERITY_ERROR;      break;
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


		Log::GetDebugLog().Print(severity, "[Vulkan Validation Layer - type=%s, object=0x%llx, objectname=%s] %s\n", pTypeStr, object, pObjectName, pCallbackData->pMessage);
		return VK_FALSE;
	}
}
