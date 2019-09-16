#pragma once
#include "Defines.h"
#include "Types.h"
#include "Graphics/IGraphicsDevice.h"
#if defined(LAMBDA_PLAT_WINDOWS)
	#define VK_USE_PLATFORM_WIN32_KHR
#endif
#include "VKNUtilities.h"

namespace Lambda
{
	class IWindow;

	//--------------
	//DeviceSettings
	//--------------

	struct DeviceSettings
	{
		uint32					FramesAhead = 0;
		VkSampleCountFlagBits	SampleCount = VK_SAMPLE_COUNT_1_BIT;
	};


	//---------
	//VKNDevice
	//---------

	class VKNDevice
	{
	public:
		LAMBDA_NO_COPY(VKNDevice);

		VKNDevice(const GraphicsDeviceDesc& desc);
		~VKNDevice() = default;

		void Destroy();
		void SetVulkanObjectName(VkObjectType type, uint64 objectHandle, const std::string& name);

		VkInstance					GetVkInstance() const;
		VkPhysicalDevice			GetPhysicalDevice() const;
		VkDevice					GetDevice() const;
		VkSurfaceKHR				GetSurface() const;
		QueueFamilyIndices			GetQueueFamilyIndices() const;
		VkPhysicalDeviceProperties	GetPhysicalDeviceProperties() const;
		DeviceSettings				GetDeviceSettings() const;
		GraphicsDeviceDesc			GetDesc() const;

	private:
		void Init(const GraphicsDeviceDesc& desc);
		void InitDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo);

		bool PhysicalDeviceIsSuitable(VkPhysicalDevice physcialDevice);

		VkPhysicalDevice	QueryPhyscialDevice();
		VkSurfaceKHR		CreateSurface(IWindow* pWindow);

		std::vector<const char*> GetRequiredValidationLayers(bool debug);
		std::vector<const char*> GetRequiredDeviceExtensions();
		std::vector<const char*> GetRequiredInstanceExtensions(bool debug);

	private:
		VkInstance					m_Instance;
		VkDebugUtilsMessengerEXT	m_DebugMessenger;
		VkDevice					m_Device;
		DeviceSettings				m_DeviceSettings;
		QueueFamilyIndices			m_FamiliyIndices;
		VkPhysicalDevice			m_PhysicalDevice;
		VkPhysicalDeviceProperties	m_PhysicalDeviceProperties;
		VkSurfaceKHR				m_Surface;
		GraphicsDeviceDesc          m_Desc;

	private:
		static VKNDevice*							s_pInstance;
		static PFN_vkSetDebugUtilsObjectNameEXT		SetDebugUtilsObjectNameEXT;
		static PFN_vkCreateDebugUtilsMessengerEXT	CreateDebugUtilsMessengerEXT;
		static PFN_vkDestroyDebugUtilsMessengerEXT	DestroyDebugUtilsMessengerEXT;

	private:
		static VKAPI_ATTR VkBool32 VKAPI_CALL VulkanDebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
			VkDebugUtilsMessageTypeFlagsEXT messageType,
			const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
			void* pUserData);

	public:
		static VKNDevice& GetInstance();
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


	inline DeviceSettings VKNDevice::GetDeviceSettings() const
	{
		return m_DeviceSettings;
	}

	
	inline GraphicsDeviceDesc VKNDevice::GetDesc() const
	{
		return m_Desc;
	}
}

