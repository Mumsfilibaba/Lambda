#include "LambdaPch.h"
#include "VKNDevice.h"
#include "VKNConversions.inl"
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
	//---------
	//VKNDevice
	//---------

	VKNDevice*							VKNDevice::s_pInstance						= nullptr;
	PFN_vkSetDebugUtilsObjectNameEXT	VKNDevice::SetDebugUtilsObjectNameEXT		= nullptr;
	PFN_vkCreateDebugUtilsMessengerEXT	VKNDevice::CreateDebugUtilsMessengerEXT		= nullptr;
	PFN_vkDestroyDebugUtilsMessengerEXT	VKNDevice::DestroyDebugUtilsMessengerEXT	= nullptr;


	VKNDevice::VKNDevice(const GraphicsDeviceDesc& desc)
		: m_Instance(VK_NULL_HANDLE),
		m_DebugMessenger(VK_NULL_HANDLE),
		m_Device(VK_NULL_HANDLE),
		m_PhysicalDevice(VK_NULL_HANDLE),
		m_Surface(VK_NULL_HANDLE),
		m_DeviceSettings(),
		m_FamiliyIndices(),
		m_PhysicalDeviceProperties()
	{
		LAMBDA_ASSERT(s_pInstance == nullptr);
		s_pInstance = this;

		Init(desc);
	}


	void VKNDevice::Destroy()
	{
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

		delete this;
	}


	void VKNDevice::Init(const GraphicsDeviceDesc& desc)
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


		//Get max MSAA we can use on the device
		VkSampleCountFlags		sampleCountFlags = std::min(m_PhysicalDeviceProperties.limits.framebufferStencilSampleCounts,
			std::min(m_PhysicalDeviceProperties.limits.framebufferColorSampleCounts, m_PhysicalDeviceProperties.limits.framebufferDepthSampleCounts));
		VkSampleCountFlagBits	sampleCount = VK_SAMPLE_COUNT_1_BIT;

		if (sampleCountFlags & VK_SAMPLE_COUNT_64_BIT) { sampleCount = VK_SAMPLE_COUNT_64_BIT; }
		else if (sampleCountFlags & VK_SAMPLE_COUNT_32_BIT) { sampleCount = VK_SAMPLE_COUNT_32_BIT; }
		else if (sampleCountFlags & VK_SAMPLE_COUNT_16_BIT) { sampleCount = VK_SAMPLE_COUNT_16_BIT; }
		else if (sampleCountFlags & VK_SAMPLE_COUNT_8_BIT) { sampleCount = VK_SAMPLE_COUNT_8_BIT; }
		else if (sampleCountFlags & VK_SAMPLE_COUNT_4_BIT) { sampleCount = VK_SAMPLE_COUNT_4_BIT; }
		else if (sampleCountFlags & VK_SAMPLE_COUNT_2_BIT) { sampleCount = VK_SAMPLE_COUNT_2_BIT; }

		//Set devicesettings
		m_DeviceSettings.FramesAhead = desc.BackBufferCount;
		m_DeviceSettings.SampleCount = ConvertSampleCount(desc.SampleCount);
		if (m_DeviceSettings.SampleCount > sampleCount)
		{
			m_DeviceSettings.SampleCount = sampleCount;
			LOG_DEBUG_WARNING("Vulkan: SampleCount (= %u) is higher  than the device's maximum of %u. SampleCount was set to the device's maximum.\n", uint32(m_DeviceSettings.SampleCount), uint32(sampleCount));
		}
	}


	void VKNDevice::SetVulkanObjectName(VkObjectType type, uint64 objectHandle, const std::string& name)
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


	void VKNDevice::InitDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo)
	{
		pCreateInfo->sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		pCreateInfo->pNext = nullptr;
		pCreateInfo->flags = 0;
		pCreateInfo->messageSeverity =	VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
										VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
										VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		pCreateInfo->messageType =	VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT		|
									VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT	|
									VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		pCreateInfo->pfnUserCallback = VulkanDebugCallback;
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


	std::vector<const char*> VKNDevice::GetRequiredValidationLayers(bool debug)
	{
		std::vector<const char*> requiredLayers;
		if (debug)
		{
			requiredLayers.push_back("VK_LAYER_KHRONOS_validation");
		}

		return requiredLayers;
	}


	std::vector<const char*> VKNDevice::GetRequiredDeviceExtensions()
	{
		std::vector<const char*> requiredExtensions;
		requiredExtensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
		requiredExtensions.push_back(VK_KHR_MAINTENANCE1_EXTENSION_NAME);

		return requiredExtensions;
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
		}

		return requiredExtensions;
	}


	VkSurfaceKHR VKNDevice::CreateSurface(IWindow* pWindow)
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


	VKAPI_ATTR VkBool32 VKAPI_CALL VKNDevice::VulkanDebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
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
	
	
	VKNDevice& VKNDevice::GetInstance()
	{
		LAMBDA_ASSERT(s_pInstance != nullptr);
		return *s_pInstance;
	}
}