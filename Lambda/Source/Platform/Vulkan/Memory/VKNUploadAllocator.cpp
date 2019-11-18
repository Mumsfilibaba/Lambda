#include "LambdaPch.h"
#include "Utilities/MathHelper.h"
#include "VKNUploadAllocator.h"
#include "../VKNDevice.h"

namespace Lambda
{
	//-------------
	//VKNUploadPage
	//-------------

	VKNUploadPage::VKNUploadPage(VKNDevice* pVkDevice, VkDeviceSize sizeInBytes)
		: m_VkBuffer(VK_NULL_HANDLE),
		m_Memory(),
		m_SizeInBytes(sizeInBytes),
		m_OffsetInBytes(0)
	{
		Init(pVkDevice);
	}


	void VKNUploadPage::Init(VKNDevice* pVkDevice)
	{
		//Create buffer
		VkBufferCreateInfo info = {};
		info.sType					= VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		info.pNext					= nullptr;
		info.flags					= 0;
		info.size					= m_SizeInBytes;
		info.queueFamilyIndexCount	= 0;
		info.pQueueFamilyIndices	= nullptr;
		info.sharingMode			= VK_SHARING_MODE_EXCLUSIVE;
		info.usage					= VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
		if (vkCreateBuffer(pVkDevice->GetVkDevice(), &info, nullptr, &m_VkBuffer) != VK_SUCCESS)
		{
			LOG_RENDER_API_ERROR("[Vulkan] [UPLOADBUFFER] Failed to create page\n");
			return;
		}
		else
		{
			LOG_RENDER_API_WARNING("[Vulkan] [UPLOADBUFFER] Created page\n");
		}


		//Memory properties
		VkMemoryPropertyFlags memoryProperties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

		//Allocate memory
		VkMemoryRequirements memoryRequirements = {};
		vkGetBufferMemoryRequirements(pVkDevice->GetVkDevice(), m_VkBuffer, &memoryRequirements);
		if (!pVkDevice->Allocate(m_Memory, memoryRequirements, memoryProperties))
		{
			LOG_RENDER_API_ERROR("[Vulkan] [UPLOADBUFFER] Failed to allocate page '%p'\n", m_VkBuffer);
			return;
		}
		else
		{
			LOG_RENDER_API_WARNING("[Vulkan] [UPLOADBUFFER] Allocated '%d' bytes for page\n", m_SizeInBytes);
			if (vkBindBufferMemory(pVkDevice->GetVkDevice(), m_VkBuffer, m_Memory.DeviceMemory, m_Memory.DeviceMemoryOffset) != VK_SUCCESS)
			{
				LOG_RENDER_API_WARNING("[Vulkan] [UPLOADBUFFER] Failed to bind memory page\n");
			}
		}
	}


	VKNUploadAllocation VKNUploadPage::Allocate(VkDeviceSize sizeInBytes, VkDeviceSize alignment)
	{
		VKNUploadAllocation allocation = {};
	
		//Align
		VkDeviceSize alignedOffset = Math::AlignUp<VkDeviceSize>(m_OffsetInBytes, alignment);

		//Do we have enough space? If no reallocate
		VkDeviceSize alignedSize = sizeInBytes + (alignedOffset - m_OffsetInBytes);
		if ((alignedOffset + alignedSize) > m_SizeInBytes)
		{
			return allocation;
		}

		//Move offset
		m_OffsetInBytes += alignedSize;

		//Setup allocation
		allocation.pPage  = this;
		allocation.Offset = alignedOffset;
		return allocation;
	}


	void VKNUploadPage::Destroy(VKNDevice* pVkDevice)
	{
		LAMBDA_ASSERT(pVkDevice != nullptr);

		//Deallocate memory from global memory manager
		pVkDevice->Deallocate(m_Memory);

		//Delete buffer
		if (m_VkBuffer != VK_NULL_HANDLE)
			pVkDevice->SafeReleaseVkResource<VkBuffer>(m_VkBuffer);

		LOG(LOG_CHANNEL_RENDER_API, LOG_SEVERITY_WARNING, "[Vulkan] [UPLOADBUFFER] Deallocated page\n");
		delete this;
	}


	//------------------
	//VKNUploadAllocator
	//------------------

	VKNUploadAllocator::VKNUploadAllocator(VKNDevice* pVkDevice, VkDeviceSize sizeInBytes)
		: m_pVkDevice(pVkDevice),
		m_pCurrentPage(nullptr),
		m_DiscardedPages()
	{
		//Create the first page
		m_pCurrentPage = DBG_NEW VKNUploadPage(pVkDevice, sizeInBytes);
	}


	VKNUploadAllocator::~VKNUploadAllocator()
	{
		Reset();
		m_pCurrentPage->Destroy(m_pVkDevice);

		LOG_RENDER_API_INFO("[Vulkan] [UPLOADBUFFER] Destroyed allocator\n");
	}


	VKNUploadAllocation VKNUploadAllocator::Allocate(VkDeviceSize sizeInBytes, VkDeviceSize alignment)
	{
		using namespace Math;

		if (AlignUp<VkDeviceSize>(m_pCurrentPage->GetSizeLeft(), alignment) > AlignUp<VkDeviceSize>(sizeInBytes, alignment))
			return m_pCurrentPage->Allocate(sizeInBytes, alignment);

		//If allocation cannot be made with old page, allocate new with enough size 
		m_DiscardedPages.emplace_back(m_pCurrentPage);
		m_pCurrentPage = DBG_NEW VKNUploadPage(m_pVkDevice, sizeInBytes + MB(1));

		return m_pCurrentPage->Allocate(sizeInBytes, alignment);
	}

	
	void VKNUploadAllocator::Reset()
	{
		m_pCurrentPage->Reset();

		for (auto pPage : m_DiscardedPages)
		{
			if (pPage)
				pPage->Destroy(m_pVkDevice);
		} 

		m_DiscardedPages.clear();
	}
}
