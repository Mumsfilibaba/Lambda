#pragma once
#include "Graphics/Core/IDevice.h"
#include "VKNAllocator.h"
#include "VKNUtilities.h"

#define FRAMES_AHEAD 3

namespace Lambda
{
    class VKNBuffer;
    class VKNTexture;
    class VKNSwapChain;
    class VKNAllocator;
    class VKNSamplerState;
    class VKNUploadBuffer;
    class VKNDeviceContext;
    class VKNBufferManager;
	class VKNRenderPassCache;
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
        
        virtual void CreateDefferedContext(IDeviceContext** ppDefferedContext) override final;
        virtual void CreateBuffer(IBuffer** ppBuffer, const ResourceData* pInitalData, const BufferDesc& desc) override final;
        virtual void CreateTexture(ITexture** ppTexture, const ResourceData* pInitalData, const TextureDesc& desc) override final;
        virtual void CreateShader(IShader** ppShader, const ShaderDesc& desc) override final;
        virtual void CreateSamplerState(ISamplerState** ppSamplerState, const SamplerStateDesc& desc) override final;
        virtual void CreatePipelineState(IPipelineState** ppPipelineState, const PipelineStateDesc& desc) override final;
		virtual void CreateQuery(IQuery** ppQuery, const QueryDesc& desc) override final;
        virtual IDeviceContext* GetImmediateContext() const override final;
        
        virtual void* GetNativeHandle() const override final;
		virtual const DeviceProperties& GetProperties() const override final;
        virtual const DeviceDesc& GetDesc() const override final;

		void ExecuteCommandBuffer(VkSubmitInfo* pInfo, uint32 numBuffers, VkFence fence) const;
		void Present(VkPresentInfoKHR* pInfo);
		void FinishFrame() const;
		void WaitUntilIdle() const;
		
		void SetVulkanObjectName(VkObjectType type, uint64 objectHandle, const std::string& name);
		VkSampleCountFlagBits GetHighestSampleCount() const;
		VKNDeviceContext* GetVKNImmediateContext() const;

		inline VkInstance GetVkInstance() const									{ return m_Instance; }
		inline VkPhysicalDevice GetVkPhysicalDevice() const						{ return m_PhysicalDevice; }
		inline VkDevice GetVkDevice() const										{ return m_Device; }
		inline QueueFamilyIndices GetQueueFamilyIndices() const					{ return m_FamiliyIndices; }
		inline VkPhysicalDeviceProperties GetPhysicalDeviceProperties() const	{ return m_PhysicalDeviceProperties; }
	private:
		void Init(const DeviceDesc& desc);
		void InitDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo);
		
		VkPhysicalDevice QueryPhyscialDevice();
		bool PhysicalDeviceIsSuitable(VkPhysicalDevice physcialDevice);
        
		std::vector<const char*> GetRequiredValidationLayers(bool debug);
		std::vector<const char*> GetRequiredDeviceExtensions();
		std::vector<const char*> GetRequiredInstanceExtensions(bool debug);
    private:
		VKNBufferManager*			  m_pBufferManager;
		VKNDescriptorPoolManager*	  m_pDescriptorPoolManager;
		VKNFramebufferCache*		  m_pFramebufferCache;
		VKNRenderPassCache*			  m_pRenderPassCache;
		VKNDeviceContext*		      m_pImmediateContext;
		mutable AutoRef<VKNAllocator> m_DeviceAllocator;
        VkQueue						  m_GraphicsQueue;
        VkQueue						  m_PresentationQueue;
		DeviceProperties			  m_Properties;
		VkInstance					  m_Instance;
		VkDebugUtilsMessengerEXT	  m_DebugMessenger;
		VkDevice					  m_Device;
		QueueFamilyIndices			  m_FamiliyIndices;
		VkPhysicalDevice			  m_PhysicalDevice;
		VkPhysicalDeviceProperties	  m_PhysicalDeviceProperties;
		DeviceDesc					  m_Desc;
	private:
        static VKNDevice* s_pInstance;
		static PFN_vkSetDebugUtilsObjectNameEXT		SetDebugUtilsObjectNameEXT;
		static PFN_vkCreateDebugUtilsMessengerEXT	CreateDebugUtilsMessengerEXT;
		static PFN_vkDestroyDebugUtilsMessengerEXT	DestroyDebugUtilsMessengerEXT;
	private:
		static VKAPI_ATTR VkBool32 VKAPI_CALL VulkanDebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
			VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData);
    public:
		static VKNDevice& Get();
    };
}
