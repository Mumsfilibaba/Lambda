#pragma once
#include "Graphics/IGraphicsDevice.h"
#include <vulkan/vulkan.h>

#define FRAMES_AHEAD 3

namespace Lambda
{
    //Helperstruct for when finding queuefamilies
    struct QueueFamilyIndices
    {
        int32 GraphicsFamily    = -1;
        int32 PresentFamily     = -1;
        
        inline bool Valid()
        {
            return (GraphicsFamily >= 0) && (PresentFamily >= 0);
        }
    };
    
    
    //SwapChain support helper struct
    struct SwapChainCapabilities
    {
        VkSurfaceCapabilitiesKHR Capabilities;
        std::vector<VkSurfaceFormatKHR> Formats;
        std::vector<VkPresentModeKHR> PresentModes;
        
        inline bool Valid()
        {
            return !Formats.empty() && !PresentModes.empty() && Capabilities.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_DST_BIT;
        }
    };
    
    
    //Forward declarations
    class VulkanBuffer;
    class VulkanTexture2D;
    class VulkanCommandList;
    class VulkanSamplerState;
    
    
    //Vulkan implementation of graphics device
    class VulkanGraphicsDevice final : public IGraphicsDevice
    {
    public:
        LAMBDA_NO_COPY(VulkanGraphicsDevice);
        
        VulkanGraphicsDevice(IWindow* pWindow, const GraphicsDeviceDesc& desc);
        ~VulkanGraphicsDevice();
        
        virtual void CreateCommandList(ICommandList** ppList, CommandListType type) const override final;
        virtual void CreateBuffer(IBuffer** ppBuffer, const ResourceData* pInitalData, const BufferDesc& desc) const override final;
        virtual void CreateTexture2D(ITexture2D** ppTexture, const ResourceData* pInitalData, const Texture2DDesc& desc) const override final;
        virtual void CreateShader(IShader** ppShader, const ShaderDesc& desc) const override final;
        virtual void CreateSamplerState(ISamplerState** ppSamplerState, const SamplerDesc& desc) const override final;
        virtual void CreateGraphicsPipelineState(IGraphicsPipelineState** ppPSO, const GraphicsPipelineStateDesc& desc) const override final;
        
        virtual void DestroyCommandList(ICommandList** ppList) const override final;
        virtual void DestroyBuffer(IBuffer** ppBuffer) const override final;
        virtual void DestroyTexture2D(ITexture2D** ppTexture) const override final;
        virtual void DestroyShader(IShader** ppShader) const override final;
        virtual void DestroySamplerState(ISamplerState** ppSamplerState) const override final;
        virtual void DestroyGraphicsPipelineState(IGraphicsPipelineState** ppPSO) const override final;
        virtual void Destroy() const override final;
        
        virtual void ExecuteCommandList(ICommandList* const * ppLists, uint32 numLists) const override final;
        virtual void Present(uint32 verticalSync) const override final;
        virtual void GPUWaitForFrame() const override final;
        virtual void WaitForGPU() const override final;
        
        virtual void* GetNativeHandle() const override final;
        virtual ITexture2D* GetCurrentRenderTarget() override final;
        virtual uint32 GetCurrentBackBufferIndex() const override final;
    
        VkFormat GetBackBufferFormat() const;
        VkPhysicalDevice GetAdapter() const;
        QueueFamilyIndices GetQueueFamilyIndices() const;
        
    private:
        void Init(IWindow* pWindow, const GraphicsDeviceDesc& desc);
        void InitDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo);
        
        bool CreateInstance(const GraphicsDeviceDesc& desc);
        bool CreateDebugDebugMessenger(const GraphicsDeviceDesc& desc);
        bool QueryAdapter();
        bool CreateDeviceAndQueues(const GraphicsDeviceDesc& desc);
        bool CreateSurface(IWindow* pWindow);
        bool CreateSwapChain(uint16 width, uint16 height);
        bool CreateTextures();
        bool CreateSemaphoresAndFences();
        bool CreateDefaultLayouts();
        
        void ReleaseSwapChain();
        
        void GetNextFrame() const;
        
