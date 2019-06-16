#include "LambdaPch.h"
#include "VulkanGraphicsDevice.h"
#if defined(LAMBDA_PLAT_MACOS)
    #include <GLFW/glfw3.h>
#endif
#include <set>

namespace Lambda
{
    VKAPI_ATTR VkBool32 VKAPI_CALL VulkanGraphicsDevice::VulkanDebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData)
    {
        switch (messageSeverity) {
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
                LOG_DEBUG_INFO("[Vulkan Validation Layer] %s\n", pCallbackData->pMessage);
                break;
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
                LOG_DEBUG_WARNING("[Vulkan Validation Layer] %s\n", pCallbackData->pMessage);
                break;
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
                LOG_DEBUG_ERROR("[Vulkan Validation Layer] %s\n", pCallbackData->pMessage);
                break;
        }
        
        return VK_FALSE;
    }
    
    
    VulkanGraphicsDevice::VulkanGraphicsDevice(IWindow* pWindow, const GraphicsDeviceDesc& desc)
        : m_Instance(VK_NULL_HANDLE),
        m_Device(VK_NULL_HANDLE),
        m_GraphicsQueue(VK_NULL_HANDLE),
        m_PresentationQueue(VK_NULL_HANDLE),
        m_Adapter(VK_NULL_HANDLE),
        m_Surface(VK_NULL_HANDLE),
        m_DebugMessenger(VK_NULL_HANDLE)    
    {       
        assert(s_pInstance == nullptr);
        s_pInstance = this;
        
        Init(pWindow, desc);
    }
    
    
    VulkanGraphicsDevice::~VulkanGraphicsDevice()
    {
        //Destroy surface
        if (m_Surface != VK_NULL_HANDLE)
        {
            vkDestroySurfaceKHR(m_Instance, m_Surface, nullptr);
            m_Surface = VK_NULL_HANDLE;
        }
        
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
        
        //Destroy instance
        if (m_Instance != VK_NULL_HANDLE)
        {
            vkDestroyInstance(m_Instance, nullptr);
            m_Instance = VK_NULL_HANDLE;
        }
    }
    
    
    void VulkanGraphicsDevice::Init(IWindow* pWindow, const GraphicsDeviceDesc& desc)
    {
        if (!CreateInstance(desc)) { return; }
        if (!CreateDebugDebugMessenger(desc)) { return; }
        if (!CreateSurface(pWindow)) { return; }
        if (!QueryAdapter(desc)) { return; }
        if (!CreateDeviceAndQueues(desc)) { return; }
    }
    
    
    bool VulkanGraphicsDevice::CreateInstance(const GraphicsDeviceDesc& desc)
    {
        //Aplicationinfo
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
            return false;
        }
        else
        {
            LOG_DEBUG_INFO("Vulkan: Created Vulkan instance\n");
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
            return false;
        }
    }
    
    
    bool VulkanGraphicsDevice::QueryAdapter(const GraphicsDeviceDesc& desc)
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
            VkPhysicalDeviceProperties adapterProperties;
            vkGetPhysicalDeviceProperties(m_Adapter, &adapterProperties);
            
            LOG_DEBUG_INFO("Vulkan: Selected GPU '%s'\n", adapterProperties.deviceName);
            return true;
        }
    }
    
    
    bool VulkanGraphicsDevice::AdapterIsSuitable(VkPhysicalDevice adapter)
    {
        VkPhysicalDeviceProperties adapterProperties;
        vkGetPhysicalDeviceProperties(adapter, &adapterProperties);
        
        VkPhysicalDeviceFeatures adapterFeatures;
        vkGetPhysicalDeviceFeatures(adapter, &adapterFeatures);
        
        QueueFamilyIndices indices = FindQueueFamilies(adapter);
        if (!indices.Valid())
        {
            LOG_DEBUG_ERROR("Vulkan: Failed to find a suitable queuefamilies\n");
            return false;
        }
        else
        {
            LOG_DEBUG_INFO("Vulkan: Using queueFamily-Index '%d' for graphics and '%d' for presentation\n", indices.GraphicsFamily, indices.PresentFamily);
            return true;
        }
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
        QueueFamilyIndices indices = FindQueueFamilies(m_Adapter);
        std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
        std::set<int32> uniqueQueueFamilies =
        {
            indices.GraphicsFamily,
            indices.PresentFamily
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
        
        //Setup the device
        VkDeviceCreateInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        info.pNext = nullptr;
        info.flags = 0;
        info.enabledExtensionCount = 0;
        info.ppEnabledExtensionNames = nullptr;
        info.enabledLayerCount = uint32(requiredLayers.size());
        info.ppEnabledLayerNames = requiredLayers.data();
        info.pEnabledFeatures = &deviceFeatures;
        info.queueCreateInfoCount = uint32(queueCreateInfos.size());
        info.pQueueCreateInfos = queueCreateInfos.data();
        
        //Create device
        if (vkCreateDevice(m_Adapter, &info, nullptr, &m_Device) != VK_SUCCESS)
        {
            LOG_DEBUG_ERROR("Vulkan: Failed to create device\n");
            return false;
        }
        else
        {
            LOG_DEBUG_INFO("Vulkan: Created device and retrived queue\n");
            
            //Get queues
            vkGetDeviceQueue(m_Device, indices.GraphicsFamily, 0, &m_GraphicsQueue);
            vkGetDeviceQueue(m_Device, indices.PresentFamily, 0, &m_PresentationQueue);
            
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
    
    
    bool VulkanGraphicsDevice::CreateSurface(IWindow* pWindow)
    {
#if defined(LAMBDA_PLAT_MACOS)
        GLFWwindow* pGLFWWindow = reinterpret_cast<GLFWwindow*>(pWindow->GetNativeHandle());
        if (glfwCreateWindowSurface(m_Instance, pGLFWWindow, nullptr, &m_Surface) != VK_SUCCESS)
        {
            LOG_DEBUG_ERROR("Vulkan: Failed to create SurfaceKHR\n");
            return false;
        }
        else
        {
            LOG_DEBUG_INFO("Vulkan: Created SurfaceKHR\n");
            return true;
        }
#endif
    }
    
    
    void VulkanGraphicsDevice::CreateCommandList(ICommandList** ppList, CommandListType type) const
    {
        
    }
    
    
    void VulkanGraphicsDevice::CreateBuffer(IBuffer** ppBuffer, const ResourceData* pInitalData, const BufferDesc& desc) const
    {
        
    }
    
    
    void VulkanGraphicsDevice::CreateTexture2D(ITexture2D** ppTexture, const ResourceData* pInitalData, const Texture2DDesc& desc) const
    {
        
    }
    
    
    void VulkanGraphicsDevice::CreateShader(IShader** ppShader, const ShaderDesc& desc) const
    {
        
    }
    
    
    void VulkanGraphicsDevice::CreateSamplerState(ISamplerState** ppSamplerState, const SamplerDesc& desc) const
    {
        
    }
    
    
    void VulkanGraphicsDevice::CreateGraphicsPipelineState(IGraphicsPipelineState** ppPSO, const GraphicsPipelineStateDesc& desc) const
    {
        
    }
    
    
    
    void VulkanGraphicsDevice::DestroyCommandList(ICommandList** ppList) const
    {
        
    }
    
    
    void VulkanGraphicsDevice::DestroyBuffer(IBuffer** ppBuffer) const
    {
        
    }
    
    
    void VulkanGraphicsDevice::DestroyTexture2D(ITexture2D** ppTexture) const
    {
        
    }
    
    
    void VulkanGraphicsDevice::DestroyShader(IShader** ppShader) const
    {
        
    }
    
    
    void VulkanGraphicsDevice::DestroySamplerState(ISamplerState** ppSamplerState) const
    {
        
    }
    
    
    void VulkanGraphicsDevice::DestroyGraphicsPipelineState(IGraphicsPipelineState** ppPSO) const
    {
        
    }
    
    
    void VulkanGraphicsDevice::Destroy() const
    {
		delete this;
    }
    
    
    void VulkanGraphicsDevice::ExecuteCommandList(ICommandList* const * ppLists, uint32 numLists) const
    {
        
    }
    
    
    void VulkanGraphicsDevice::Present(uint32 verticalSync) const
    {
        
    }
    
    
    void VulkanGraphicsDevice::GPUWaitForFrame() const
    {
        
    }
    
    
    void VulkanGraphicsDevice::WaitForGPU() const
    {
        
    }

    
    void* VulkanGraphicsDevice::GetNativeHandle() const
    {
        return nullptr;
    }
    
    
    ITexture2D* VulkanGraphicsDevice::GetCurrentRenderTarget()
    {
        return nullptr;
    }
    
    
    uint32 VulkanGraphicsDevice::GetCurrentBackBufferIndex() const
    {
        return 0;
    }
    
    
    bool VulkanGraphicsDevice::InternalOnEvent(const Event& event)
    {
        if (event.Type == EVENT_TYPE_WINDOW_RESIZE)
        {
            //Resize swapchain etc. here
            LOG_DEBUG_INFO("VulkanGraphicsDevice: Window resized w: %d h: %d\n", event.WindowResize.Width, event.WindowResize.Height);
        }
        
        return false;
    }
}
