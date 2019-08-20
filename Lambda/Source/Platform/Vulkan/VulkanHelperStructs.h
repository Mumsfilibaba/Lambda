#pragma once
#include "Defines.h"
#include "Types.h"
#include <vector>
#include <vulkan/vulkan.h>

namespace Lambda
{
	//Helperstruct for when finding queuefamilies
	struct QueueFamilyIndices
	{
		int32 GraphicsFamily = -1;
		int32 PresentFamily = -1;

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
			return !Formats.empty() && !PresentModes.empty() && (Capabilities.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_DST_BIT);
		}
	};


	//Helper function to retrive information about the device's swapchain capabilities
	inline SwapChainCapabilities QuerySwapChainSupport(VkPhysicalDevice adapter, VkSurfaceKHR surface)
	{
		SwapChainCapabilities info;

		//Get capabilities from the surface
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(adapter, surface, &info.Capabilities);

		//Get supported surface formats
		uint32 formatCount = 0;
		vkGetPhysicalDeviceSurfaceFormatsKHR(adapter, surface, &formatCount, nullptr);

		if (formatCount != 0)
		{
			info.Formats.resize(formatCount);
			vkGetPhysicalDeviceSurfaceFormatsKHR(adapter, surface, &formatCount, info.Formats.data());
		}

		//Get presentation modes
		uint32 presentModeCount = 0;
		vkGetPhysicalDeviceSurfacePresentModesKHR(adapter, surface, &presentModeCount, nullptr);

		if (presentModeCount != 0)
		{
			info.PresentModes.resize(presentModeCount);
			vkGetPhysicalDeviceSurfacePresentModesKHR(adapter, surface, &presentModeCount, info.PresentModes.data());
		}

		return info;
	}


	//Function to retrive queuefamilies from physicaldevice
	inline QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice adapter, VkSurfaceKHR surface)
	{
		//Get all queuefamilies for this adapter
		uint32 queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(adapter, &queueFamilyCount, nullptr);

		std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(adapter, &queueFamilyCount, queueFamilies.data());

		//Find a queuefamily that supports
		int32 i = 0;
		QueueFamilyIndices indices;
		for (const auto& queueFamiliy : queueFamilies)
		{
			//Check for graphics support
			if (queueFamiliy.queueCount > 0 && queueFamiliy.queueFlags & VK_QUEUE_GRAPHICS_BIT)
			{
				indices.GraphicsFamily = i;
			}

			//Check for presentationsupport
			VkBool32 presentSupport = false;
			vkGetPhysicalDeviceSurfaceSupportKHR(adapter, i, surface, &presentSupport);
			if (queueFamiliy.queueCount > 0 && presentSupport)
			{
				indices.PresentFamily = i;
			}

			//If we have found a queuefamiliy for both presentation and graphics we break
			if (indices.Valid())
			{
				break;
			}

			i++;
		}

		return indices;
	}
}