        bool AdapterIsSuitable(VkPhysicalDevice adapter);
        SwapChainCapabilities QuerySwapChainSupport(VkPhysicalDevice adapter);
        QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice adapter); //Returns -1 on failure, otherwise index of queuefamily
        
        std::vector<const char*> GetRequiredValidationLayers(bool debug);
        std::vector<const char*> GetRequiredDeviceExtensions();
        
        virtual bool InternalOnEvent(const Event& event) override final;
        
    private:
        VkInstance m_Instance;
        VkDebugUtilsMessengerEXT m_DebugMessenger;
        VkDevice m_Device;
        VkQueue m_GraphicsQueue;
        VkQueue m_PresentationQueue;
        VkFence m_Fences[FRAMES_AHEAD];
        VkSemaphore m_ImageSemaphores[FRAMES_AHEAD];
        VkSemaphore m_RenderSemaphores[FRAMES_AHEAD];
        QueueFamilyIndices m_FamiliyIndices;
        
        VkPhysicalDevice m_Adapter;
        VkPhysicalDeviceProperties m_AdapterProperties;
        
        VkSurfaceKHR m_Surface;
        VkSwapchainKHR m_SwapChain;
        VkFormat m_SwapChainFormat;
        VkExtent2D m_SwapChainSize;
        
        mutable uint64 m_CurrentFrame;
        mutable uint32 m_CurrentBackbufferIndex;
        std::vector<VulkanTexture2D*> m_BackBuffers;
        
        VulkanCommandList* m_pCommandList;
        
        VkPipelineLayout m_DefaultPipelineLayout;
        VkDescriptorSetLayout m_DefaultDescriptorSetLayouts[LAMBDA_SHADERSTAGE_COUNT];
        
        VulkanBuffer* m_pNullBuffer;
        VkDescriptorBufferInfo m_NullBufferDescriptor;
        VulkanTexture2D* m_pNullTexture;
        VkDescriptorImageInfo m_NullTextureDescriptor;
        VulkanSamplerState* m_pNullSampler;
        VkDescriptorImageInfo m_NullSamplerDescriptor;
        
        uint32 m_UniformBinding;
        uint32 m_TextureBinding;
        uint32 m_SamplerBinding;
        
    public:
        static VkDevice GetCurrentDevice();
        static VkPhysicalDevice GetCurrentAdapter();
        static VkPipelineLayout GetDefaultPipelineLayout();
        static VkDescriptorBufferInfo GetNullBufferDescriptor();
        static VkDescriptorImageInfo GetNullTextureDescriptor();
        static VkDescriptorImageInfo GetNullSamplerDescriptor();
        static uint32 GetUniformBinding();
        static uint32 GetTextureBinding();
        static uint32 GetSamplerBinding();
        static VkDescriptorSetLayout* GetDefaultDescriptorSetLayouts();
        static void SetVulkanObjectName(VkObjectType type, uint64 objectHandle, const std::string& name);
        
    private:
        static VKAPI_ATTR VkBool32 VKAPI_CALL VulkanDebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                                                  VkDebugUtilsMessageTypeFlagsEXT messageType,
                                                                  const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                                                                  void* pUserData);
        
    private:
        static PFN_vkSetDebugUtilsObjectNameEXT SetDebugUtilsObjectNameEXT;
    };
    
    
    inline VkFormat VulkanGraphicsDevice::GetBackBufferFormat() const
    {
        return m_SwapChainFormat;
    }
    
    
    inline VkPhysicalDevice VulkanGraphicsDevice::GetAdapter() const
    {
        return m_Adapter;
    }
    
    
    inline QueueFamilyIndices VulkanGraphicsDevice::GetQueueFamilyIndices() const
    {
        return m_FamiliyIndices;
    }

    
    inline VkDevice VulkanGraphicsDevice::GetCurrentDevice()
    {
        assert(IGraphicsDevice::GetInstance() != nullptr);
        return reinterpret_cast<VulkanGraphicsDevice*>(IGraphicsDevice::GetInstance())->m_Device;
    }
    
    
    inline VkPhysicalDevice VulkanGraphicsDevice::GetCurrentAdapter()
    {
        assert(IGraphicsDevice::GetInstance() != nullptr);
        return reinterpret_cast<VulkanGraphicsDevice*>(IGraphicsDevice::GetInstance())->m_Adapter;
    }
    
    
    inline VkPipelineLayout VulkanGraphicsDevice::GetDefaultPipelineLayout()
    {
        assert(IGraphicsDevice::GetInstance() != nullptr);
        return reinterpret_cast<VulkanGraphicsDevice*>(IGraphicsDevice::GetInstance())->m_DefaultPipelineLayout;
    }
    
    
    inline VkDescriptorBufferInfo VulkanGraphicsDevice::GetNullBufferDescriptor()
    {
        assert(IGraphicsDevice::GetInstance() != nullptr);
        return reinterpret_cast<VulkanGraphicsDevice*>(IGraphicsDevice::GetInstance())->m_NullBufferDescriptor;
    }
    
    
    inline VkDescriptorImageInfo VulkanGraphicsDevice::GetNullTextureDescriptor()
    {
        assert(IGraphicsDevice::GetInstance() != nullptr);
        return reinterpret_cast<VulkanGraphicsDevice*>(IGraphicsDevice::GetInstance())->m_NullTextureDescriptor;
    }
    
    
    inline VkDescriptorImageInfo VulkanGraphicsDevice::GetNullSamplerDescriptor()
    {
        assert(IGraphicsDevice::GetInstance() != nullptr);
        return reinterpret_cast<VulkanGraphicsDevice*>(IGraphicsDevice::GetInstance())->m_NullSamplerDescriptor;
    }
    
    
    inline VkDescriptorSetLayout* VulkanGraphicsDevice::GetDefaultDescriptorSetLayouts()
    {
        assert(IGraphicsDevice::GetInstance() != nullptr);
        return reinterpret_cast<VulkanGraphicsDevice*>(IGraphicsDevice::GetInstance())->m_DefaultDescriptorSetLayouts;
    }
    
    
    inline uint32 VulkanGraphicsDevice::GetUniformBinding()
    {
        assert(IGraphicsDevice::GetInstance() != nullptr);
        return reinterpret_cast<VulkanGraphicsDevice*>(IGraphicsDevice::GetInstance())->m_UniformBinding;
    }
    
    
    inline uint32 VulkanGraphicsDevice::GetTextureBinding()
    {
        assert(IGraphicsDevice::GetInstance() != nullptr);
        return reinterpret_cast<VulkanGraphicsDevice*>(IGraphicsDevice::GetInstance())->m_TextureBinding;
    }
    
    
    inline uint32 VulkanGraphicsDevice::GetSamplerBinding()
    {
        assert(IGraphicsDevice::GetInstance() != nullptr);
        return reinterpret_cast<VulkanGraphicsDevice*>(IGraphicsDevice::GetInstance())->m_SamplerBinding;
    }
}
