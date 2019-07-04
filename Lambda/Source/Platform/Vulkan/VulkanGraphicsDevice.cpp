#include "LambdaPch.h"
#include "VulkanGraphicsDevice.h"
#include "VulkanShader.h"
#include "VulkanPipelineState.h"
#include "VulkanTexture2D.h"
#include "VulkanCommandList.h"
#include "VulkanFramebufferCache.h"
#include "VulkanBuffer.h"
#include "VulkanConversions.inl"
#if defined(LAMBDA_PLAT_MACOS)
    #include <GLFW/glfw3.h>
#endif
#include <set>

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
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:      severity = LOG_SEVERITY_INFO; break;
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:   severity = LOG_SEVERITY_WARNING; break;
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:     severity = LOG_SEVERITY_ERROR; break;
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
    PFN_vkSetDebugUtilsObjectNameEXT VulkanGraphicsDevice::SetDebugUtilsObjectNameEXT = nullptr;
    
    //Vulkan Graphics Context
    VulkanGraphicsDevice::VulkanGraphicsDevice(IWindow* pWindow, const GraphicsDeviceDesc& desc)
        : m_Instance(VK_NULL_HANDLE),
        m_DebugMessenger(VK_NULL_HANDLE),
        m_Device(VK_NULL_HANDLE),
        m_GraphicsQueue(VK_NULL_HANDLE),
        m_PresentationQueue(VK_NULL_HANDLE),
        m_ImageSemaphores(),
        m_RenderSemaphores(),
        m_FamiliyIndices(),
        m_Adapter(VK_NULL_HANDLE),
        m_AdapterProperties(),
        m_Surface(VK_NULL_HANDLE),
        m_SwapChain(VK_NULL_HANDLE),
        m_SwapChainFormat(),
        m_SwapChainSize(),
        m_CurrentFrame(0),
        m_CurrentBackbufferIndex(0),
        m_BackBuffers()
    {       
        assert(s_pInstance == nullptr);
        s_pInstance = this;
        
        //Init default descriptorsetlayouts
        for (uint32 i = 0; i < LAMBDA_SHADERSTAGE_COUNT; i++)
            m_DefaultDescriptorSetLayouts[i] = VK_NULL_HANDLE;
        
        //Init semaphores and fences
        for (uint32 i = 0; i < FRAMES_AHEAD; i++)
        {
            m_ImageSemaphores[i] = VK_NULL_HANDLE;
            m_RenderSemaphores[i] = VK_NULL_HANDLE;
            m_Fences[i] = VK_NULL_HANDLE;
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

        
        //Destroy all framebuffers
        VulkanFramebufferCache::Release(m_Device);
        
        //Destroy swapchain and related resources
        ReleaseSwapChain();
        
        //Destroy device
        if (m_Device != VK_NULL_HANDLE)
        {
            vkDestroyDevice(m_Device, nullptr);
            m_Device = VK_NULL_HANDLE;
        }

        //Destroy debugmessenger
        if (m_DebugMessenger != VK_NULL_HANDLE)
        {
            auto destroyDebugUtilsMessengerEXT = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(m_Instance, "vkDestroyDebugUtilsMessengerEXT");
            if (destroyDebugUtilsMessengerEXT != nullptr)
            {
                destroyDebugUtilsMessengerEXT(m_Instance, m_DebugMessenger, nullptr);
                m_DebugMessenger = VK_NULL_HANDLE;
            }
            else
            {
                LOG_DEBUG_ERROR("Vulkan: Failed to find the function 'vkDestroyDebugUtilsMessengerEXT'\n");
            }
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
        if (!CreateDefaultLayouts()) { return; }
        
        //Create nulldescriptor
        {
            //Create nullbuffer
            BufferDesc desc = {};
            desc.SizeInBytes    = 4;
            desc.Flags          = BUFFER_FLAGS_CONSTANT_BUFFER | BUFFER_FLAGS_VERTEX_BUFFER;
            desc.StrideInBytes  = 4;
            desc.Usage          = RESOURCE_USAGE_DEFAULT;
            
            m_pNullBuffer = DBG_NEW VulkanBuffer(m_Device, m_Adapter, desc);
            
            //Fill in bufferdescriptpr
            m_NullBufferDescriptor.buffer = reinterpret_cast<VkBuffer>(m_pNullBuffer->GetNativeHandle());
            m_NullBufferDescriptor.offset = 0;
            m_NullBufferDescriptor.range  = VK_WHOLE_SIZE;
        }
        
        //Init GraphicsDevice dependent members
        CreateCommandList(reinterpret_cast<ICommandList**>(&m_pCommandList), COMMAND_LIST_TYPE_GRAPHICS);
        if (m_pCommandList)
        {
            m_pCommandList->SetName("Graphics Device Internal CommandList");
        }
    }
    
    
    bool VulkanGraphicsDevice::CreateInstance(const GraphicsDeviceDesc& desc)
    {
        //Applicationinfo
        VkApplicationInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        info.pNext = nullptr;
        info.pApplicationName = "Lambda Engine";
        info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        info.pEngineName = "Lambda Engine";
        info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        info.apiVersion = VK_API_VERSION_1_0;
        
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
                LOG_DEBUG_INFO("   Instance-Extension '%s'\n", extension.extensionName);
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
                LOG_DEBUG_INFO("   Instance-Layer '%s'\n", layer.layerName);
        }
        else
        {
            LOG_DEBUG_ERROR("Vulkan: No available Instance-Layers\n");
        }
        
        //Get extensions
        uint32 extensionCount = 0;
        const char** ppExtensions = nullptr;
#if defined(LAMBDA_PLAT_MACOS)
        //Get extensions on macOS
        ppExtensions = glfwGetRequiredInstanceExtensions(&extensionCount);
#endif
        
        //The layers and extensions that we want
        std::vector<const char*> requiredLayers = GetRequiredValidationLayers(desc.Flags & GRAPHICS_CONTEXT_FLAG_DEBUG);
        std::vector<const char*> requiredExtensions(ppExtensions, ppExtensions + extensionCount);
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
        iInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        iInfo.pNext = (desc.Flags & GRAPHICS_CONTEXT_FLAG_DEBUG) ? (VkDebugUtilsMessengerCreateInfoEXT*)&dInfo : nullptr;
        iInfo.flags = 0;
        iInfo.pApplicationInfo = &info;
        iInfo.enabledExtensionCount = uint32(requiredExtensions.size());
        iInfo.ppEnabledExtensionNames = requiredExtensions.data();
        iInfo.enabledLayerCount = uint32(requiredLayers.size());
        iInfo.ppEnabledLayerNames = requiredLayers.data();
        
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
            LOG_DEBUG_INFO("Vulkan: Created Vulkan instance\n");
            
            //Get instance functions
            SetDebugUtilsObjectNameEXT = (PFN_vkSetDebugUtilsObjectNameEXT)vkGetInstanceProcAddr(m_Instance, "vkSetDebugUtilsObjectNameEXT");
            if (!SetDebugUtilsObjectNameEXT)
            {
                LOG_DEBUG_ERROR("Vulkan: Failed to retrive 'vkSetDebugUtilsObjectNameEXT'");
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
        auto createDebugUtilsMessengerEXT = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(m_Instance, "vkCreateDebugUtilsMessengerEXT");
        if (createDebugUtilsMessengerEXT != nullptr)
        {
            if (createDebugUtilsMessengerEXT(m_Instance, &info, nullptr, &m_DebugMessenger) != VK_SUCCESS)
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
        else
        {
            LOG_DEBUG_ERROR("Vulkan: Failed to find function 'vkCreateDebugUtilsMessengerEXT'\n");
            
            m_DebugMessenger = VK_NULL_HANDLE;
            return false;
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
            m_FamiliyIndices = FindQueueFamilies(m_Adapter);
            
            LOG_DEBUG_INFO("Vulkan: Selected GPU '%s'\n", m_AdapterProperties.deviceName);
            return true;
        }
    }
    
    
    bool VulkanGraphicsDevice::AdapterIsSuitable(VkPhysicalDevice adapter)
    {
        VkPhysicalDeviceProperties adapterProperties;
        vkGetPhysicalDeviceProperties(adapter, &adapterProperties);
        
        VkPhysicalDeviceFeatures adapterFeatures;
        vkGetPhysicalDeviceFeatures(adapter, &adapterFeatures);
        
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
        
        //Check if the swapchain supports whats required
        SwapChainCapabilities swapChainInfo = QuerySwapChainSupport(adapter);
        if (!swapChainInfo.Valid())
        {
            LOG_DEBUG_ERROR("Vulkan: Adapter does not have valid SwapChain support\n");
            return false;
        }
        
        return true;
    }
    
    
    QueueFamilyIndices VulkanGraphicsDevice::FindQueueFamilies(VkPhysicalDevice adapter)
    {
        //Get all queuefamilies for this adapter
        uint32 queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(adapter, &queueFamilyCount, nullptr);
        
        std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(adapter, &queueFamilyCount, queueFamilies.data());
        
        //Debug print the number of queuefamilies
        LOG_DEBUG_INFO("Vulkan: Number of queuefamilies on adapter '%d'\n", queueFamilyCount);
        
        //Find a queuefamily that supports
        int32 i = 0;
        QueueFamilyIndices indices;
        for (const auto& queueFamiliy : queueFamilies)
        {
            //Check for graphics support
            if (queueFamiliy.queueCount > 0 && queueFamiliy.queueFlags & VK_QUEUE_GRAPHICS_BIT)
            {
                indices.GraphicsFamily = i;
            }
            
            //Check for presentationsupport
            VkBool32 presentSupport = false;
            vkGetPhysicalDeviceSurfaceSupportKHR(adapter, i, m_Surface, &presentSupport);
            if (queueFamiliy.queueCount > 0 && presentSupport)
            {
                indices.PresentFamily = i;
            }
            
            //If we have found a queuefamiliy for both presentation and graphics we break
            if (indices.Valid())
            {
                break;
            }
            
            i++;
        }
        
        return indices;
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
            qInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            qInfo.pNext = nullptr;
            qInfo.flags = 0;
            qInfo.pQueuePriorities = &priority;
            qInfo.queueFamilyIndex = queueFamiliy;
            qInfo.queueCount = 1;
            
            queueCreateInfos.push_back(qInfo);
        }

        //Device features we want to enable
        VkPhysicalDeviceFeatures deviceFeatures = {};
        
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
            LOG_DEBUG_INFO("   Device-Extension '%s'\n", extension.extensionName);
        
        //Print all required extensions
        LOG_DEBUG_INFO("[Vulkan] Required Device-Extensions:\n");
        for (const auto& extension : deviceExtensions)
            LOG_DEBUG_INFO("   Device-Extension '%s'\n", extension);
        
        //Setup the device
        VkDeviceCreateInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        info.pNext = nullptr;
        info.flags = 0;
        info.enabledExtensionCount = uint32(deviceExtensions.size());
        info.ppEnabledExtensionNames = deviceExtensions.data();
        info.enabledLayerCount = uint32(requiredLayers.size());
        info.ppEnabledLayerNames = requiredLayers.data();
        info.pEnabledFeatures = &deviceFeatures;
        info.queueCreateInfoCount = uint32(queueCreateInfos.size());
        info.pQueueCreateInfos = queueCreateInfos.data();
        
        //Create device
        if (vkCreateDevice(m_Adapter, &info, nullptr, &m_Device) != VK_SUCCESS)
        {
            LOG_DEBUG_ERROR("Vulkan: Failed to create device\n");
            
            m_Device = VK_NULL_HANDLE;
            return false;
        }
        else
        {
            LOG_DEBUG_INFO("Vulkan: Created device and retrived queues\n");
            
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
            LOG_DEBUG_INFO("Vulkan: Created SurfaceKHR\n");
            return true;
        }
#endif
    }
    
    
    SwapChainCapabilities VulkanGraphicsDevice::QuerySwapChainSupport(VkPhysicalDevice adapter)
    {
        SwapChainCapabilities info;
        
        //Get capabilities from the surface
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(adapter, m_Surface, &info.Capabilities);
        
        //Get supported surface formats
        uint32 formatCount = 0;
        vkGetPhysicalDeviceSurfaceFormatsKHR(adapter, m_Surface, &formatCount, nullptr);
        
        if (formatCount != 0)
        {
            info.Formats.resize(formatCount);
            vkGetPhysicalDeviceSurfaceFormatsKHR(adapter, m_Surface, &formatCount, info.Formats.data());
        }
        
        //Get presentation modes
        uint32 presentModeCount = 0;
        vkGetPhysicalDeviceSurfacePresentModesKHR(adapter, m_Surface, &presentModeCount, nullptr);
        
        if (presentModeCount != 0)
        {
            info.PresentModes.resize(presentModeCount);
            vkGetPhysicalDeviceSurfacePresentModesKHR(adapter, m_Surface, &presentModeCount, info.PresentModes.data());
        }
        
        return info;
    }
    
    
    bool VulkanGraphicsDevice::CreateSwapChain(uint16 width, uint16 height)
    {
        SwapChainCapabilities cap = QuerySwapChainSupport(m_Adapter);
        
        //Choose a swapchain format
        VkSurfaceFormatKHR format = cap.Formats[0];
        for (const auto& availableFormat : cap.Formats)
        {
            if (availableFormat.format == VK_FORMAT_B8G8R8A8_UNORM && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
            {
                format = availableFormat;
                break;
            }
        }
        
        LOG_DEBUG_INFO("Vulkan: Chosen SwapChain format '%s'\n", VkFormatToString(format.format));
        
        //Choose a presentationmode
        VkPresentModeKHR presentationMode = VK_PRESENT_MODE_FIFO_KHR;
        for (const auto& availablePresentMode : cap.PresentModes)
        {
            if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
            {
                presentationMode = availablePresentMode;
                break;
            }
            else if (availablePresentMode == VK_PRESENT_MODE_IMMEDIATE_KHR)
            {
                presentationMode = availablePresentMode;
            }
        }
        
        LOG_DEBUG_INFO("Vulkan: Chosen SwapChain PresentationMode '%d'\n", presentationMode);
        
        //Choose swapchain extent (Size)
        VkExtent2D extent;
        if (cap.Capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
        {
            extent = cap.Capabilities.currentExtent;
        }
        else
        {
            VkExtent2D actualExtent = { width, height };
            
            actualExtent.width = std::max(cap.Capabilities.minImageExtent.width, std::min(cap.Capabilities.maxImageExtent.width, actualExtent.width));
            actualExtent.height = std::max(cap.Capabilities.minImageExtent.height, std::min(cap.Capabilities.maxImageExtent.height, actualExtent.height));
            
            extent = actualExtent;
        }
        
        LOG_DEBUG_INFO("Vulkan: Chosen SwapChain Extent w: %u h: %u\n", extent.width, extent.height);
        
        //Setup swapchain images
        uint32 imageCount = cap.Capabilities.minImageCount + 1;
        if (cap.Capabilities.maxImageCount > 0 && imageCount > cap.Capabilities.maxImageCount)
            imageCount = cap.Capabilities.maxImageCount;
        
        LOG_DEBUG_INFO("Vulkan: Number of images in SwapChain '%u'\n", imageCount);
        
        
        //Setup swapchain
        VkSwapchainCreateInfoKHR info = {};
        info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        info.pNext = nullptr;
        info.surface = m_Surface;
        info.minImageCount = imageCount;
        info.imageFormat = format.format;
        info.imageColorSpace = format.colorSpace;
        info.imageExtent = extent;
        info.imageArrayLayers = 1;
        info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT; //Use as color attachment and clear
        info.preTransform = cap.Capabilities.currentTransform;
        info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        info.presentMode = presentationMode;
        info.clipped = VK_TRUE;
        info.oldSwapchain = VK_NULL_HANDLE;
        
        QueueFamilyIndices indices = FindQueueFamilies(m_Adapter);
        uint32 queueFamilyIndices[] = { uint32(indices.GraphicsFamily), uint32(indices.PresentFamily) };
        
        if (indices.GraphicsFamily != indices.PresentFamily)
        {
            info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            info.queueFamilyIndexCount = 2;
            info.pQueueFamilyIndices = queueFamilyIndices;
        }
        else
        {
            info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
            info.queueFamilyIndexCount = 0; // Optional
            info.pQueueFamilyIndices = nullptr; // Optional
        }
        
        //Create swapchain
        if (vkCreateSwapchainKHR(m_Device, &info, nullptr, &m_SwapChain) != VK_SUCCESS)
        {
            LOG_DEBUG_ERROR("Vulkan: Failed to create SwapChain\n");
            
            m_SwapChain = VK_NULL_HANDLE;
            return false;
        }
        else
        {
            LOG_DEBUG_INFO("Vulkan: Created SwapChain\n");
            
            //Save the extent and formats
            m_SwapChainFormat = format.format;
            m_SwapChainSize = extent;
            return true;
        }
    }
    
    
    bool VulkanGraphicsDevice::CreateTextures()
    {
        //Get SwapChain images
        uint32 imageCount = 0;
        vkGetSwapchainImagesKHR(m_Device, m_SwapChain, &imageCount, nullptr);
    
        std::vector<VkImage> textures(imageCount);
        vkGetSwapchainImagesKHR(m_Device, m_SwapChain, &imageCount, textures.data());
    
        //Init textures
        for (uint32 i = 0; i < imageCount; i++)
        {
            OptimizedClearValue clearValue = {};
            clearValue.Color[0] = 0.0f;
            clearValue.Color[1] = 0.0f;
            clearValue.Color[2] = 0.0f;
            clearValue.Color[3] = 0.0f;
            
            uint32 flags = TEXTURE_FLAGS_RENDER_TARGET;
            
            //Create texture
            VulkanTexture2D* pTexture = DBG_NEW VulkanTexture2D(textures[i], m_SwapChainSize, ConvertVkFormat(m_SwapChainFormat), clearValue, flags, 0, 1);
            m_BackBuffers.push_back(pTexture);
            
            //Setup image views
            VkImageViewCreateInfo info = {};
            info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            info.flags = 0;
            info.image = reinterpret_cast<VkImage>(pTexture->GetNativeHandle());
            info.viewType = VK_IMAGE_VIEW_TYPE_2D;
            info.format = m_SwapChainFormat;
            info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
            info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
            info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
            info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
            info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            info.subresourceRange.baseMipLevel = 0;
            info.subresourceRange.levelCount = 1;
            info.subresourceRange.baseArrayLayer = 0;
            info.subresourceRange.layerCount = 1;
            
            //Create image views
            VkImageView view;
            if (vkCreateImageView(m_Device, &info, nullptr, &view) != VK_SUCCESS)
            {
                LOG_DEBUG_ERROR("Vulkan: Failed to create ImageView '%u'\n", i);
                
                view = VK_NULL_HANDLE;
                return false;
            }
            else
            {
                //Set the imageview
                pTexture->SetImageView(view);
            }
        }
        
        //Aquire the first swapchain image
        m_CurrentFrame = 0;
        GetNextFrame();
        
        LOG_DEBUG_INFO("Vulkan: Created ImageViews\n");
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

        for (uint32 i = 0; i < FRAMES_AHEAD; i++)
        {
            //Create semaphores
            if (vkCreateSemaphore(m_Device, &info, nullptr, &m_ImageSemaphores[i]) != VK_SUCCESS ||
                vkCreateSemaphore(m_Device, &info, nullptr, &m_RenderSemaphores[i]) != VK_SUCCESS)
            {
                LOG_DEBUG_ERROR("Vulkan: Failed to create Semaphores\n");
                return false;
            }
            else
            {
                //Set semaphorenames
                VulkanGraphicsDevice::SetVulkanObjectName(VK_OBJECT_TYPE_SEMAPHORE, (uint64)m_ImageSemaphores[i], "ImageSemaphore[" +  std::to_string(i) + "]");
                VulkanGraphicsDevice::SetVulkanObjectName(VK_OBJECT_TYPE_SEMAPHORE, (uint64)m_RenderSemaphores[i], "RenderSemaphore[" +  std::to_string(i) + "]");
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
        
        //Create descriptor layout
        VkDescriptorSetLayoutBinding uboLayoutBinding = {};
        uboLayoutBinding.descriptorType     = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        uboLayoutBinding.binding            = 0;
        uboLayoutBinding.descriptorCount    = LAMBDA_SHADERSTAGE_UNIFORM_COUNT;
        uboLayoutBinding.pImmutableSamplers = nullptr;
        
        VkDescriptorSetLayoutCreateInfo descriptorLayoutInfo = {};
        descriptorLayoutInfo.sType        = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        descriptorLayoutInfo.pNext        = nullptr;
        descriptorLayoutInfo.flags        = 0;
        descriptorLayoutInfo.bindingCount = 1;
        descriptorLayoutInfo.pBindings    = &uboLayoutBinding;
        
        for (uint32 i = 0; i < LAMBDA_SHADERSTAGE_COUNT; i++)
        {
            //Set shaderstage
            uboLayoutBinding.stageFlags = shaderStages[i];
            
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
        //Release backbuffers
        for (size_t i = 0; i < m_BackBuffers.size(); i++)
        {
            if (m_BackBuffers[i])
            {
                m_BackBuffers[i]->Destroy(m_Device);
                m_BackBuffers[i] = nullptr;
            }
        }
        
        //Clear backbuffer array
        m_BackBuffers.clear();
        
        //Release swapchain
        if (m_SwapChain != VK_NULL_HANDLE)
        {
            vkDestroySwapchainKHR(m_Device, m_SwapChain, nullptr);
            m_SwapChain = VK_NULL_HANDLE;
        }
    }
    
    
    void VulkanGraphicsDevice::GetNextFrame() const
    {
        //Advance current frame counter
        m_CurrentFrame = (m_CurrentFrame + 1) % FRAMES_AHEAD;
        
        //Aquire the next swapchain image
        vkAcquireNextImageKHR(m_Device, m_SwapChain, std::numeric_limits<uint64>::max(), m_ImageSemaphores[m_CurrentFrame], VK_NULL_HANDLE, &m_CurrentBackbufferIndex);
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
    }
    
    
    void VulkanGraphicsDevice::CreateShader(IShader** ppShader, const ShaderDesc& desc) const
    {
        assert(ppShader != nullptr);
        (*ppShader) = DBG_NEW VulkanShader(m_Device, desc);
    }
    
    
    void VulkanGraphicsDevice::CreateSamplerState(ISamplerState** ppSamplerState, const SamplerDesc& desc) const
    {
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
		delete this;
    }
    
    
    void VulkanGraphicsDevice::ExecuteCommandList(ICommandList* const * ppLists, uint32 numLists) const
    {
        //LOG_DEBUG_INFO("Vulkan: VulkanGraphicsDevice::ExecuteCommandList currentFrame=%u\n", m_CurrentFrame);
        
        //Retrive commandbuffers
        std::vector<VkCommandBuffer> buffers;
        for (uint32 i = 0; i < numLists; i++)
            buffers.push_back(reinterpret_cast<VkCommandBuffer>(ppLists[i]->GetNativeHandle()));
        
        //Setup submitinfo
        VkSemaphore waitSemaphores[]        = { m_ImageSemaphores[m_CurrentFrame] };
        VkSemaphore signalSemaphores[]      = { m_RenderSemaphores[m_CurrentFrame] };
        VkPipelineStageFlags waitStages[]   = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
        
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
    }
    
    
    void VulkanGraphicsDevice::Present(uint32 verticalSync) const
    {
        //Setup presentinfo
        VkSemaphore signalSemaphores[] = { m_RenderSemaphores[m_CurrentFrame] };
        VkSwapchainKHR swapChains[] = { m_SwapChain };
        
        VkPresentInfoKHR info = {};
        info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        info.pNext = nullptr;
        info.waitSemaphoreCount = 1;
        info.pWaitSemaphores = signalSemaphores;
        info.swapchainCount = 1;
        info.pSwapchains = swapChains;
        info.pImageIndices = &m_CurrentBackbufferIndex;
        info.pResults = nullptr;
        
        vkQueuePresentKHR(m_PresentationQueue, &info);
        
        //LOG_DEBUG_INFO("Vulkan: Present\n");
    }
    
    
    void VulkanGraphicsDevice::GPUWaitForFrame() const
    {
        //Wait for last frame
        vkWaitForFences(m_Device, 1, &m_Fences[m_CurrentFrame], VK_TRUE, std::numeric_limits<uint64_t>::max());
        vkResetFences(m_Device, 1, &m_Fences[m_CurrentFrame]);
        
        //LOG_DEBUG_INFO("VulkanGraphicsDevice::GPUWaitForFrame, currentframe=%u\n", m_CurrentFrame);
        
        GetNextFrame();
    }
    
    
    void VulkanGraphicsDevice::WaitForGPU() const
    {
        //LOG_DEBUG_INFO("VulkanGraphicsDevice::WaitForGPU\n");
        vkQueueWaitIdle(m_GraphicsQueue);
    }

    
    void* VulkanGraphicsDevice::GetNativeHandle() const
    {
        return (void*)m_Device;
    }
    
    
    ITexture2D* VulkanGraphicsDevice::GetCurrentRenderTarget()
    {
        return m_BackBuffers[GetCurrentBackBufferIndex()];
    }
    
    
    uint32 VulkanGraphicsDevice::GetCurrentBackBufferIndex() const
    {
        return m_CurrentBackbufferIndex;
    }
    
    
    bool VulkanGraphicsDevice::InternalOnEvent(const Event& event)
    {
        //Handle resize event
        if (event.Type == EVENT_TYPE_WINDOW_RESIZE)
        {
            //When we minimize or any other reason the size is zero
            //Do not resize if the size is the same as the current one
            if ((event.WindowResize.Width == 0 || event.WindowResize.Height == 0) || (event.WindowResize.Width == m_SwapChainSize.width && event.WindowResize.Height == m_SwapChainSize.height))
            {
                return false;
            }
            
            //Syncronize the GPU so no operations are in flight when recreating swapchain
            vkDeviceWaitIdle(m_Device);
            
            //Release the old SwapChain
            ReleaseSwapChain();
            
            //Create new swapchain
            if (!CreateSwapChain(event.WindowResize.Width, event.WindowResize.Height))
            {
                LOG_DEBUG_ERROR("Vulkan: Failed to create new SwapChain\n");
                return false;
            }
            
            //Create new backbuffer textures
            CreateTextures();
            
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
