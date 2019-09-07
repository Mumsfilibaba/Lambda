#pragma once
#include "Graphics/IGraphicsDevice.h"
#if defined(LAMBDA_PLAT_WINDOWS)
	#define VK_USE_PLATFORM_WIN32_KHR
#endif
#include "VulkanUtilities.h"

#define FRAMES_AHEAD 3

namespace Lambda
{    
    class VulkanBuffer;
    class VulkanTexture2D;
    class VulkanCommandList;
    class VulkanSamplerState;
	class VulkanSwapChain;
    
    
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
        virtual void CreateSamplerState(ISamplerState** ppSamplerState, const SamplerStateDesc& desc) const override final;
        virtual void CreateGraphicsPipelineState(IGraphicsPipelineState** ppPipelineState, const GraphicsPipelineStateDesc& desc) const override final;
		virtual void CreateRenderPass(IRenderPass** ppRenderPass, const RenderPassDesc& desc) const override final;
		virtual void CreateResourceState(IResourceState** ppResourceState, const ResourceStateDesc& desc) const override final;

        virtual void DestroyCommandList(ICommandList** ppList) const override final;
        virtual void DestroyBuffer(IBuffer** ppBuffer) const override final;
        virtual void DestroyTexture2D(ITexture2D** ppTexture) const override final;
        virtual void DestroyShader(IShader** ppShader) const override final;
        virtual void DestroySamplerState(ISamplerState** ppSamplerState) const override final;
        virtual void DestroyGraphicsPipelineState(IGraphicsPipelineState** ppPipelineState) const override final;
		virtual void DestroyRenderPass(IRenderPass** ppRenderPass) const override final;
		virtual void DestroyResourceState(IResourceState** ppResourceState) const override final;
        virtual void Destroy() const override final;
        
        virtual void ExecuteCommandList(ICommandList* const * ppLists, uint32 numLists) const override final;
		virtual void ExecuteCommandListAndPresent(ICommandList* const* ppLists, uint32 numLists) const override final;
        virtual void Present() const override final;
        virtual void GPUWaitForFrame() const override final;
        virtual void WaitForGPU() const override final;
        
        virtual void* GetNativeHandle() const override final;
        virtual ITexture2D* GetDepthStencil() const override final;
        virtual ITexture2D* GetCurrentRenderTarget() const override final;
        virtual ResourceFormat GetBackBufferFormat() const override final;
        virtual uint32 GetCurrentBackBufferIndex() const override final;
        virtual uint32 GetSwapChainWidth() const override final;
        virtual uint32 GetSwapChainHeight() const override final;
    
        VkPhysicalDevice GetAdapter() const;
        QueueFamilyIndices GetQueueFamilyIndices() const;
        
        void SetVulkanObjectName(VkObjectType type, uint64 objectHandle, const std::string& name);
        
    private:
        void Init(IWindow* pWindow, const GraphicsDeviceDesc& desc);
        void InitDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo);
        
        VkPhysicalDevice QueryAdapter();
		VkSurfaceKHR CreateSurface(IWindow* pWindow);
        bool CreateDepthStencil();

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
        
    public:
        static VkDevice GetCurrentDevice();
        static VkPhysicalDevice GetCurrentAdapter();
        
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
}
