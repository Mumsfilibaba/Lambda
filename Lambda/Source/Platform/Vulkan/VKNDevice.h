#pragma once
#include "Graphics/Core/IDevice.h"
#include "VKNAllocator.h"
#include "VKNSafeReleaseManager.h"
#include "VKNUtilities.h"

#define FRAMES_AHEAD 3

namespace Lambda
{
    class VKNBuffer;
    class VKNTexture;
    class VKNSwapChain;
    class VKNSamplerState;
    class VKNUploadBuffer;
    class VKNDeviceContext;
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
		
		bool AllocateImage(VKNAllocation& allocation, VkImage image, ResourceUsage usage);
		bool AllocateBuffer(VKNAllocation& allocation, VkBuffer buffer, ResourceUsage usage);

		VkSampleCountFlagBits GetHighestSampleCount() const;
		VKNDeviceContext* GetVKNImmediateContext() const;

		void SetVulkanObjectName(VkObjectType type, uint64 objectHandle, const std::string& name);


		inline void VKNDevice::Deallocate(VKNAllocation& allocation)
		{
			m_pDeviceAllocator->Deallocate(allocation);
		}


		inline bool VKNDevice::AllocateDynamicMemory(VKNDynamicAllocation& allocation, uint64 sizeInBytes, uint64 alignment)
		{
			return m_pDynamicMemoryAllocator->Allocate(allocation, sizeInBytes, alignment);
		}


		inline void VKNDevice::DeallocateDynamicMemory(VKNDynamicAllocation& allocation)
		{
			m_pDynamicMemoryAllocator->Deallocate(allocation);
		}


		inline VkInstance GetVkInstance() const				
		{ 
			return m_Instance; 
		}
		

		inline VkDevice GetVkDevice() const					
		{ 
			return m_Device; 
		}
		
		
		inline VkPhysicalDevice GetVkPhysicalDevice() const	
		{ 
			return m_PhysicalDevice; 
		}
		
		
		inline const QueueFamilyIndices& GetQueueFamilyIndices() const				 
		{ 
			return m_FamiliyIndices; 
		}
		

		inline const VkPhysicalDeviceProperties& GetPhysicalDeviceProperties() const 
		{ 
			return m_PhysicalDeviceProperties; 
		}


		template<typename VkResourceType>
		inline void SafeReleaseVulkanResource(const VkResourceType& resource)
		{
			m_pSafeReleaseManager->ReleaseResource<VkResourceType>(resource);
		}
	private:
		void Init(const DeviceDesc& desc);
		void InitDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo);		
		VkPhysicalDevice QueryPhyscialDevice();
		bool PhysicalDeviceIsSuitable(VkPhysicalDevice physcialDevice);        
		std::vector<const char*> GetRequiredValidationLayers(bool debug);
		std::vector<const char*> GetRequiredDeviceExtensions();
		std::vector<const char*> GetRequiredInstanceExtensions(bool debug);
    private:
		VKNAllocator*			   m_pDeviceAllocator;
		VKNDynamicMemoryAllocator* m_pDynamicMemoryAllocator;
		VKNDeviceContext*		   m_pImmediateContext;
		VKNRenderPassCache*		   m_pRenderPassCache;
		VKNFramebufferCache*	   m_pFramebufferCache;
		VKNSafeReleaseManager*	   m_pSafeReleaseManager;
		VkInstance m_Instance;
		VkDevice   m_Device;
        VkQueue	   m_GraphicsQueue;
        VkQueue	   m_PresentationQueue;
		DeviceProperties		   m_Properties;
		VkDebugUtilsMessengerEXT   m_DebugMessenger;
		QueueFamilyIndices		   m_FamiliyIndices;
		VkPhysicalDevice		   m_PhysicalDevice;
		VkPhysicalDeviceProperties m_PhysicalDeviceProperties;
		DeviceDesc				   m_Desc;
	private:
		static PFN_vkSetDebugUtilsObjectNameEXT		SetDebugUtilsObjectNameEXT;
		static PFN_vkCreateDebugUtilsMessengerEXT	CreateDebugUtilsMessengerEXT;
		static PFN_vkDestroyDebugUtilsMessengerEXT	DestroyDebugUtilsMessengerEXT;
	private:
		static VKAPI_ATTR VkBool32 VKAPI_CALL VulkanDebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
			VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData);
    };
}
