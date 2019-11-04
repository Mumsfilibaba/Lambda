#pragma once
#include "Graphics/Core/DeviceBase.h"
#include "Memory/VKNDeviceAllocator.h"
#include "VKNSafeReleaseManager.h"
#include "Memory/VKNDynamicMemoryAllocator.h"
#include "VKNUtilities.h"

#define FRAMES_AHEAD 3

namespace Lambda
{
    class VKNBuffer;
    class VKNTexture;
    class VKNSwapChain;
    class VKNSamplerState;
    class VKNDeviceContext;
	class VKNRenderPassCache;
    class VKNFramebufferCache;
    
	//---------
	//VKNDevice
	//---------

    class VKNDevice final : public DeviceBase
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

        VkResult Present(VkPresentInfoKHR* pInfo);
        void ExecuteCommandBuffer(VkSubmitInfo* pInfo, uint32 numBuffers, VkFence fence) const;
		void FinishFrame() const;
		void WaitUntilIdle() const;

		void SetVulkanObjectName(VkObjectType type, uint64 objectHandle, const std::string& name);

		VkSampleCountFlagBits GetHighestSampleCount() const;
		VKNDeviceContext* GetVKNImmediateContext() const;


		_forceinline bool Allocate(VKNAllocation& allocation, const VkMemoryRequirements& memoryRequirements, VkMemoryPropertyFlags properties)
		{
			return m_pVkDeviceAllocator->Allocate(allocation, memoryRequirements, properties);
		}


		_forceinline void Deallocate(VKNAllocation& allocation)
		{
			m_pVkDeviceAllocator->Deallocate(allocation);
		}


		_forceinline bool AllocateDynamicMemory(VKNDynamicAllocation& allocation, uint64 sizeInBytes, uint64 alignment)
		{
			return m_pVkDynamicMemoryAllocator->Allocate(allocation, sizeInBytes, alignment);
		}


		_forceinline void DeallocateDynamicMemory(VKNDynamicAllocation& allocation)
		{
			m_pVkDynamicMemoryAllocator->Deallocate(allocation);
		}


		_forceinline VkInstance GetVkInstance() const
		{ 
			return m_VkInstance; 
		}
		

		_forceinline VkDevice GetVkDevice() const
		{ 
			return m_VkDevice; 
		}
		
		
		_forceinline VkPhysicalDevice GetVkPhysicalDevice() const
		{ 
			return m_VkPhysicalDevice; 
		}
		
		
		_forceinline const QueueFamilyIndices& GetQueueFamilyIndices() const
		{ 
			return m_FamiliyIndices; 
		}
		

		_forceinline const VkPhysicalDeviceProperties& GetVkPhysicalDeviceProperties() const
		{ 
			return m_VkPhysicalDeviceProperties; 
		}


		_forceinline const VkQueueFamilyProperties& GetVkGraphicsQueueProperties() const
		{
			return m_GraphicsVkQueueProperties;
		}


		_forceinline VKNBuffer* GetDefaultVertexBuffer() const
		{
			return m_pDefaultVertexBuffer;
		}


		_forceinline VKNBuffer* GetDefaultIndexBuffer() const
		{
			return m_pDefaultIndexBuffer;
		}


		_forceinline VKNBuffer* GetDefaultConstantBuffer() const
		{
			return m_pDefaultConstantBuffer;
		}


		_forceinline VKNSamplerState* GetDefaultSamplerState() const
		{
			return m_pDefaultSamplerState;
		}


		_forceinline VKNTexture* GetDefaultTexture() const
		{
			return m_pDefaultTexture;
		}


		template<typename VkResourceType>
		_forceinline void SafeReleaseVkResource(const VkResourceType& resource)
		{
			m_pVkSafeReleaseManager->ReleaseResource<VkResourceType>(resource);
		}
	private:
		void Init(const DeviceDesc& desc);
		void InitDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo);		
		VkPhysicalDevice QueryPhyscialDevice();
		bool PhysicalDeviceIsSuitable(VkPhysicalDevice physcialDevice);        
		std::vector<const char*> GetRequiredValidationLayers(bool debug);
		std::vector<const char*> GetOptionalValidationLayers(bool debug);
		std::vector<const char*> GetRequiredDeviceExtensions();
		std::vector<const char*> GetOptionalDeviceExtensions();
		std::vector<const char*> GetRequiredInstanceExtensions(bool debug);
		std::vector<const char*> GetOptionalInstanceExtensions(bool debug);
    private:
		VKNDeviceAllocator*	m_pVkDeviceAllocator;
		VKNDynamicMemoryAllocator* m_pVkDynamicMemoryAllocator;
		VKNDeviceContext* m_pVkImmediateContext;
		VKNRenderPassCache*	m_pVkRenderPassCache;
		VKNFramebufferCache* m_pVkFramebufferCache;
		VKNSafeReleaseManager* m_pVkSafeReleaseManager;
		VkInstance m_VkInstance;
		VkDevice m_VkDevice;
        VkQueue	m_GraphicsVkQueue;
        VkQueue	m_PresentationVkQueue;
		VkDebugUtilsMessengerEXT m_VkDebugMessenger;
		QueueFamilyIndices m_FamiliyIndices;
		VkPhysicalDevice m_VkPhysicalDevice;
		VkPhysicalDeviceProperties m_VkPhysicalDeviceProperties;
		VkQueueFamilyProperties	m_GraphicsVkQueueProperties;
		VKNBuffer* m_pDefaultIndexBuffer;
		VKNBuffer* m_pDefaultVertexBuffer;
		VKNBuffer* m_pDefaultConstantBuffer;
		VKNTexture* m_pDefaultTexture;
		VKNSamplerState* m_pDefaultSamplerState;
	private:
		static PFN_vkSetDebugUtilsObjectNameEXT	vkSetDebugUtilsObjectNameEXT;
		static PFN_vkCreateDebugUtilsMessengerEXT vkCreateDebugUtilsMessengerEXT;
		static PFN_vkDestroyDebugUtilsMessengerEXT vkDestroyDebugUtilsMessengerEXT;
	private:
		static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
			VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData);
    };
}
