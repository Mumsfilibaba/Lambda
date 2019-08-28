#pragma once
#include "Graphics/IGraphicsDevice.h"
#if defined(LAMBDA_PLAT_WINDOWS)
	#define VK_USE_PLATFORM_WIN32_KHR
#endif
#include "VulkanUtilities.h"

#define FRAMES_AHEAD 3

namespace Lambda
{    
    //Forward declarations
    class VulkanBuffer;
    class VulkanTexture2D;
    class VulkanCommandList;
    class VulkanSamplerState;
	class VulkanSwapChain;
    
    
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
        virtual void CreateGraphicsPipelineState(IGraphicsPipelineState** ppPipelineState, const GraphicsPipelineStateDesc& desc) const override final;
		virtual void CreateRenderPass(IRenderPass** ppRenderPass, const RenderPassDesc& desc) const override final;

        virtual void DestroyCommandList(ICommandList** ppList) const override final;
        virtual void DestroyBuffer(IBuffer** ppBuffer) const override final;
        virtual void DestroyTexture2D(ITexture2D** ppTexture) const override final;
        virtual void DestroyShader(IShader** ppShader) const override final;
        virtual void DestroySamplerState(ISamplerState** ppSamplerState) const override final;
        virtual void DestroyGraphicsPipelineState(IGraphicsPipelineState** ppPipelineState) const override final;
		virtual void DestroyRenderPass(IRenderPass** ppRenderPass) const override final;
        virtual void Destroy() const override final;
        
        virtual void ExecuteCommandList(ICommandList* const * ppLists, uint32 numLists) const override final;
		virtual void ExecuteCommandListAndPresent(ICommandList* const* ppLists, uint32 numLists) const override final;
        virtual void Present() const override final;
        virtual void GPUWaitForFrame() const override final;
        virtual void WaitForGPU() const override final;
        
        virtual ResourceFormat GetBackBufferFormat() const override final;
        virtual void* GetNativeHandle() const override final;
        virtual ITexture2D* GetCurrentRenderTarget() const override final;
        virtual ITexture2D* GetDepthStencil() const override final;
        virtual uint32 GetCurrentBackBufferIndex() const override final;
        virtual uint32 GetCurrentSwapChainWidth() const override final;
        virtual uint32 GetCurrentSwapChainHeight() const override final;
    
        VkPhysicalDevice GetAdapter() const;
        QueueFamilyIndices GetQueueFamilyIndices() const;
        
    private:
        void Init(IWindow* pWindow, const GraphicsDeviceDesc& desc);
        void InitDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo);
        
        VkPhysicalDevice QueryAdapter();
		VkSurfaceKHR CreateSurface(IWindow* pWindow);
        
        bool CreateSwapChain(uint32 width, uint32 height);
        bool CreateDepthStencil();
        bool CreateTextures();
        bool CreateSemaphoresAndFences();
        bool CreateDefaultLayouts();
        
        void ReleaseSwapChain();
        void ReleaseDepthStencil();
        
        void GetNextFrame() const;
        
        bool AdapterIsSuitable(VkPhysicalDevice adapter);
        
        std::vector<const char*> GetRequiredValidationLayers(bool debug);
        std::vector<const char*> GetRequiredDeviceExtensions();
		std::vector<const char*> GetRequiredInstanceExtensions(bool debug);
        
        virtual bool InternalOnEvent(const Event& event) override final;
        
    private:
        VkInstance m_Instance;
        VkDebugUtilsMessengerEXT m_DebugMessenger;
        VkDevice m_Device;
        VkQueue m_GraphicsQueue;
        VkQueue m_PresentationQueue;

		std::vector<VkFence> m_Fences;
		std::vector<VkSemaphore> m_RenderSemaphores;
		std::vector<VkSemaphore> m_ImageSemaphores;

        QueueFamilyIndices m_FamiliyIndices;
        
        VkPhysicalDevice m_Adapter;
        VkPhysicalDeviceProperties m_AdapterProperties;
        
        VkSurfaceKHR m_Surface;

		VulkanSwapChain* m_pSwapChain;
		VulkanTexture2D* m_pDepthStencil;
		VulkanCommandList* m_pCommandList;

        mutable uint64 m_CurrentFrame;
        
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
        static PFN_vkCreateDebugUtilsMessengerEXT CreateDebugUtilsMessengerEXT;
        static PFN_vkDestroyDebugUtilsMessengerEXT DestroyDebugUtilsMessengerEXT;
    };
    

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
