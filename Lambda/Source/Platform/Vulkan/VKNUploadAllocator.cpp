#include "LambdaPch.h"
#include "Utilities/MathHelper.h"
#include "VKNUploadAllocator.h"
#include "VKNDevice.h"

namespace Lambda
{
	//---------------
	//VKNUploadAllocator
	//---------------

	VKNUploadAllocator::VKNUploadAllocator(VKNDevice* pDevice, uint64 sizeInBytes)
		: m_pDevice(pDevice),
		m_Offset(0),
		m_Memory(),
		m_Buffer(VK_NULL_HANDLE)
	{
		Init(sizeInBytes);
	}


	VKNUploadAllocator::~VKNUploadAllocator()
	{
		Reset();

		if (m_Buffer != VK_NULL_HANDLE)
		{
			m_pDevice->Deallocate(m_Memory);
			m_pDevice->SafeReleaseVulkanResource<VkBuffer>(m_Buffer);
		}

		LOG_DEBUG_INFO("Vulkan: Destroyed UploadBuffer '%p'\n", m_Buffer);
	}


	bool VKNUploadAllocator::Init(uint64 sizeInBytes)
	{
		//Create buffer
		VkBufferCreateInfo info = {};
		info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		info.pNext = nullptr;
		info.flags = 0;
		info.queueFamilyIndexCount = 0;
		info.pQueueFamilyIndices = nullptr;
		info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		info.size = sizeInBytes;
		info.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
		if (vkCreateBuffer(m_pDevice->GetVkDevice(), &info, nullptr, &m_Buffer) != VK_SUCCESS)
		{
			LOG_DEBUG_ERROR("Vulkan: Failed to create UploadBuffer\n");
			return false;
		}
		else
		{
			LOG_DEBUG_INFO("Vulkan: Created UploadBuffer '%p'\n", m_Buffer);
		}

		//Allocate memory
		if (m_pDevice->AllocateBuffer(m_Memory, m_Buffer, RESOURCE_USAGE_DYNAMIC))
		{
			Reset();
			return true;
		}
		else
		{
			LOG_DEBUG_ERROR("Vulkan: Failed to allocate memory for UploadBuffer '%p'\n", m_Buffer);
			return false;
		}
	}


	void VKNUploadAllocator::Reallocate(uint64 sizeInBytes)
	{
		VkBuffer  newBuffer = VK_NULL_HANDLE;
		VKNAllocation newMemory = {};

		//Create buffer
		VkBufferCreateInfo info = {};
		info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		info.pNext = nullptr;
		info.flags = 0;
		info.queueFamilyIndexCount = 0;
		info.pQueueFamilyIndices = nullptr;
		info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		info.size = sizeInBytes;
		info.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
		if (vkCreateBuffer(m_pDevice->GetVkDevice(), &info, nullptr, &newBuffer) != VK_SUCCESS)
		{
			LOG_DEBUG_ERROR("Vulkan: Failed to recreate UploadBuffer\n");
		}
		else
		{
			LOG_DEBUG_WARNING("Vulkan: Recreated UploadBuffer '%p'\n", newBuffer);
		}

		//Allocate memory
		if (m_pDevice->AllocateBuffer(newMemory, newBuffer, RESOURCE_USAGE_DYNAMIC))
		{
			m_pDevice->Deallocate(m_Memory);
			m_pDevice->SafeReleaseVulkanResource<VkBuffer>(m_Buffer);

			//Set new buffer and memory
			m_Buffer = newBuffer;
			m_Memory = newMemory;

			Reset();
		}
		else
		{
			LOG_DEBUG_ERROR("Vulkan: Failed to reallocate memory for UploadBuffer '%p'\n", m_Buffer);
		}
	}


	VKNUploadAllocation VKNUploadAllocator::Allocate(uint64 bytesToAllocate, uint64 alignment)
	{
		VKNUploadAllocation allocation = {};

		//Align
		uint64 alignedOffset = Math::AlignUp<uint64>(m_Offset, alignment);
		uint64 padding = alignedOffset - m_Offset;

		//Do we have enough space? If no reallocate
		uint64 alignedSize = bytesToAllocate + padding;
		if ((alignedOffset + alignedSize) >= m_Memory.SizeInBytes)
			Reallocate(m_Memory.SizeInBytes + alignedSize + MB(1));

		//Move pointer
		allocation.pHostMemory = reinterpret_cast<void*>(m_Memory.pHostMemory + alignedOffset);
		m_Offset += alignedSize;

		//Set offset
		allocation.SizeInBytes = alignedSize;
		allocation.DeviceOffset = alignedOffset;

		//Set buffer
		allocation.Buffer = m_Buffer;
		return allocation;
	}


	void VKNUploadAllocator::Reset()
	{
		//Reset buffer by resetting current to the start
		m_Offset = 0;
	}
}
