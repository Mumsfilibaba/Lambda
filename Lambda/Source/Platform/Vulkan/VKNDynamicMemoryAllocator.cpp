#include "LambdaPch.h"
#include "Utilities/MathHelper.h"
#include "VKNDynamicMemoryAllocator.h"
#include "VKNDevice.h"

namespace Lambda
{
	//---------------------
	//VKNDynamicMemoryPage
	//---------------------

	VKNDynamicMemoryPage::VKNDynamicMemoryPage(VKNDevice* pDevice, uint32 id, VkDeviceSize sizeInBytes)
		: m_Buffer(VK_NULL_HANDLE),
		m_ID(id),
		m_SizeInBytes(sizeInBytes),
		m_BlockCount(0),
		m_pHead(nullptr),
		m_pNextFree(nullptr)
	{
		Init(pDevice);
	}


	void VKNDynamicMemoryPage::Init(VKNDevice* pDevice)
	{
		VkBufferCreateInfo info = {};
		info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		info.pNext = nullptr;
		info.flags = 0;
		info.size = m_SizeInBytes;
		info.queueFamilyIndexCount = 0;
		info.pQueueFamilyIndices = nullptr;
		info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		info.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT |
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT |
			VK_BUFFER_USAGE_VERTEX_BUFFER_BIT |
			VK_BUFFER_USAGE_INDEX_BUFFER_BIT |
			VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;

		//Create buffer
		if (vkCreateBuffer(pDevice->GetVkDevice(), &info, nullptr, &m_Buffer) != VK_SUCCESS)
		{
			LOG_DEBUG_ERROR("Vulkan: Failed to create Buffer\n");
			return;
		}
		else
		{
			LOG_DEBUG_WARNING("Vulkan: Created Dynamic Memory-Page\n");
		}

		//Allocate memory
		if (!pDevice->AllocateBuffer(m_Memory, m_Buffer, RESOURCE_USAGE_DYNAMIC))
		{
			LOG_DEBUG_ERROR("Vulkan: Failed to allocate Dynamic Memory-Page '%p'\n", m_Buffer);
		}
		else
		{
			LOG_DEBUG_WARNING("Vulkan: Allocated '%d' bytes for Dynamic Memory-Page\n", m_SizeInBytes);
		}

		//Setup first block
		m_pHead = DBG_NEW VKNDynamicMemoryBlock();
		m_pHead->pPage = this;
		m_pHead->pNext = nullptr;
		m_pHead->pPrevious = nullptr;
		m_pHead->IsFree = true;
		m_pHead->ID = m_BlockCount++;
		m_pHead->SizeInBytes = m_SizeInBytes;
		m_pHead->BufferOffset = 0;

		//Set next free to the first block
		m_pNextFree = m_pHead;
	}


