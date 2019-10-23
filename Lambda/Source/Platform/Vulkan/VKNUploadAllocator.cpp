#include "LambdaPch.h"
#include "Utilities/MathHelper.h"
#include "VKNUploadAllocator.h"
#include "VKNDevice.h"

namespace Lambda
{
	//-------------
	//VKNUploadPage
	//-------------

	VKNUploadPage::VKNUploadPage(VKNDevice* pDevice, VkDeviceSize sizeInBytes)
		: m_VkBuffer(VK_NULL_HANDLE),
		m_Memory(),
		m_SizeInBytes(sizeInBytes),
		m_Offset(0)
	{
		Init(pDevice);
	}


	void VKNUploadPage::Init(VKNDevice* pDevice)
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
		if (vkCreateBuffer(pDevice->GetVkDevice(), &info, nullptr, &m_VkBuffer) != VK_SUCCESS)
		{
			LOG_DEBUG_ERROR("Vulkan: Failed to create UploadPage\n");
			return;
		}
		else
		{
			LOG_DEBUG_WARNING("Vulkan: Created UploadPage\n");
		}

		//Allocate memory
		if (!pDevice->AllocateBuffer(m_Memory, m_VkBuffer, RESOURCE_USAGE_DYNAMIC))
		{
			LOG_DEBUG_ERROR("Vulkan: Failed to allocate UploadPage '%p'\n", m_VkBuffer);
		}
		else
		{
			LOG_DEBUG_WARNING("Vulkan: Allocated '%d' bytes for UploadPage\n", m_SizeInBytes);
		}
	}


	VKNUploadAllocation VKNUploadPage::Allocate(VkDeviceSize sizeInBytes, VkDeviceSize alignment)
	{
		VKNUploadAllocation allocation = {};
	
		//Align
		VkDeviceSize alignedOffset = Math::AlignUp<VkDeviceSize>(m_Offset, alignment);

		//Do we have enough space? If no reallocate
		VkDeviceSize alignedSize = sizeInBytes + (alignedOffset - m_Offset);
		if ((alignedOffset + alignedSize) > m_SizeInBytes)
		{
			return allocation;
		}

		//Move offset
		m_Offset += alignedSize;

		//Setup allocation
		allocation.pPage  = this;
		allocation.Offset = alignedOffset;
		return allocation;
	}

	
	void VKNUploadPage::Reset()
	{
		m_Offset = 0;
	}


	void VKNUploadPage::Destroy(VKNDevice* pDevice)
	{
		LAMBDA_ASSERT(pDevice != nullptr);

		//Deallocate memory from global memory manager
		pDevice->Deallocate(m_Memory);

		//Delete buffer
		if (m_VkBuffer != VK_NULL_HANDLE)
			pDevice->SafeReleaseVulkanResource<VkBuffer>(m_VkBuffer);

		LOG_SYSTEM(LOG_SEVERITY_WARNING, "Vulkan: Deallocated VKNUploadPage\n");
		delete this;
	}


	//------------------
	//VKNUploadAllocator
	//------------------

	VKNUploadAllocator::VKNUploadAllocator(VKNDevice* pDevice, VkDeviceSize sizeInBytes)
		: m_pDevice(pDevice),
		m_pCurrentPage(nullptr),
		m_DiscardedPages()
	{
		//Create the first page
		m_pCurrentPage = DBG_NEW VKNUploadPage(pDevice, sizeInBytes);
	}


	VKNUploadAllocator::~VKNUploadAllocator()
	{
		Reset();
		m_pCurrentPage->Destroy(m_pDevice);

		LOG_DEBUG_INFO("Vulkan: Destroyed VKNUploadAllocator\n");
	}


	VKNUploadAllocation VKNUploadAllocator::Allocate(VkDeviceSize sizeInBytes, VkDeviceSize alignment)
	{
		using namespace Math;

		if (AlignUp<VkDeviceSize>(m_pCurrentPage->GetSizeLeft(), alignment) > AlignUp<VkDeviceSize>(sizeInBytes, alignment))
			return m_pCurrentPage->Allocate(sizeInBytes, alignment);

		//If allocation cannot be made with old page, allocate new with enough size 
		m_DiscardedPages.emplace_back(m_pCurrentPage);
		m_pCurrentPage = DBG_NEW VKNUploadPage(m_pDevice, sizeInBytes + MB(1));

		return m_pCurrentPage->Allocate(sizeInBytes, alignment);
	}


	void VKNUploadAllocator::Reset()
	{
		m_pCurrentPage->Reset();

		for (auto page : m_DiscardedPages)
			page->Destroy(m_pDevice);

		m_DiscardedPages.clear();
	}
}
