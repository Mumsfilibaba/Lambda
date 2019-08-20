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
#include "VulkanHelpers.inl"
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
        
        //Init default descriptorsetlayouts
		for (uint32 i = 0; i < LAMBDA_SHADERSTAGE_COUNT; i++)
		{
            m_DefaultDescriptorSetLayouts[i] = VK_NULL_HANDLE;
		}
        
        Init(pWindow, desc);
    }
    
    
    VulkanGraphicsDevice::~VulkanGraphicsDevice()
    {
        //Destroy nullbuffer
        if (m_pNullBuffer)
        {
            m_pNullBuffer->Destroy(m_Device);
            m_pNullBuffer = nullptr;
        }
        //Destroy nulltexture
        if (m_pNullTexture)
        {
            m_pNullTexture->Destroy(m_Device);
            m_pNullTexture = nullptr;
        }
        //Destroy nullbuffer
        if (m_pNullSampler)
        {
            m_pNullSampler->Destroy(m_Device);
            m_pNullSampler = nullptr;
        }
        
        
        //Destroy the directcommandlist
        ICommandList* pDirectCommandList = m_pCommandList;
        DestroyCommandList(&pDirectCommandList);
        
        //Destroy default pipelinelayout
        if (m_DefaultPipelineLayout != VK_NULL_HANDLE)
        {
            vkDestroyPipelineLayout(m_Device, m_DefaultPipelineLayout, nullptr);
            m_DefaultPipelineLayout = VK_NULL_HANDLE;
        }
        
        //Destroy default descriptorsetlayouts
        for (uint32 i = 0; i < LAMBDA_SHADERSTAGE_COUNT; i++)
        {
            if (m_DefaultDescriptorSetLayouts[i] != VK_NULL_HANDLE)
            {
                vkDestroyDescriptorSetLayout(m_Device, m_DefaultDescriptorSetLayouts[i], nullptr);
                m_DefaultDescriptorSetLayouts[i] = VK_NULL_HANDLE;
            }
        }
        
        //Destroy semaphores and fences
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
        
        //Destroy all framebuffers and renderpasses
        VulkanFramebufferCache::ReleaseAll(m_Device);
        VulkanRenderPassCache::ReleaseAll(m_Device);
        
        //Destroy swapchain and depthstencil
		m_pSwapChain->Destroy(m_Device);
        ReleaseDepthStencil();
        
        //Destroy device
        if (m_Device != VK_NULL_HANDLE)
        {
            vkDestroyDevice(m_Device, nullptr);
            m_Device = VK_NULL_HANDLE;
        }

        //Destroy debugmessenger
        if (m_DebugMessenger != VK_NULL_HANDLE)
        {
            DestroyDebugUtilsMessengerEXT(m_Instance, m_DebugMessenger, nullptr);
            m_DebugMessenger = VK_NULL_HANDLE;
        }
        
        //Destroy surface
        if (m_Surface != VK_NULL_HANDLE)
        {
            vkDestroySurfaceKHR(m_Instance, m_Surface, nullptr);
            m_Surface = VK_NULL_HANDLE;
        }
        
        //Destroy instance
        if (m_Instance != VK_NULL_HANDLE)
        {
            vkDestroyInstance(m_Instance, nullptr);
            m_Instance = VK_NULL_HANDLE;
        }
    }
    
    
    void VulkanGraphicsDevice::Init(IWindow* pWindow, const GraphicsDeviceDesc& desc)
    {
        assert(pWindow != nullptr);
        
        //Init GraphicsDevice
        if (!CreateInstance(desc)) { return; }
        if (!CreateDebugDebugMessenger(desc)) { return; }
        if (!CreateSurface(pWindow)) { return; }
        if (!QueryAdapter()) { return; }
        if (!CreateDeviceAndQueues(desc)) { return; }
        if (!CreateSemaphoresAndFences()) { return; }
        if (!CreateSwapChain(pWindow->GetWidth(), pWindow->GetHeight())) { return; }
        if (!CreateTextures()) { return; }
        if (!CreateDepthStencil()) { return; }
        if (!CreateDefaultLayouts()) { return; }
        
        //Create nullbufferdescriptor
        {
            //Create nullbuffer
            BufferDesc nullBufferDesc = {};
			nullBufferDesc.SizeInBytes    = 4;
			nullBufferDesc.Flags          = BUFFER_FLAGS_CONSTANT_BUFFER | BUFFER_FLAGS_VERTEX_BUFFER;
			nullBufferDesc.StrideInBytes  = 4;
			nullBufferDesc.Usage          = RESOURCE_USAGE_DEFAULT;
            
            m_pNullBuffer = DBG_NEW VulkanBuffer(m_Device, m_Adapter, nullBufferDesc);
            
            //Fill in bufferdescriptpr
            m_NullBufferDescriptor.buffer = reinterpret_cast<VkBuffer>(m_pNullBuffer->GetNativeHandle());
            m_NullBufferDescriptor.offset = 0;
            m_NullBufferDescriptor.range  = VK_WHOLE_SIZE;
        }
        
        //Create nulltexturedescriptor
        {
            //Create nulltexture
            Texture2DDesc nullTextureDesc = {};
			nullTextureDesc.ArraySize      = 1;
			nullTextureDesc.Flags          = TEXTURE_FLAGS_SHADER_RESOURCE;
			nullTextureDesc.Width          = 2;
			nullTextureDesc.Height         = 2;
			nullTextureDesc.Format         = FORMAT_R8G8B8A8_UNORM;
			nullTextureDesc.MipLevels      = 1;
			nullTextureDesc.SampleCount    = 1;
			nullTextureDesc.Usage          = RESOURCE_USAGE_DEFAULT;
            
            m_pNullTexture = DBG_NEW VulkanTexture2D(m_Device, m_Adapter, nullTextureDesc);
            
            //Fill in texturedescriptpr
            m_NullTextureDescriptor.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            m_NullTextureDescriptor.imageView   = m_pNullTexture->GetImageView();
            m_NullTextureDescriptor.sampler     = VK_NULL_HANDLE;
        }
        
        //Create nullsamplerdescriptor
        {
            //Create nullsampler
            SamplerDesc nullSamplerdesc = {};

            //Create sampler
            m_pNullSampler = DBG_NEW VulkanSamplerState(m_Device, nullSamplerdesc);
            
            //Fill in samplerdescriptpr
            m_NullSamplerDescriptor.imageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            m_NullSamplerDescriptor.imageView   = VK_NULL_HANDLE;
            m_NullSamplerDescriptor.sampler     = reinterpret_cast<VkSampler>(m_pNullSampler->GetNativeHandle());
        }
        
        //Init GraphicsDevice dependent members
        CreateCommandList(reinterpret_cast<ICommandList**>(&m_pCommandList), COMMAND_LIST_TYPE_GRAPHICS);
        m_pCommandList->SetName("Graphics Device Internal CommandList");
    }
    
    
    bool VulkanGraphicsDevice::CreateInstance(const GraphicsDeviceDesc& desc)
    {
        //Applicationinfo
        VkApplicationInfo info = {};
        info.sType              = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        info.pNext              = nullptr;
        info.pApplicationName   = "Lambda Engine";
        info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        info.pEngineName        = "Lambda Engine";
        info.engineVersion      = VK_MAKE_VERSION(1, 0, 0);
        info.apiVersion         = VK_API_VERSION_1_0;
        
        //Get all the available extensions
        uint32 availableExtensionsCount = 0;
        vkEnumerateInstanceExtensionProperties(nullptr, &availableExtensionsCount, nullptr);
        
        std::vector<VkExtensionProperties> availableExtensions(availableExtensionsCount);
        vkEnumerateInstanceExtensionProperties(nullptr, &availableExtensionsCount, availableExtensions.data());
        
        //Print all available extensions
        if (availableExtensions.size() > 0)
        {
            LOG_DEBUG_INFO("[Vulkan] Available Instance-Extensions:\n");
			for (const auto& extension : availableExtensions)
			{
                LOG_DEBUG_INFO("   Instance-Extension '%s'\n", extension.extensionName);
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
        
        //Print all available layers
        if (availableLayers.size() > 0)
        {
            LOG_DEBUG_INFO("[Vulkan] Available Instance-Layers:\n");
			for (const auto& layer : availableLayers)
			{
                LOG_DEBUG_INFO("   Instance-Layer '%s'\n", layer.layerName);
			}
        }
        else
        {
            LOG_DEBUG_ERROR("Vulkan: No available Instance-Layers\n");
        }
        
        //Get extensions
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
		if (desc.Flags & GRAPHICS_CONTEXT_FLAG_DEBUG)
		{
			//Pushback extensions
			requiredExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
		}
        
        //if extensioncount is more than 0, we print witch ones we need to use
        if (requiredExtensions.size() > 0)
        {
            LOG_DEBUG_INFO("[Vulkan] Required Instance-Extensions:\n");
            for (const auto& extension : requiredExtensions)
                LOG_DEBUG_INFO("   Instance-Extension '%s'\n", extension);
        }
        
		//The layer that we want
		std::vector<const char*> requiredLayers = GetRequiredValidationLayers(desc.Flags & GRAPHICS_CONTEXT_FLAG_DEBUG);

		//if layercount is more than 0, we print witch ones we need to use
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
                return false;
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
                return false;
            }
        }
        
        
        //Setup debugmessenger for instance
        VkDebugUtilsMessengerCreateInfoEXT dInfo;
        InitDebugMessengerCreateInfo(&dInfo);
        
        //Setup instance info
        VkInstanceCreateInfo iInfo = {};
        iInfo.sType                     = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        iInfo.pNext                     = (desc.Flags & GRAPHICS_CONTEXT_FLAG_DEBUG) ? (VkDebugUtilsMessengerCreateInfoEXT*)&dInfo : nullptr;
        iInfo.flags                     = 0;
        iInfo.pApplicationInfo          = &info;
        iInfo.enabledExtensionCount     = uint32(requiredExtensions.size());
        iInfo.ppEnabledExtensionNames   = requiredExtensions.data();
        iInfo.enabledLayerCount         = uint32(requiredLayers.size());
        iInfo.ppEnabledLayerNames       = requiredLayers.data();
        
        //Create instance
        VkResult res = vkCreateInstance(&iInfo, nullptr, &m_Instance);
        if (res != VK_SUCCESS)
        {
            LOG_DEBUG_ERROR("Vulkan: Failed to create Vulkan Instance. Error %d\n", res);
            
            m_Instance = VK_NULL_HANDLE;
            return false;
        }
        else
        {
            LOG_SYSTEM_PRINT ("Vulkan: Created Vulkan instance\n");
            
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
            
            return true;
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
    
    
    bool VulkanGraphicsDevice::CreateDebugDebugMessenger(const GraphicsDeviceDesc& desc)
    {
        //If not the debugflag is set we do not want to create the messenger
        if (!(desc.Flags & GRAPHICS_CONTEXT_FLAG_DEBUG))
        {
            return true;
        }
        
        assert(m_Instance != VK_NULL_HANDLE);
        
        //Setup debugmessenger
        VkDebugUtilsMessengerCreateInfoEXT info;
        InitDebugMessengerCreateInfo(&info);
        
        //Create debugmessenger
        if (CreateDebugUtilsMessengerEXT(m_Instance, &info, nullptr, &m_DebugMessenger) != VK_SUCCESS)
        {
            LOG_DEBUG_ERROR("Vulkan: Failed to create debugmessenger, maybe the extension is not present?\n");
            
            m_DebugMessenger = VK_NULL_HANDLE;
            return false;
        }
        else
        {
            LOG_DEBUG_INFO("Vulkan: Created Debugmessenger\n");
            return true;
        }
    }
    
    
    bool VulkanGraphicsDevice::QueryAdapter()
    {
        //Enumerate all adapters
        uint32 adapterCount = 0;
        vkEnumeratePhysicalDevices(m_Instance, &adapterCount, nullptr);
        if (adapterCount == 0)
        {
            LOG_DEBUG_ERROR("Vulkan: No vulkan-supported GPUs found\n");
            return false;
        }
        
        std::vector<VkPhysicalDevice> adapters(adapterCount);
        vkEnumeratePhysicalDevices(m_Instance, &adapterCount, adapters.data());
        
        LOG_DEBUG_INFO("Vulkan: Number of adapters '%d'\n", adapters.size());
        
        //Find a suitable graphics card
        int i = 0;
        for (const auto& adapter : adapters)
        {
            if (AdapterIsSuitable(adapter))
            {
                m_Adapter = adapter;
                break;
            }
            
            i++;
        }
        
        //Check if we found a suiable adapter
        if (m_Adapter == VK_NULL_HANDLE)
        {
            LOG_DEBUG_ERROR("Vulkan: Failed to find a suitable GPU\n");
            return false;
        }
        else
        {
            //Get adapterproperties
            vkGetPhysicalDeviceProperties(m_Adapter, &m_AdapterProperties);
            
            //Get familiy indices
            m_FamiliyIndices = FindQueueFamilies(m_Adapter, m_Surface);
            
            LOG_SYSTEM_PRINT("Vulkan: Selected GPU '%s'\n", m_AdapterProperties.deviceName);
            return true;
        }
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
    
    
    bool VulkanGraphicsDevice::CreateDeviceAndQueues(const GraphicsDeviceDesc& desc)
    {
        assert(m_Adapter != VK_NULL_HANDLE);

        //Find the queuefamily indices for the adapter that we have chosen, assume they are valid since queryadapter succeded.
        std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
        std::set<int32> uniqueQueueFamilies =
        {
            m_FamiliyIndices.GraphicsFamily,
            m_FamiliyIndices.PresentFamily
        };
        
        //Only one queue, we set it to have the highest priority
        float priority = 0.0f;
        for (int32 queueFamiliy : uniqueQueueFamilies)
        {
            //Setup creation of a queue
            VkDeviceQueueCreateInfo qInfo = {};
            qInfo.sType             = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            qInfo.pNext             = nullptr;
            qInfo.flags             = 0;
            qInfo.pQueuePriorities  = &priority;
            qInfo.queueFamilyIndex  = queueFamiliy;
            qInfo.queueCount        = 1;
            
            queueCreateInfos.push_back(qInfo);
        }

        //Device features we want to enable
        VkPhysicalDeviceFeatures deviceFeatures = {};
        deviceFeatures.samplerAnisotropy = VK_TRUE;
        
        //Get the required layers, assume that the layers exists because we could create the vulkaninstance
        std::vector<const char*> requiredLayers = GetRequiredValidationLayers(desc.Flags & GRAPHICS_CONTEXT_FLAG_DEBUG);
        
        //Get the required extension for device
        std::vector<const char*> deviceExtensions = GetRequiredDeviceExtensions();
        
        //Print all the available extensions
        uint32 extensionCount;
        vkEnumerateDeviceExtensionProperties(m_Adapter, nullptr, &extensionCount, nullptr);
        
        std::vector<VkExtensionProperties> availableExtensions(extensionCount);
        vkEnumerateDeviceExtensionProperties(m_Adapter, nullptr, &extensionCount, availableExtensions.data());
        
        LOG_DEBUG_INFO("[Vulkan] Available Device-Extensions:\n");
		for (const auto& extension : availableExtensions)
		{
            LOG_DEBUG_INFO("   Device-Extension '%s'\n", extension.extensionName);
		}
        
        //Print all required extensions
        LOG_DEBUG_INFO("[Vulkan] Required Device-Extensions:\n");
		for (const auto& extension : deviceExtensions)
		{
            LOG_DEBUG_INFO("   Device-Extension '%s'\n", extension);
		}
        
        //Setup the device
        VkDeviceCreateInfo info = {};
        info.sType                      = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        info.pNext                      = nullptr;
        info.flags                      = 0;
        info.enabledExtensionCount      = uint32(deviceExtensions.size());
        info.ppEnabledExtensionNames    = deviceExtensions.data();
        info.enabledLayerCount          = uint32(requiredLayers.size());
        info.ppEnabledLayerNames        = requiredLayers.data();
        info.pEnabledFeatures           = &deviceFeatures;
        info.queueCreateInfoCount       = uint32(queueCreateInfos.size());
        info.pQueueCreateInfos          = queueCreateInfos.data();
        
        //Create device
        if (vkCreateDevice(m_Adapter, &info, nullptr, &m_Device) != VK_SUCCESS)
        {
            LOG_DEBUG_ERROR("Vulkan: Failed to create device\n");
            
            m_Device = VK_NULL_HANDLE;
            return false;
        }
        else
        {
            LOG_SYSTEM_PRINT("Vulkan: Created device and retrived queues\n");
            
            //Get queues
            vkGetDeviceQueue(m_Device, m_FamiliyIndices.GraphicsFamily, 0, &m_GraphicsQueue);
            vkGetDeviceQueue(m_Device, m_FamiliyIndices.PresentFamily, 0, &m_PresentationQueue);
           
            return true;
        }
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
    
    
    bool VulkanGraphicsDevice::CreateSurface(IWindow* pWindow)
    {
#if defined(LAMBDA_PLAT_MACOS)
        GLFWwindow* pGLFWWindow = reinterpret_cast<GLFWwindow*>(pWindow->GetNativeHandle());
        if (glfwCreateWindowSurface(m_Instance, pGLFWWindow, nullptr, &m_Surface) != VK_SUCCESS)
        {
            LOG_DEBUG_ERROR("Vulkan: Failed to create SurfaceKHR\n");
            
            m_Surface = VK_NULL_HANDLE;
            return false;
        }
        else
        {
            LOG_SYSTEM_PRINT("Vulkan: Created SurfaceKHR\n");
            return true;
        }
#elif defined(LAMBDA_PLAT_WINDOWS)
		//Get native window handle for windows
		HWND hWnd = reinterpret_cast<HWND>(pWindow->GetNativeHandle());

		//Create a surface for windows
		VkWin32SurfaceCreateInfoKHR info = {};
		info.sType		= VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
		info.pNext		= nullptr;
		info.flags		= 0;
		info.hwnd		= hWnd;
		info.hinstance	= GetModuleHandle(nullptr);

		if (vkCreateWin32SurfaceKHR(m_Instance, &info, nullptr, &m_Surface) != VK_SUCCESS) 
		{
			LOG_DEBUG_ERROR("Vulkan: Failed to create SurfaceKHR\n");
			return false;
		}
		else
		{
			LOG_SYSTEM_PRINT("Vulkan: Created SurfaceKHR\n");
			return true;
		}
#endif
    }
    
    
    bool VulkanGraphicsDevice::CreateSwapChain(uint32 width, uint32 height)
    {
		VulkanSwapChainDesc desc = {};
		desc.Adapter			= m_Adapter;
		desc.Surface			= m_Surface;
		desc.SignalSemaphore	= m_ImageSemaphores[m_CurrentFrame];
		desc.Extent				= { width, height };
		desc.ImageCount			= FRAMES_AHEAD;

		m_pSwapChain = DBG_NEW VulkanSwapChain(m_Device, desc);
		return true;
    }
    
    
    bool VulkanGraphicsDevice::CreateDepthStencil()
    {
        //Create depthbuffer
        Texture2DDesc desc = {};
        desc.Usage              = RESOURCE_USAGE_DEFAULT;
        desc.Flags              = TEXTURE_FLAGS_DEPTH_STENCIL;
        desc.ArraySize          = 1;
        desc.Width              = m_pSwapChain->GetWidth();
        desc.Height             = m_pSwapChain->GetHeight();
        desc.Format             = FORMAT_D24_UNORM_S8_UINT;
        desc.SampleCount        = 1;
        desc.MipLevels          = 1;
        desc.ClearValue.Depth   = 1.0f;
        desc.ClearValue.Stencil = 0;
        
        m_pDepthStencil = DBG_NEW VulkanTexture2D(m_Device, m_Adapter, desc);
        return true;
    }
    
    
    bool VulkanGraphicsDevice::CreateTextures()
    {
        return true;
    }
    
    
    bool VulkanGraphicsDevice::CreateSemaphoresAndFences()
    {        
        //Setup semaphore structure
        VkSemaphoreCreateInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
        info.pNext = nullptr;
        info.flags = 0;
        
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
            if (vkCreateSemaphore(m_Device, &info, nullptr, &m_ImageSemaphores[i]) != VK_SUCCESS ||
				vkCreateSemaphore(m_Device, &info, nullptr, &m_RenderSemaphores[i]) != VK_SUCCESS)
            {
                LOG_DEBUG_ERROR("Vulkan: Failed to create Semaphore\n");
                return false;
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
                return false;
            }
            else
            {
                VulkanGraphicsDevice::SetVulkanObjectName(VK_OBJECT_TYPE_FENCE, (uint64)m_Fences[i], "Fence[" +  std::to_string(i) + "]");
            }
        }
        
        LOG_DEBUG_INFO("Vulkan: Created Semaphores and fences\n");
        return true;
    }
    
    
    bool VulkanGraphicsDevice::CreateDefaultLayouts()
    {
        VkShaderStageFlagBits shaderStages[] =
        {
            VK_SHADER_STAGE_VERTEX_BIT,
            VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT,
            VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT,
            VK_SHADER_STAGE_GEOMETRY_BIT,
            VK_SHADER_STAGE_FRAGMENT_BIT,
        };
        
        //Vector for keeping all the bindins for a stage
        std::vector<VkDescriptorSetLayoutBinding> layoutBindings;
        m_UniformBinding = 0;
        
        //Create descriptor bindings for uniformbuffers
        VkDescriptorSetLayoutBinding uboLayoutBinding = {};
        uboLayoutBinding.descriptorType     = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        uboLayoutBinding.descriptorCount    = 1;
        uboLayoutBinding.pImmutableSamplers = nullptr;
        for (uint32 i = 0; i < LAMBDA_SHADERSTAGE_UNIFORM_COUNT; i++)
        {
            uboLayoutBinding.binding = m_UniformBinding + i;
            layoutBindings.push_back(uboLayoutBinding);
        }
        
        //Increment offset
        m_TextureBinding = uint32(layoutBindings.size());

        //Create descriptor bindings for textures
        VkDescriptorSetLayoutBinding textureLayoutBinding = {};
        textureLayoutBinding.descriptorType     = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
        textureLayoutBinding.descriptorCount    = 1;
        textureLayoutBinding.pImmutableSamplers = nullptr;
        for (uint32 i = 0; i < LAMBDA_SHADERSTAGE_TEXTURE_COUNT; i++)
        {
            textureLayoutBinding.binding = m_TextureBinding + i;
            layoutBindings.push_back(textureLayoutBinding);
        }
        
        //Increment offset
        m_SamplerBinding = uint32(layoutBindings.size());
        
        //Create descriptor bindings for samplers
        VkDescriptorSetLayoutBinding samplerLayoutBinding = {};
        samplerLayoutBinding.descriptorType     = VK_DESCRIPTOR_TYPE_SAMPLER;
        samplerLayoutBinding.descriptorCount    = 1;
        samplerLayoutBinding.pImmutableSamplers = nullptr;
        for (uint32 i = 0; i < LAMBDA_SHADERSTAGE_SAMPLER_COUNT; i++)
        {
            samplerLayoutBinding.binding = m_SamplerBinding + i;
            layoutBindings.push_back(samplerLayoutBinding);
        }
        
        //Setup layout
        VkDescriptorSetLayoutCreateInfo descriptorLayoutInfo = {};
        descriptorLayoutInfo.sType        = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        descriptorLayoutInfo.pNext        = nullptr;
        descriptorLayoutInfo.flags        = 0;
        descriptorLayoutInfo.bindingCount = uint32(layoutBindings.size());
        descriptorLayoutInfo.pBindings    = layoutBindings.data();
        
        for (uint32 i = 0; i < LAMBDA_SHADERSTAGE_COUNT; i++)
        {
            //Set shaderstage
            for (auto& binding : layoutBindings)
                binding.stageFlags = shaderStages[i];
            
            //Create layout for shaderstage
            if (vkCreateDescriptorSetLayout(m_Device, &descriptorLayoutInfo, nullptr, &m_DefaultDescriptorSetLayouts[i]) != VK_SUCCESS)
            {
                LOG_DEBUG_ERROR("Vulkan: Failed to create default DescriptorSetLayout\n");
                return false;
            }
            else
            {
                LOG_DEBUG_INFO("Vulkan: Created default DescriptorSetLayout\n");
            }
        }

        
        //Setup pipelinelayout
        VkPipelineLayoutCreateInfo layoutInfo = {};
        layoutInfo.sType                    = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        layoutInfo.flags                    = 0;
        layoutInfo.pNext                    = nullptr;
        layoutInfo.setLayoutCount           = LAMBDA_SHADERSTAGE_COUNT;
        layoutInfo.pSetLayouts              = m_DefaultDescriptorSetLayouts;
        layoutInfo.pushConstantRangeCount   = 0;
        layoutInfo.pPushConstantRanges      = nullptr;
        
        //Create pipelinelayout
        if (vkCreatePipelineLayout(m_Device, &layoutInfo, nullptr, &m_DefaultPipelineLayout) != VK_SUCCESS)
        {
            LOG_DEBUG_ERROR("Vulkan: Failed to create default PipelineLayout\n");
            return false;
        }
        else
        {
            LOG_DEBUG_INFO("Vulkan: Created default PipelineLayout\n");
        }
        
        return true;
    }
    
    
    void VulkanGraphicsDevice::ReleaseSwapChain()
    {
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
                //Prepare commandlist
                m_pCommandList->Reset();
                
                //Update data with inital data
                m_pCommandList->UpdateBuffer(pBuffer, pInitalData);
                
                //Execute commands
                m_pCommandList->Close();
                
                VkCommandBuffer buffers[] =
                {
                    reinterpret_cast<VkCommandBuffer>(m_pCommandList->GetNativeHandle())
                };
                
                VkSubmitInfo submitInfo = {};
                submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
                submitInfo.pNext = nullptr;
                submitInfo.commandBufferCount = 1;
                submitInfo.pCommandBuffers = buffers;
                
                vkQueueSubmit(m_GraphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
                
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
            //Reset internal commandlist
            m_pCommandList->Reset();
            
            //Update texture data
            m_pCommandList->UpdateTexture(pTexture, pInitalData, 0);
            
            //Execute commands
            m_pCommandList->Close();
            VkCommandBuffer buffers[] =
            {
                reinterpret_cast<VkCommandBuffer>(m_pCommandList->GetNativeHandle())
            };
            
            VkSubmitInfo submitInfo = {};
            submitInfo.sType                = VK_STRUCTURE_TYPE_SUBMIT_INFO;
            submitInfo.pNext                = nullptr;
            submitInfo.commandBufferCount   = 1;
            submitInfo.pCommandBuffers      = buffers;
            
            vkQueueSubmit(m_GraphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
            
            //Wait until buffer is copied
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
    
    
    void VulkanGraphicsDevice::CreateSamplerState(ISamplerState** ppSamplerState, const SamplerDesc& desc) const
    {
        assert(ppSamplerState != nullptr);
        (*ppSamplerState) = DBG_NEW VulkanSamplerState(m_Device, desc);
    }
    
    
    void VulkanGraphicsDevice::CreateGraphicsPipelineState(IGraphicsPipelineState** ppPSO, const GraphicsPipelineStateDesc& desc) const
    {
        assert(ppPSO != nullptr);
        (*ppPSO) = DBG_NEW VulkanGraphicsPipelineState(m_Device, desc);
    }
    
    
    void VulkanGraphicsDevice::DestroyCommandList(ICommandList** ppList) const
    {
        assert(ppList != nullptr);
        
        //Delete list
        VulkanCommandList* pList = reinterpret_cast<VulkanCommandList*>(*ppList);
        if (pList != nullptr)
        {
            pList->Destroy(m_Device);
            
            //Set ptr to null
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
            
            //Set ptr to null
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
            
            //Set ptr to null
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

            //Set ptr to null
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
            
            //Set ptr to null
            *ppSamplerState = nullptr;
            
            LOG_DEBUG_INFO("Vulkan: Destroyed SamplerState\n");
        }
    }
    
    
    void VulkanGraphicsDevice::DestroyGraphicsPipelineState(IGraphicsPipelineState** ppPSO) const
    {
        assert(ppPSO != nullptr);
        
        //Delete shader
        VulkanGraphicsPipelineState* pPSO = reinterpret_cast<VulkanGraphicsPipelineState*>(*ppPSO);
        if (pPSO != nullptr)
        {
            pPSO->Destroy(m_Device);
            
            //Set ptr to null
            *ppPSO = nullptr;
            
            LOG_DEBUG_INFO("Vulkan: Destroyed PipelineState\n");
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
		
		m_pSwapChain->Present(m_Device, m_PresentationQueue, m_RenderSemaphores[m_CurrentFrame]);
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
    
    
    uint32 VulkanGraphicsDevice::GetCurrentSwapChainWidth() const
    {
        return m_pSwapChain->GetWidth();
    }
    
    
    uint32 VulkanGraphicsDevice::GetCurrentSwapChainHeight() const
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
            vkDeviceWaitIdle(m_Device);
            
            //Release the old SwapChain and depthbuffer
            ReleaseSwapChain();
            ReleaseDepthStencil();
            
            //Create new swapchain
            if (!CreateSwapChain(event.WindowResize.Width, event.WindowResize.Height))
            {
                LOG_DEBUG_ERROR("Vulkan: Failed to create new SwapChain\n");
                return false;
            }
            
            //Create new textures
            CreateTextures();
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
            
            if (SetDebugUtilsObjectNameEXT(VulkanGraphicsDevice::GetCurrentDevice(), &info) != VK_SUCCESS)
            {
                LOG_DEBUG_ERROR("Vulkan: Failed to set name '%s'\n", info.pObjectName);
            }
        }
    }
}
