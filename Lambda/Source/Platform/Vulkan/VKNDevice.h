#pragma once
#include "Graphics/IDevice.h"
#if defined(LAMBDA_PLAT_WINDOWS)
	#define VK_USE_PLATFORM_WIN32_KHR
#endif
#include "VKNAllocator.h"
#include "VKNUtilities.h"

#define FRAMES_AHEAD 3

namespace Lambda
{
    class VKNBuffer;
    class VKNTexture;
    class VKNSwapChain;
    class VKNAllocator;
    class VKNCommandList;
    class VKNSamplerState;
    class VKNUploadBuffer;
    class VKNBufferManager;
    class VKNFramebufferCache;
    
	//---------
	//VKNDevice
	//---------

    class VKNDevice final : public RefCountedObject<IDevice>
    {
    public:
        LAMBDA_NO_COPY(VKNDevice);
        
        VKNDevice(const DeviceDesc& desc);
        ~VKNDevice();
        
        virtual void CreateCommandList(ICommandList** ppList, CommandListType type) override final;
        virtual void CreateBuffer(IBuffer** ppBuffer, const ResourceData* pInitalData, const BufferDesc& desc) override final;
        virtual void CreateTexture(ITexture** ppTexture, const ResourceData* pInitalData, const TextureDesc& desc) override final;
        virtual void CreateShader(IShader** ppShader, const ShaderDesc& desc) override final;
        virtual void CreateSamplerState(ISamplerState** ppSamplerState, const SamplerStateDesc& desc) override final;
        virtual void CreateGraphicsPipelineState(IGraphicsPipelineState** ppPipelineState, const GraphicsPipelineStateDesc& desc) override final;
		virtual void CreateRenderPass(IRenderPass** ppRenderPass, const RenderPassDesc& desc) override final;
		virtual void CreatePipelineResourceState(IPipelineResourceState** ppResourceState, const PipelineResourceStateDesc& desc) override final;
		virtual void CreateQuery(IQuery** ppQuery, const QueryDesc& desc) override final;
        
        virtual void ExecuteCommandList(ICommandList* const * ppLists, uint32 numLists) const override final;
		
		virtual void PresentBegin() const override final;
		virtual void PresentEnd(ICommandList* const* ppLists, uint32 numLists) const override final;

        virtual void GPUWaitForFrame() const override final;
        virtual void WaitForGPU() const override final;
        
        virtual void* GetNativeHandle() const override final;
		virtual DeviceProperties GetProperties() const override final;
        virtual const DeviceDesc& GetDesc() const override final;
        virtual ITexture* GetDepthStencil() const override final;
        virtual ITexture* GetRenderTarget() const override final;
        virtual ResourceFormat GetBackBufferFormat() const override final;
        virtual uint32 GetBackBufferIndex() const override final;
        virtual uint32 GetSwapChainWidth() const override final;
        virtual uint32 GetSwapChainHeight() const override final;

		void SetVulkanObjectName(VkObjectType type, uint64 objectHandle, const std::string& name);
		VkSampleCountFlagBits GetHighestSampleCount() const;

		VkInstance GetVkInstance() const;
		VkPhysicalDevice GetPhysicalDevice() const;
		VkDevice GetDevice() const;
		VkSurfaceKHR GetSurface() const;
		QueueFamilyIndices GetQueueFamilyIndices() const;
		VkPhysicalDeviceProperties GetPhysicalDeviceProperties() const;
	private:
		void Init(const DeviceDesc& desc);
		void InitDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo);
		
		VkPhysicalDevice QueryPhyscialDevice();
		VkSurfaceKHR CreateSurface(IWindow* pWindow);
		bool CreateDepthStencil();
		bool CreateMSAABuffer();
		bool PhysicalDeviceIsSuitable(VkPhysicalDevice physcialDevice);
        void ReleaseDepthStencil();
		void ReleaseMSAABuffer();
        
		std::vector<const char*> GetRequiredValidationLayers(bool debug);
		std::vector<const char*> GetRequiredDeviceExtensions();
		std::vector<const char*> GetRequiredInstanceExtensions(bool debug);

        virtual bool OnResize(const WindowResizeEvent& event) override final;
    private:
		VKNBufferManager*			  m_pBufferManager;
		VKNDescriptorPoolManager*	  m_pDescriptorPoolManager;
		VKNFramebufferCache*		  m_pFramebufferCache;
		VKNTexture*					  m_pDepthStencil;
		VKNTexture*					  m_pMSAABuffer;
		VKNSwapChain*				  m_pSwapChain;
		VKNCommandList*				  m_pCommandList;
		mutable AutoRef<VKNAllocator> m_DeviceAllocator;
        VkQueue						  m_GraphicsQueue;
        VkQueue						  m_PresentationQueue;
		std::vector<VkFence>		  m_Fences;
		std::vector<VkSemaphore>	  m_RenderSemaphores;
		std::vector<VkSemaphore>	  m_ImageSemaphores;
        mutable uint64				  m_CurrentFrame;
		DeviceProperties			  m_Properties;
		VkInstance					  m_Instance;
		VkDebugUtilsMessengerEXT	  m_DebugMessenger;
		VkDevice					  m_Device;
		QueueFamilyIndices			  m_FamiliyIndices;
		VkPhysicalDevice			  m_PhysicalDevice;
		VkPhysicalDeviceProperties	  m_PhysicalDeviceProperties;
		VkSurfaceKHR				  m_Surface;
		DeviceDesc					  m_Desc;
	private:
		static PFN_vkSetDebugUtilsObjectNameEXT		SetDebugUtilsObjectNameEXT;
		static PFN_vkCreateDebugUtilsMessengerEXT	CreateDebugUtilsMessengerEXT;
		static PFN_vkDestroyDebugUtilsMessengerEXT	DestroyDebugUtilsMessengerEXT;
	private:
		static VKAPI_ATTR VkBool32 VKAPI_CALL VulkanDebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
			VkDebugUtilsMessageTypeFlagsEXT messageType,
			const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
			void* pUserData);
    public:
		static VKNDevice& Get();
    };


	inline VkDevice VKNDevice::GetDevice() const
	{
		return m_Device;
	}


	inline VkInstance VKNDevice::GetVkInstance() const
	{
		return m_Instance;
	}


	inline VkPhysicalDevice VKNDevice::GetPhysicalDevice() const
	{
		return m_PhysicalDevice;
	}


	inline VkSurfaceKHR VKNDevice::GetSurface() const
	{
		return m_Surface;
	}


	inline QueueFamilyIndices VKNDevice::GetQueueFamilyIndices() const
	{
		return m_FamiliyIndices;
	}


	inline VkPhysicalDeviceProperties VKNDevice::GetPhysicalDeviceProperties() const
	{
		return m_PhysicalDeviceProperties;
	}
}