	bool VKNDynamicMemoryPage::Allocate(VKNDynamicAllocation& allocation, VkDeviceSize sizeInBytes, VkDeviceSize alignment)
	{
		VkDeviceSize paddedBufferOffset = 0;
		VkDeviceSize paddedSizeInBytes = 0;
		VKNDynamicMemoryBlock* pBestFit = nullptr;

		//Find enough free space, and find the block that best fits
		for (VKNDynamicMemoryBlock* pCurrent = m_pNextFree; pCurrent != nullptr; pCurrent = pCurrent->pNext)
		{
			//Check if the block is allocated or not
			if (!pCurrent->IsFree)
				continue;

			//Does it fit into the block
			if (sizeInBytes > pCurrent->SizeInBytes)
				continue;

			//Align the offset and padding
			paddedBufferOffset = Math::AlignUp<uint64>(pCurrent->BufferOffset, alignment);
			paddedSizeInBytes = sizeInBytes + (paddedBufferOffset - pCurrent->BufferOffset);

			//Does it still fit
			if (paddedSizeInBytes > pCurrent->SizeInBytes)
				continue;

			pBestFit = pCurrent;
			break;
		}


		//If we did not find a fitting block we try again but from the start of the list
		if (pBestFit == nullptr)
		{
			m_pNextFree = m_pHead;
			for (VKNDynamicMemoryBlock* pCurrent = m_pNextFree; pCurrent != nullptr; pCurrent = pCurrent->pNext)
			{
				//Check if the block is allocated or not
				if (!pCurrent->IsFree)
					continue;

				//Does it fit into the block
				if (sizeInBytes > pCurrent->SizeInBytes)
					continue;

				//Align the offset and padding
				paddedBufferOffset = Math::AlignUp<uint64>(pCurrent->BufferOffset, alignment);
				paddedSizeInBytes = sizeInBytes + (paddedBufferOffset - pCurrent->BufferOffset);

				//Does it still fit
				if (paddedSizeInBytes > pCurrent->SizeInBytes)
					continue;

				pBestFit = pCurrent;
				break;
			}
		}


		//Did we find a suitable block to make the allocation?
		if (pBestFit == nullptr)
		{
			return false;
		}


		//        Free block
		//|--------------------------|
		//padding Allocation Remaining
		//|------|----------|--------|
		if (pBestFit->SizeInBytes > sizeInBytes)
		{
			//Create a new block after allocation
			VKNDynamicMemoryBlock* pBlock = DBG_NEW VKNDynamicMemoryBlock();
			pBlock->ID = m_BlockCount++;
			pBlock->pPage = this;
			pBlock->SizeInBytes = pBestFit->SizeInBytes - paddedSizeInBytes;
			pBlock->BufferOffset = pBestFit->BufferOffset + paddedSizeInBytes;
			pBlock->IsFree = true;

			//Set pointers
			pBlock->pNext = pBestFit->pNext;
			pBlock->pPrevious = pBestFit;
			if (pBestFit->pNext)
				pBestFit->pNext->pPrevious = pBlock;
			pBestFit->pNext = pBlock;

			//Set block as the next free block. At next allocation we start by looking at this block
			m_pNextFree = pBlock;
		}
		else
		{
			m_pNextFree = m_pHead;
		}

		//Update bestfit
		pBestFit->SizeInBytes = paddedSizeInBytes;
		pBestFit->IsFree = false;

		//Setup allocation
		allocation.pBlock = pBestFit;
		allocation.Buffer = m_Buffer;
		allocation.BufferOffset = paddedBufferOffset;
		allocation.SizeInBytes = sizeInBytes;
		allocation.pHostMemory = m_Memory.pHostMemory + allocation.BufferOffset;

#if defined (LAMBDA_DYNAMIC_ALLOCATOR_DEBUG_ALLOC)
		{
			LOG_DEBUG_INFO("Vulkan: [DYNAMIC ALLOCATION] Dynamic Memory Page '%d'\n", m_ID);
			for (VKNDynamicMemoryBlock* pCurrent = m_pHead; pCurrent != nullptr; pCurrent = pCurrent->pNext)
			{
				LOG_DEBUG_INFO("----Block %d----\n", pCurrent->ID);
				LOG_DEBUG_INFO("Starts at: %llu\n", pCurrent->BufferOffset);
				LOG_DEBUG_INFO("Free: %s\n", pCurrent->IsFree ? "true" : "false");
				if (pCurrent->pPrevious)
				{
					VKNDynamicMemoryBlock* pPrevious = pCurrent->pPrevious;
					if ((pPrevious->BufferOffset + pPrevious->PaddedSizeInBytes) > pCurrent->BufferOffset)
					{
						LOG_DEBUG_WARNING("Overlapping memory in page '%d' between blocks '%d' and '%d'\n", m_ID, pPrevious->ID, pCurrent->ID);
					}
				}
				LOG_DEBUG_INFO("   End at: %llu\n", pCurrent->BufferOffset + pCurrent->PaddedSizeInBytes);
				LOG_DEBUG_INFO("----------------\n", pCurrent->ID);
			}
		}
#endif

		return true;
	}


