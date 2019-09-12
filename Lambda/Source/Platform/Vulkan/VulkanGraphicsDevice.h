#pragma once
#include "Graphics/IGraphicsDevice.h"
#if defined(LAMBDA_PLAT_WINDOWS)
	#define VK_USE_PLATFORM_WIN32_KHR
#endif
#include "VulkanAllocator.h"
#include "VulkanUtilities.h"

#define FRAMES_AHEAD 3

namespace Lambda
{    
    class VulkanBuffer;
    class VulkanTexture;
    class VulkanCommandList;
    class VulkanSamplerState;
	class VulkanSwapChain;
	class VulkanUploadBuffer;
	class VulkanDeviceAllocator;
	class VulkanFramebufferCache;
	class VulkanDynamicBufferManager;


	struct DeviceLimits
	{
		VkSampleCountFlagBits MaxSampleCount = VK_SAMPLE_COUNT_1_BIT;
		uint64 UniformBufferAlignment = 0;
	};


	struct DeviceSettings
	{
		VkSampleCountFlagBits SampleCount = VK_SAMPLE_COUNT_1_BIT;
		uint32 FramesAhead = 0;
	};
    
    
    class VulkanGraphicsDevice final : public IGraphicsDevice
    {
    public:
        LAMBDA_NO_COPY(VulkanGraphicsDevice);
        
        VulkanGraphicsDevice(IWindow* pWindow, const GraphicsDeviceDesc& desc);
        ~VulkanGraphicsDevice();
        
        virtual void CreateCommandList(ICommandList** ppList, CommandListType type) override final;
        virtual void CreateBuffer(IBuffer** ppBuffer, const ResourceData* pInitalData, const BufferDesc& desc) override final;
        virtual void CreateTexture(ITexture** ppTexture, const ResourceData* pInitalData, const TextureDesc& desc) override final;
        virtual void CreateShader(IShader** ppShader, const ShaderDesc& desc) override final;
        virtual void CreateSamplerState(ISamplerState** ppSamplerState, const SamplerStateDesc& desc) override final;
        virtual void CreateGraphicsPipelineState(IGraphicsPipelineState** ppPipelineState, const GraphicsPipelineStateDesc& desc) override final;
		virtual void CreateRenderPass(IRenderPass** ppRenderPass, const RenderPassDesc& desc) override final;
		virtual void CreatePipelineResourceState(IPipelineResourceState** ppResourceState, const PipelineResourceStateDesc& desc) override final;
		void CreateUploadBuffer(VulkanUploadBuffer** ppUploadBuffer, uint64 sizeInBytes);

        virtual void DestroyCommandList(ICommandList** ppList) override final;
        virtual void DestroyBuffer(IBuffer** ppBuffer) override final;
        virtual void DestroyTexture(ITexture** ppTexture) override final;
        virtual void DestroyShader(IShader** ppShader) override final;
        virtual void DestroySamplerState(ISamplerState** ppSamplerState) override final;
        virtual void DestroyGraphicsPipelineState(IGraphicsPipelineState** ppPipelineState) override final;
		virtual void DestroyRenderPass(IRenderPass** ppRenderPass) override final;
		virtual void DestroyResourceState(IPipelineResourceState** ppResourceState) override final;
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
    
        void SetVulkanObjectName(VkObjectType type, uint64 objectHandle, const std::string& name);
        
		VkPhysicalDevice GetPhysicalDevice() const;
        QueueFamilyIndices GetQueueFamilyIndices() const;
		DeviceLimits GetDeviceLimits() const;
		DeviceSettings GetDeviceSettings() const;

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

		VulkanDynamicBufferManager* m_pDynamicBufferManager;
		VulkanFramebufferCache* m_pFramebufferCache;
		VulkanTexture* m_pDepthStencil;
		VulkanTexture* m_pMSAABuffer;
		VulkanSwapChain* m_pSwapChain;
		VulkanCommandList* m_pCommandList;
		VulkanDeviceAllocator* m_pDeviceAllocator;

        mutable uint64 m_CurrentFrame;
        
    public:
		static VulkanGraphicsDevice& GetInstance();
        
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
	
	
	inline DeviceLimits VulkanGraphicsDevice::GetDeviceLimits() const
	{
		return m_DeviceLimits;
	}
	
	
	inline DeviceSettings VulkanGraphicsDevice::GetDeviceSettings() const
	{
		return m_DeviceSettings;
	}
}
