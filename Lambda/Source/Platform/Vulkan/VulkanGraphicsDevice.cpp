#include "LambdaPch.h"
#include <set>
#include <algorithm>
#include "VulkanGraphicsDevice.h"
#include "VulkanShader.h"
#include "VulkanPipelineState.h"
#include "VulkanTexture2D.h"
#include "VulkanSamplerState.h"
#include "VulkanCommandList.h"
#include "VulkanFramebuffer.h"
#include "VulkanBuffer.h"
#include "VulkanSwapChain.h"
#include "VulkanRenderPass.h"
#include "VulkanResourceState.h"
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
    //Debugcallback
    VKAPI_ATTR VkBool32 VKAPI_CALL VulkanGraphicsDevice::VulkanDebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData)
    {
        //Check message type
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
        uint64 object           = (pCallbackData->objectCount != 0) ? pCallbackData->pObjects->objectHandle : 0;
        const char* pObjectName = (pCallbackData->objectCount != 0) ? pCallbackData->pObjects->pObjectName : "";
        
        //Print message
        Log::GetDebugLog().Print(severity, "[Vulkan Validation Layer - type=%s, object=0x%llx, objectname=%s] %s\n", pTypeStr, object, pObjectName, pCallbackData->pMessage);
        return VK_FALSE;
    }
    
    
    //Vulkan device static members
    PFN_vkSetDebugUtilsObjectNameEXT VulkanGraphicsDevice::SetDebugUtilsObjectNameEXT		= nullptr;
    PFN_vkCreateDebugUtilsMessengerEXT VulkanGraphicsDevice::CreateDebugUtilsMessengerEXT	= nullptr;
    PFN_vkDestroyDebugUtilsMessengerEXT VulkanGraphicsDevice::DestroyDebugUtilsMessengerEXT = nullptr;
    
    
    //Vulkan Graphics Context
    VulkanGraphicsDevice::VulkanGraphicsDevice(IWindow* pWindow, const GraphicsDeviceDesc& desc)
        : m_Instance(VK_NULL_HANDLE),
        m_DebugMessenger(VK_NULL_HANDLE),
        m_Device(VK_NULL_HANDLE),
        m_GraphicsQueue(VK_NULL_HANDLE),
        m_PresentationQueue(VK_NULL_HANDLE),
        m_RenderSemaphores(),
        m_FamiliyIndices(),
        m_Adapter(VK_NULL_HANDLE),
        m_AdapterProperties(),
        m_Surface(VK_NULL_HANDLE),
		m_pSwapChain(nullptr),
		m_CurrentFrame(0)
    {       
        assert(s_pInstance == nullptr);
        s_pInstance = this;
        
        Init(pWindow, desc);
    }
    
    
    VulkanGraphicsDevice::~VulkanGraphicsDevice()
    {
        ICommandList* pDirectCommandList = m_pCommandList;
        DestroyCommandList(&pDirectCommandList);
               
        for (uint32 i = 0; i < FRAMES_AHEAD; i++)
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
        

        VulkanFramebufferCache::ReleaseAll(m_Device);
        
		m_pSwapChain->Destroy(m_Device);
        ReleaseDepthStencil();

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
    
    
    void VulkanGraphicsDevice::Init(IWindow* pWindow, const GraphicsDeviceDesc& desc)
    {
        assert(pWindow != nullptr);
        
		VkApplicationInfo applicationInfo = {};
		applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		applicationInfo.pNext = nullptr;
		applicationInfo.pApplicationName = "Lambda Engine";
		applicationInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
		applicationInfo.pEngineName = "Lambda Engine";
		applicationInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
		applicationInfo.apiVersion = VK_API_VERSION_1_0;


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
		instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		instanceInfo.pNext = (desc.Flags & GRAPHICS_CONTEXT_FLAG_DEBUG) ? (VkDebugUtilsMessengerCreateInfoEXT*)& dInfo : nullptr;
		instanceInfo.flags = 0;
		instanceInfo.pApplicationInfo = &applicationInfo;
		instanceInfo.enabledExtensionCount = uint32(requiredExtensions.size());
		instanceInfo.ppEnabledExtensionNames = requiredExtensions.data();
		instanceInfo.enabledLayerCount = uint32(requiredLayers.size());
		instanceInfo.ppEnabledLayerNames = requiredLayers.data();

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
		m_Surface = CreateSurface(pWindow);
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
		m_Adapter = QueryAdapter();
		if (m_Adapter == VK_NULL_HANDLE)
		{
			LOG_DEBUG_ERROR("Vulkan: Failed to find a suitable GPU\n");
			return;
		}
		else
		{
			vkGetPhysicalDeviceProperties(m_Adapter, &m_AdapterProperties);
			m_FamiliyIndices = FindQueueFamilies(m_Adapter, m_Surface);

			VkPhysicalDeviceMemoryProperties memoryProperties = {};
			vkGetPhysicalDeviceMemoryProperties(m_Adapter, &memoryProperties);

			uint64 vram = 0;
			for (uint32 i = 0; i < memoryProperties.memoryHeapCount; i++)
			{
				if (memoryProperties.memoryHeaps[i].flags & VK_MEMORY_HEAP_DEVICE_LOCAL_BIT)
					vram = memoryProperties.memoryHeaps[i].size;
			}

			vram = vram / (1024 * 1024);
			LOG_SYSTEM_PRINT("Vulkan: Selected GPU '%s'\n        VRAM: %llu MB\n", m_AdapterProperties.deviceName, vram);
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
		vkEnumerateDeviceExtensionProperties(m_Adapter, nullptr, &extensionCount, nullptr);

		std::vector<VkExtensionProperties> availableDeviceExtensions(extensionCount);
		vkEnumerateDeviceExtensionProperties(m_Adapter, nullptr, &extensionCount, availableDeviceExtensions.data());

		LOG_DEBUG_INFO("[Vulkan] Available Device-Extensions:\n");
		for (const auto& extension : availableDeviceExtensions)
		{
			LOG_DEBUG_INFO("   Device-Extension '%s'\n", extension.extensionName);
		}


		//Setup the device
		VkDeviceCreateInfo info = {};
		info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		info.pNext = nullptr;
		info.flags = 0;
		info.enabledExtensionCount = uint32(deviceExtensions.size());
		info.ppEnabledExtensionNames = deviceExtensions.data();
		info.enabledLayerCount = uint32(requiredLayers.size());
		info.ppEnabledLayerNames = requiredLayers.data(); //Same as for the instance
		info.pEnabledFeatures = &deviceFeatures;
		info.queueCreateInfoCount = uint32(queueCreateInfos.size());
		info.pQueueCreateInfos = queueCreateInfos.data();

		//Create device
		if (vkCreateDevice(m_Adapter, &info, nullptr, &m_Device) != VK_SUCCESS)
		{
			LOG_DEBUG_ERROR("Vulkan: Failed to create device\n");

			m_Device = VK_NULL_HANDLE;
			return;
		}
		else
		{
			LOG_SYSTEM_PRINT("Vulkan: Created device and retrived queues\n");

			//Get queues
			vkGetDeviceQueue(m_Device, m_FamiliyIndices.GraphicsFamily, 0, &m_GraphicsQueue);
			vkGetDeviceQueue(m_Device, m_FamiliyIndices.PresentFamily, 0, &m_PresentationQueue);
		}


        //Setup semaphore structure
        VkSemaphoreCreateInfo semaphoreInfo = {};
        semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
        semaphoreInfo.pNext = nullptr;
        semaphoreInfo.flags = 0;
        
        //Setup fence struct
        VkFenceCreateInfo fenceInfo = {};
        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        
        //Resize vectors
        m_ImageSemaphores.resize(FRAMES_AHEAD);
        m_RenderSemaphores.resize(FRAMES_AHEAD);
        m_Fences.resize(FRAMES_AHEAD);
        
        //Create sync-objects
        for (uint32 i = 0; i < FRAMES_AHEAD; i++)
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
                //Set semaphorenames
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
        
        
        //Create swapchain
        VulkanSwapChainDesc swapChainInfo = {};
        swapChainInfo.Adapter            = m_Adapter;
        swapChainInfo.Surface            = m_Surface;
        swapChainInfo.SignalSemaphore    = m_ImageSemaphores[m_CurrentFrame];
        swapChainInfo.PresentationMode   = VK_PRESENT_MODE_MAILBOX_KHR;
        swapChainInfo.Format.format      = VK_FORMAT_B8G8R8A8_UNORM;
        swapChainInfo.Format.colorSpace  = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
        swapChainInfo.Extent             = { pWindow->GetWidth(), pWindow->GetHeight() };
        swapChainInfo.ImageCount         = FRAMES_AHEAD;
        
        m_pSwapChain = DBG_NEW VulkanSwapChain(m_Device, swapChainInfo);

        
        //Create depthbuffer
        if (!CreateDepthStencil())
        {
            LOG_DEBUG_INFO("Vulkan: Failed to create DepthBuffer");
            return;
        }
         
        //Init GraphicsDevice dependent members
        CreateCommandList(reinterpret_cast<ICommandList**>(&m_pCommandList), COMMAND_LIST_TYPE_GRAPHICS);
        m_pCommandList->SetName("Graphics Device Internal CommandList");
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
    
    
    VkPhysicalDevice VulkanGraphicsDevice::QueryAdapter()
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
            if (AdapterIsSuitable(adapter))
            {
                return adapter;
            }
        }

		return VK_NULL_HANDLE;
    }
    
    
    bool VulkanGraphicsDevice::AdapterIsSuitable(VkPhysicalDevice adapter)
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
        
        //Check if the swapchain supports whats required
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
            //Pushback layers
            requiredLayers.push_back("VK_LAYER_KHRONOS_validation");
        }

        return requiredLayers;
    }
    
    
    std::vector<const char*> VulkanGraphicsDevice::GetRequiredDeviceExtensions()
    {
        std::vector<const char*> requiredExtensions;
        //Pushback layers
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
			//Pushback extensions
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
        Texture2DDesc depthBufferDesc = {};
        depthBufferDesc.Usage              = RESOURCE_USAGE_DEFAULT;
        depthBufferDesc.Flags              = TEXTURE_FLAGS_DEPTH_STENCIL;
        depthBufferDesc.ArraySize          = 1;
        depthBufferDesc.Width              = m_pSwapChain->GetWidth();
        depthBufferDesc.Height             = m_pSwapChain->GetHeight();
        depthBufferDesc.Format             = FORMAT_D24_UNORM_S8_UINT;
        depthBufferDesc.SampleCount        = 1;
        depthBufferDesc.MipLevels          = 1;
        depthBufferDesc.ClearValue.Depth   = 1.0f;
        depthBufferDesc.ClearValue.Stencil = 0;
        
        m_pDepthStencil = DBG_NEW VulkanTexture2D(m_Device, m_Adapter, depthBufferDesc);
        return true;
    }
    
    
    void VulkanGraphicsDevice::ReleaseDepthStencil()
    {
        //Release depthbuffer
        if (m_pDepthStencil != nullptr)
        {
            m_pDepthStencil->Destroy(m_Device);
            m_pDepthStencil = nullptr;
        }
    }
    
    
    void VulkanGraphicsDevice::GetNextFrame() const
    {
        //Advance current frame counter
        m_CurrentFrame = (m_CurrentFrame + 1) % FRAMES_AHEAD;
    }
    
    
    void VulkanGraphicsDevice::CreateCommandList(ICommandList** ppList, CommandListType type) const
    {
        assert(ppList != nullptr);
        (*ppList) = DBG_NEW VulkanCommandList(m_Device, type);
    }
    
    
    void VulkanGraphicsDevice::CreateBuffer(IBuffer** ppBuffer, const ResourceData* pInitalData, const BufferDesc& desc) const
    {
        assert(ppBuffer != nullptr);
        
        //Create buffer
        VulkanBuffer* pBuffer = DBG_NEW VulkanBuffer(m_Device, m_Adapter, desc);
        
        //Upload inital data
        if (pInitalData)
        {
            if (desc.Usage == RESOURCE_USAGE_DYNAMIC)
            {
                //Upload directly to buffer if it is dynamic
                void* pData = nullptr;
                pBuffer->Map(&pData);
                memcpy(pData, pInitalData->pData, pInitalData->SizeInBytes);
                pBuffer->Unmap();
            }
            else if (desc.Usage == RESOURCE_USAGE_DEFAULT)
            {
                m_pCommandList->Reset();
                m_pCommandList->UpdateBuffer(pBuffer, pInitalData);
                m_pCommandList->Close();
            
                ICommandList* ppLists[] = { m_pCommandList };
                ExecuteCommandList(ppLists, 1);
                
                //Wait until buffer is copied
                WaitForGPU();
            }
        }
        
        //Set inputptr to the newly created buffer
        (*ppBuffer) = pBuffer;
    }
    
    
    void VulkanGraphicsDevice::CreateTexture2D(ITexture2D** ppTexture, const ResourceData* pInitalData, const Texture2DDesc& desc) const
    {
        assert(ppTexture != nullptr);
        
        //Create texture object
        VulkanTexture2D* pTexture = DBG_NEW VulkanTexture2D(m_Device, m_Adapter, desc);
        
        //Upload inital data
        if (pInitalData)
        {
            m_pCommandList->Reset();
            m_pCommandList->UpdateTexture(pTexture, pInitalData, 0);
            
            if (desc.Flags & TEXTURE_FLAGS_GENEATE_MIPS)
            {
				uint32 mipLevels = pTexture->GetMipLevels();

				VkImage image = reinterpret_cast<VkImage>(pTexture->GetNativeHandle());
				VkCommandBuffer commandBuffer = reinterpret_cast<VkCommandBuffer>(m_pCommandList->GetNativeHandle());

				VkImageMemoryBarrier barrier = {};
				barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
				barrier.image = image;
				barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
				barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
				barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
				barrier.subresourceRange.baseArrayLayer = 0;
				barrier.subresourceRange.layerCount = 1;
				barrier.subresourceRange.levelCount = 1;

				int32 mipWidth = pTexture->GetWidth();
				int32 mipHeight = pTexture->GetHeight();
				for (uint32 i = 1; i < mipLevels; i++) 
				{
					barrier.subresourceRange.baseMipLevel = i - 1;
					barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
					barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
					barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
					barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
		
					vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier);

					VkImageBlit blit = {};
					blit.srcOffsets[0] = { 0, 0, 0 };
					blit.srcOffsets[1] = { mipWidth, mipHeight, 1 };
					blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
					blit.srcSubresource.mipLevel = i - 1;
					blit.srcSubresource.baseArrayLayer = 0;
					blit.srcSubresource.layerCount = 1;
					blit.dstOffsets[0] = { 0, 0, 0 };
					blit.dstOffsets[1] = { mipWidth > 1 ? mipWidth / 2 : 1, mipHeight > 1 ? mipHeight / 2 : 1, 1 };
					blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
					blit.dstSubresource.mipLevel = i;
					blit.dstSubresource.baseArrayLayer = 0;
					blit.dstSubresource.layerCount = 1;

					vkCmdBlitImage(commandBuffer, image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &blit, VK_FILTER_LINEAR);

					barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
					barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
					barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
					barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

					vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier);

					if (mipWidth > 1)
						mipWidth /= 2;

					if (mipHeight > 1) 
						mipHeight /= 2;
				}
            }
            
            m_pCommandList->Close();
            
            ICommandList* ppLists[] = { m_pCommandList };
            ExecuteCommandList(ppLists, 1);

            WaitForGPU();
        }
        
        //Return texture
        (*ppTexture) = pTexture;
    }
    
    
    void VulkanGraphicsDevice::CreateShader(IShader** ppShader, const ShaderDesc& desc) const
    {
        assert(ppShader != nullptr);
        (*ppShader) = DBG_NEW VulkanShader(m_Device, desc);
    }
    
    
    void VulkanGraphicsDevice::CreateSamplerState(ISamplerState** ppSamplerState, const SamplerStateDesc& desc) const
    {
        assert(ppSamplerState != nullptr);
        (*ppSamplerState) = DBG_NEW VulkanSamplerState(m_Device, desc);
    }
    
    
    void VulkanGraphicsDevice::CreateGraphicsPipelineState(IGraphicsPipelineState** ppPipelineState, const GraphicsPipelineStateDesc& desc) const
    {
        assert(ppPipelineState != nullptr);
        (*ppPipelineState) = DBG_NEW VulkanGraphicsPipelineState(m_Device, desc);
    }


	void VulkanGraphicsDevice::CreateRenderPass(IRenderPass** ppRenderPass, const RenderPassDesc& desc) const
	{
		assert(ppRenderPass != nullptr);
		(*ppRenderPass) = DBG_NEW VulkanRenderPass(m_Device, desc);
	}


	void VulkanGraphicsDevice::CreateResourceState(IResourceState** ppResourceState, const ResourceStateDesc& desc) const
	{
		assert(ppResourceState != nullptr);
		(*ppResourceState) = DBG_NEW VulkanResourceState(m_Device, desc);
	}
    
    
    void VulkanGraphicsDevice::DestroyCommandList(ICommandList** ppList) const
    {
        assert(ppList != nullptr);
        
        //Delete list
        VulkanCommandList* pList = reinterpret_cast<VulkanCommandList*>(*ppList);
        if (pList != nullptr)
        {
            pList->Destroy(m_Device);
            *ppList = nullptr;
        }
    }
    
    
    void VulkanGraphicsDevice::DestroyBuffer(IBuffer** ppBuffer) const
    {
        assert(ppBuffer != nullptr);
        
        //Delete buffer
        VulkanBuffer* pBuffer = reinterpret_cast<VulkanBuffer*>(*ppBuffer);
        if (pBuffer != nullptr)
        {
            pBuffer->Destroy(m_Device);
            *ppBuffer = nullptr;
            
            LOG_DEBUG_INFO("Vulkan: Destroyed Buffer\n");
        }
    }
    
    
    void VulkanGraphicsDevice::DestroyTexture2D(ITexture2D** ppTexture) const
    {
        assert(ppTexture != nullptr);
        
        //Delete texture
        VulkanTexture2D* pTexture = reinterpret_cast<VulkanTexture2D*>(*ppTexture);
        if (pTexture != nullptr)
        {
            pTexture->Destroy(m_Device);
            *ppTexture = nullptr;
            
            LOG_DEBUG_INFO("Vulkan: Destroyed Texture2D\n");
        }
    }
    
    
    void VulkanGraphicsDevice::DestroyShader(IShader** ppShader) const
    {
        assert(ppShader != nullptr);
        
        //Delete shader
        VulkanShader* pShader = reinterpret_cast<VulkanShader*>(*ppShader);
        if (pShader != nullptr)
        {
            pShader->Destroy(m_Device);
            *ppShader = nullptr;
            
            LOG_DEBUG_INFO("Vulkan: Destroyed Shader\n");
        }
    }
    
    
    void VulkanGraphicsDevice::DestroySamplerState(ISamplerState** ppSamplerState) const
    {
        assert(ppSamplerState != nullptr);
        
        //Delete SamplerState
        VulkanSamplerState* pSamplerState = reinterpret_cast<VulkanSamplerState*>(*ppSamplerState);
        if (pSamplerState != nullptr)
        {
            pSamplerState->Destroy(m_Device);
            *ppSamplerState = nullptr;
            
            LOG_DEBUG_INFO("Vulkan: Destroyed SamplerState\n");
        }
    }
    
    
    void VulkanGraphicsDevice::DestroyGraphicsPipelineState(IGraphicsPipelineState** ppPipelineState) const
    {
        assert(ppPipelineState != nullptr);
        
        //Delete PipelineState
        VulkanGraphicsPipelineState* pPipelineState = reinterpret_cast<VulkanGraphicsPipelineState*>(*ppPipelineState);
        if (pPipelineState != nullptr)
        {
			pPipelineState->Destroy(m_Device);
            *ppPipelineState = nullptr;
            
            LOG_DEBUG_INFO("Vulkan: Destroyed PipelineState\n");
        }
    }


	void VulkanGraphicsDevice::DestroyRenderPass(IRenderPass** ppRenderPass) const
	{
		assert(ppRenderPass != nullptr);

		//Delete PipelineState
		VulkanRenderPass* pRenderPass = reinterpret_cast<VulkanRenderPass*>(*ppRenderPass);
		if (pRenderPass != nullptr)
		{
			pRenderPass->Destroy(m_Device);
			*ppRenderPass = nullptr;

			LOG_DEBUG_INFO("Vulkan: Destroyed RenderPass\n");
		}
	}


	void VulkanGraphicsDevice::DestroyResourceState(IResourceState** ppResourceState) const
	{
		assert(ppResourceState != nullptr);

		//Delete PipelineState
		VulkanResourceState* pResourceState = reinterpret_cast<VulkanResourceState*>(*ppResourceState);
		if (pResourceState != nullptr)
		{
			pResourceState->Destroy(m_Device);
			*ppResourceState = nullptr;

			LOG_DEBUG_INFO("Vulkan: Destroyed ResourceState\n");
		}
	}
    
    
    void VulkanGraphicsDevice::Destroy() const
    {
		//Delete 
		delete this;
    }
    
    
    void VulkanGraphicsDevice::ExecuteCommandList(ICommandList* const * ppLists, uint32 numLists) const
    {
        //Retrive commandbuffers
        std::vector<VkCommandBuffer> buffers;
        for (uint32 i = 0; i < numLists; i++)
            buffers.push_back(reinterpret_cast<VkCommandBuffer>(ppLists[i]->GetNativeHandle()));
        
        //Setup submitinfo        
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

        //submit commandbuffers
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
			buffers.push_back(reinterpret_cast<VkCommandBuffer>(ppLists[i]->GetNativeHandle()));

		//Setup submitinfo
		VkSemaphore waitSemaphores[] = { m_ImageSemaphores[m_CurrentFrame] };
		VkSemaphore signalSemaphores[] = { m_RenderSemaphores[m_CurrentFrame] };
		VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

		VkSubmitInfo submitInfo = {};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.pNext = nullptr;
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = waitSemaphores;
		submitInfo.pWaitDstStageMask = waitStages;
		submitInfo.commandBufferCount = numLists;
		submitInfo.pCommandBuffers = buffers.data();
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = signalSemaphores;

		//submit commandbuffers
		if (vkQueueSubmit(m_GraphicsQueue, 1, &submitInfo, m_Fences[m_CurrentFrame]) != VK_SUCCESS)
		{
			LOG_DEBUG_ERROR("Vulkan: Failed to submit CommandBuffers\n");
		}

		//Present
		Present();
	}
    
    
    void VulkanGraphicsDevice::Present() const
    {
        //LOG_DEBUG_INFO("Vulkan: Present Frame '%d' - WaitSemaphore='%x', SignalSemaphore='%x'\n", m_CurrentFrame, m_RenderSemaphores[m_CurrentFrame], m_ImageSemaphores[m_CurrentFrame]);
		
		m_pSwapChain->Present(m_PresentationQueue, m_RenderSemaphores[m_CurrentFrame]);
        GPUWaitForFrame();

		m_pSwapChain->AquireNextImage(m_Device, m_ImageSemaphores[m_CurrentFrame]);
    }
    
    
    void VulkanGraphicsDevice::GPUWaitForFrame() const
    {
        //Wait for last frame
        vkWaitForFences(m_Device, 1, &m_Fences[m_CurrentFrame], VK_TRUE, std::numeric_limits<uint64_t>::max());
        vkResetFences(m_Device, 1, &m_Fences[m_CurrentFrame]);
        
        GetNextFrame();
    }
    
    
    void VulkanGraphicsDevice::WaitForGPU() const
    {
        //LOG_DEBUG_INFO("VulkanGraphicsDevice::WaitForGPU\n");
        vkQueueWaitIdle(m_GraphicsQueue);
        //vkDeviceWaitIdle(m_Device);
    }

    
    ResourceFormat VulkanGraphicsDevice::GetBackBufferFormat() const
    {
        return ConvertVkFormat(m_pSwapChain->GetFormat());
    }
    
    
    void* VulkanGraphicsDevice::GetNativeHandle() const
    {
        return reinterpret_cast<void*>(m_Device);
    }
    
    
    ITexture2D* VulkanGraphicsDevice::GetCurrentRenderTarget() const
    {
        return m_pSwapChain->GetCurrentBuffer();
    }
    
    
    ITexture2D* VulkanGraphicsDevice::GetDepthStencil() const
    {
        return m_pDepthStencil;
    }
    
    
    uint32 VulkanGraphicsDevice::GetCurrentBackBufferIndex() const
    {
        return m_pSwapChain->GetCurrentBackBufferIndex();
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
            
            //Resize the swapchain
            m_pSwapChain->ResizeBuffers(m_Device, m_ImageSemaphores[m_CurrentFrame], event.WindowResize.Width, event.WindowResize.Height);

			//Recreate depthstencil
            ReleaseDepthStencil();
            CreateDepthStencil();
            
            //Resize swapchain etc. here
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
            info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
            info.pNext = nullptr;
            info.objectType = type;
            info.pObjectName = name.c_str();
            info.objectHandle = objectHandle;
            
            if (SetDebugUtilsObjectNameEXT(m_Device, &info) != VK_SUCCESS)
            {
                LOG_DEBUG_ERROR("Vulkan: Failed to set name '%s'\n", info.pObjectName);
            }
        }
    }
}
