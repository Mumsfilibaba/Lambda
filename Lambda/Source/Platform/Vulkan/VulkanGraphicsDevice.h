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
    class VulkanTexture;
    class VulkanCommandList;
    class VulkanSamplerState;
	class VulkanSwapChain;


	struct DeviceLimits
	{
		VkSampleCountFlagBits MaxSampleCount = VK_SAMPLE_COUNT_1_BIT;
	};


	struct DeviceSettings
	{
		VkSampleCountFlagBits SampleCount = VK_SAMPLE_COUNT_1_BIT;
	};
    
    
    class VulkanGraphicsDevice final : public IGraphicsDevice
    {
    public:
        LAMBDA_NO_COPY(VulkanGraphicsDevice);
        
        VulkanGraphicsDevice(IWindow* pWindow, const GraphicsDeviceDesc& desc);
        ~VulkanGraphicsDevice();
        
        virtual void CreateCommandList(ICommandList** ppList, CommandListType type) const override final;
        virtual void CreateBuffer(IBuffer** ppBuffer, const ResourceData* pInitalData, const BufferDesc& desc) const override final;
        virtual void CreateTexture(ITexture** ppTexture, const ResourceData* pInitalData, const TextureDesc& desc) const override final;
        virtual void CreateShader(IShader** ppShader, const ShaderDesc& desc) const override final;
        virtual void CreateSamplerState(ISamplerState** ppSamplerState, const SamplerStateDesc& desc) const override final;
        virtual void CreateGraphicsPipelineState(IGraphicsPipelineState** ppPipelineState, const GraphicsPipelineStateDesc& desc) const override final;
		virtual void CreateRenderPass(IRenderPass** ppRenderPass, const RenderPassDesc& desc) const override final;
		virtual void CreateResourceState(IResourceState** ppResourceState, const ResourceStateDesc& desc) const override final;

        virtual void DestroyCommandList(ICommandList** ppList) const override final;
        virtual void DestroyBuffer(IBuffer** ppBuffer) const override final;
        virtual void DestroyTexture(ITexture** ppTexture) const override final;
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
        virtual ITexture* GetDepthStencil() const override final;
        virtual ITexture* GetRenderTarget() const override final;
        virtual ResourceFormat GetBackBufferFormat() const override final;
        virtual uint32 GetBackBufferIndex() const override final;
        virtual uint32 GetSwapChainWidth() const override final;
        virtual uint32 GetSwapChainHeight() const override final;
    
        VkDeviceMemory AllocateImage(VkImage image, VkMemoryPropertyFlags properties) const;
        
        void SetVulkanObjectName(VkObjectType type, uint64 objectHandle, const std::string& name);
        VkPhysicalDevice GetPhysicalDevice() const;
        QueueFamilyIndices GetQueueFamilyIndices() const;
        
    private:
        void Init(IWindow* pWindow, const GraphicsDeviceDesc& desc);
        void InitDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo);
        
        VkPhysicalDevice QueryPhyscialDevice();
		VkSurfaceKHR CreateSurface(IWindow* pWindow);
        bool CreateDepthStencil();
		bool CreateMSAABuffer();

        void ReleaseDepthStencil();
		void ReleaseMSAABuffer();

        void GetNextFrame() const;
        bool PhysicalDeviceIsSuitable(VkPhysicalDevice physcialDevice);
        
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

		DeviceLimits m_DeviceLimits;
		DeviceSettings m_DeviceSettings;
        QueueFamilyIndices m_FamiliyIndices;
        
        VkPhysicalDevice m_PhysicalDevice;
        VkPhysicalDeviceProperties m_AdapterProperties;
        
        VkSurfaceKHR m_Surface;

		VulkanSwapChain* m_pSwapChain;
		VulkanTexture* m_pDepthStencil;
		VulkanTexture* m_pMSAABuffer;
		VulkanCommandList* m_pCommandList;

        mutable uint64 m_CurrentFrame;
        
    public:
        static VkDevice GetCurrentDevice();
        static VkPhysicalDevice GetCurrentPhysicalDevice();
        
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
    

    inline VkPhysicalDevice VulkanGraphicsDevice::GetPhysicalDevice() const
    {
        return m_PhysicalDevice;
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
    
    
    inline VkPhysicalDevice VulkanGraphicsDevice::GetCurrentPhysicalDevice()
    {
        assert(IGraphicsDevice::GetInstance() != nullptr);
        return reinterpret_cast<VulkanGraphicsDevice*>(IGraphicsDevice::GetInstance())->m_PhysicalDevice;
    }
}