	void VKNDynamicMemoryPage::Deallocate(VKNDynamicAllocation& allocation)
	{
		//LOG_DEBUG_INFO("Vulkan: Deallocated Dynamic-Block ID=%u\n", allocation.BlockID);

		//Try to find the correct block
		VKNDynamicMemoryBlock* pCurrent = allocation.pBlock;
		if (!pCurrent)
		{
			LOG_DEBUG_ERROR("Vulkan: Block for Dynamic Allocation is invalid\n");
			return;
		}

		//Set this block to free
		pCurrent->IsFree = true;

		//Merge previous with current
		if (pCurrent->pPrevious)
		{
			VKNDynamicMemoryBlock* pPrevious = pCurrent->pPrevious;
			if (pPrevious->IsFree)
			{
				//Set size
				pPrevious->SizeInBytes += pCurrent->SizeInBytes;

				//Set pointers
				pPrevious->pNext = pCurrent->pNext;
				if (pCurrent->pNext)
					pCurrent->pNext->pPrevious = pPrevious;

				//Remove block
				delete pCurrent;
				pCurrent = pPrevious;
			}
		}

		//Try and merge current with next
		if (pCurrent->pNext)
		{
			VKNDynamicMemoryBlock* pNext = pCurrent->pNext;
			if (pNext->IsFree)
			{
				//Set size
				pCurrent->SizeInBytes += pNext->SizeInBytes;

				//Set pointers
				if (pNext->pNext)
					pNext->pNext->pPrevious = pCurrent;
				pCurrent->pNext = pNext->pNext;

				//Remove block
				delete pNext;
			}
		}

#if defined (LAMBDA_DYNAMIC_ALLOCATOR_DEBUG_DEALLOC)
		{
			LOG_DEBUG_INFO("Vulkan: [DYNAMIC DEALLOCATION] Dynamic Memory Page '%d'\n", m_ID);
			for (pCurrent = m_pHead; pCurrent != nullptr; pCurrent = pCurrent->pNext)
			{
				LOG_DEBUG_INFO("----Block %d----\n", pCurrent->ID);
				LOG_DEBUG_INFO("Starts at: %llu\n", pCurrent->BufferOffset);
				LOG_DEBUG_INFO("Free: %s\n", pCurrent->IsFree ? "true" : "false");
				if (pCurrent->pPrevious)
				{
					VKNDynamicMemoryBlock* pPrevious = pCurrent->pPrevious;
					if ((pPrevious->BufferOffset + pPrevious->PaddedSizeInBytes) > pCurrent->BufferOffset)
					{
						LOG_DEBUG_WARNING("Overlapping memory in page '%d' between blocks '%d' and '%d'\n", m_ID, pPrevious->ID, pCurrent->ID);
					}
				}
				LOG_DEBUG_INFO("   End at: %llu\n", pCurrent->BufferOffset + pCurrent->PaddedSizeInBytes);
				LOG_DEBUG_INFO("----------------\n", pCurrent->ID);
			}
		}
#endif
	}


	void VKNDynamicMemoryPage::Destroy(VKNDevice* pDevice)
	{
		LAMBDA_ASSERT(pDevice != nullptr);

		//Print memoryleaks
#if defined(LAMBDA_DEBUG)
		VKNDynamicMemoryBlock* pDebug = m_pHead;
		LOG_DEBUG_WARNING("Allocated blocks left in Dynamic Memory-Page %u:\n", m_ID);
		while (pDebug)
		{
			LOG_DEBUG_WARNING("    VulkanDynamicBlock: ID=%u, Offset=%u, Size=%u, IsFree=%s\n", pDebug->ID, pDebug->BufferOffset, pDebug->SizeInBytes, pDebug->IsFree ? "True" : "False");
			pDebug = pDebug->pNext;
		}
#endif

		//Deallocate memory from global memory manager
		pDevice->Deallocate(m_Memory);

		//Delete first block
		SafeDelete(m_pHead);

		//Delete buffer
		if (m_Buffer != VK_NULL_HANDLE)
			pDevice->SafeReleaseVulkanResource<VkBuffer>(m_Buffer);

		LOG_SYSTEM(LOG_SEVERITY_WARNING, "Vulkan: Deallocated DynamicMemoryPage\n");
		delete this;
	}


	//-------------------------
	//VKNDynamicMemoryAllocator
	//-------------------------

	VKNDynamicMemoryAllocator::VKNDynamicMemoryAllocator(VKNDevice* pDevice)
		: m_pDevice(pDevice),
		m_pCurrentPage(nullptr),
		m_TotalReserved(0),
		m_TotalAllocated(0),
		m_FrameIndex(0),
		m_Pages(),
		m_MemoryToDeallocate()
	{
		//Resize the number of garbage memory vectors
		m_MemoryToDeallocate.resize(numFrames);
		for (auto& mem : m_MemoryToDeallocate)
			mem.reserve(500);
	}


	VKNDynamicMemoryAllocator::~VKNDynamicMemoryAllocator()
	{
		//Cleanup all garbage memory before deleting
		for (uint32 i = 0; i < numFrames; i++)
			EmptyGarbageMemory();

		//Delete allocator
		LOG_SYSTEM(LOG_SEVERITY_WARNING, "Vulkan: Deleting Dynamic MemoryAllocator. Number of pages: %u\n", m_Pages.size());
		for (auto page : m_Pages)
		{
			if (page)
				page->Destroy(m_pDevice);
		}

		LOG_DEBUG_INFO("Vulkan: Destroyed Dynamic MemoryAllocator\n");
	}


	bool VKNDynamicMemoryAllocator::Allocate(VKNDynamicAllocation& allocation, uint64 sizeInBytes, uint64 alignment)
	{
		//Add to total
		m_TotalAllocated += sizeInBytes;

		//Try allocating from existing page
		if (m_pCurrentPage)
		{
			if (m_pCurrentPage->Allocate(allocation, sizeInBytes, alignment))
			{
				return true;
			}
		}

		//Add to total
		uint64 bytesToReserve = MB(16);
		m_TotalReserved += bytesToReserve;

		//Allocate new page
		m_pCurrentPage = DBG_NEW VKNDynamicMemoryPage(m_pDevice, uint32(m_Pages.size()), bytesToReserve);
		m_Pages.emplace_back(m_pCurrentPage);

		return m_pCurrentPage->Allocate(allocation, sizeInBytes, alignment);
	}


	void VKNDynamicMemoryAllocator::Deallocate(VKNDynamicAllocation& allocation)
	{
		//Set it to be removed
		if (allocation.pBlock && allocation.Buffer != VK_NULL_HANDLE)
			m_MemoryToDeallocate[m_FrameIndex].emplace_back(allocation);

		//Invalidate memory
		allocation.pBlock = nullptr;
		allocation.BufferOffset = 0;
		allocation.SizeInBytes = 0;
		allocation.Buffer = VK_NULL_HANDLE;
		allocation.pHostMemory = nullptr;
	}


	void VKNDynamicMemoryAllocator::EmptyGarbageMemory()
	{
		//Move on a frame
		m_FrameIndex = (m_FrameIndex + 1) % numFrames;

		//Clean memory
		auto& memoryBlocks = m_MemoryToDeallocate[m_FrameIndex];
		if (!memoryBlocks.empty())
		{
			//Deallocate all the blocks
			for (auto& memory : memoryBlocks)
			{
				VKNDynamicMemoryPage* pPage = memory.pBlock->pPage;
				pPage->Deallocate(memory);
				m_TotalAllocated -= memory.SizeInBytes;
			}

			memoryBlocks.clear();
		}
	}
}