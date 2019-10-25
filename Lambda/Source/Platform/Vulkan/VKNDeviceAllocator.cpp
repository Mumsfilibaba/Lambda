#include "LambdaPch.h"
#include "Utilities/MathHelper.h"
#include "VKNDevice.h"
#include "VKNDeviceAllocator.h"
#include "VKNUtilities.h"

//#define LAMBDA_ALLOCATOR_DEBUG
#define LAMBDA_ALLOCATOR_MAX MB(16)

namespace Lambda
{
	//--------------
	//VKNMemoryPage
	//--------------

	constexpr float mb = 1024.0f * 1024.0f;

	VKNMemoryPage::VKNMemoryPage(VKNDevice* pDevice, uint32 id, VkDeviceSize sizeInBytes, uint32 memoryType, ResourceUsage usage)
		: m_Usage(usage),
		m_ID(id),
		m_MemoryType(memoryType),
		m_SizeInBytes(sizeInBytes),
        m_BlockCount(0),
		m_pHead(nullptr),
		m_pHostMemory(nullptr),
		m_IsMapped(false)
	{
		Init(pDevice);
	}


	void VKNMemoryPage::Init(VKNDevice* pDevice)
	{
		//Allocate device memory
		VkMemoryAllocateInfo allocInfo  = {};
		allocInfo.sType				    = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.pNext				    = nullptr;
		allocInfo.allocationSize	    = m_SizeInBytes;
		allocInfo.memoryTypeIndex	    = m_MemoryType;
		if (vkAllocateMemory(pDevice->GetVkDevice(), &allocInfo, nullptr, &m_DeviceMemory) != VK_SUCCESS)
		{
			LOG_DEBUG_ERROR("Vulkan: Failed to allocate MemoryPage\n");
			return;
		}
		else
		{
			LOG_DEBUG_WARNING("Vulkan: Allocated '%d' bytes for MemoryPage\n", allocInfo.allocationSize);
		}

		//Setup first block
        m_pHead = DBG_NEW VKNMemoryBlock();
        m_pHead->pNext					= nullptr;
        m_pHead->pPrevious				= nullptr;
        m_pHead->IsFree					= true;
        m_pHead->ID						= m_BlockCount++;
		m_pHead->SizeInBytes			= m_SizeInBytes;
		m_pHead->PaddedSizeInBytes		= m_SizeInBytes;
		m_pHead->DeviceMemoryOffset		= 0;
		
		//If this is CPU visible -> Map
		if (m_Usage == RESOURCE_USAGE_DYNAMIC)
		{
			Map(pDevice);
		}
	}


	bool VKNMemoryPage::Allocate(VKNAllocation& allocation, VkDeviceSize sizeInBytes, VkDeviceSize alignment, VkDeviceSize granularity)
	{
        VkDeviceSize paddedDeviceOffset = 0;
        VkDeviceSize paddedSizeInBytes  = 0;
        VKNMemoryBlock* pBestFit = nullptr;
        
        //Find enough free space, and find the block that best fits
        for (VKNMemoryBlock* pCurrent = m_pHead; pCurrent != nullptr; pCurrent = pCurrent->pNext)
        {
            //Check if the block is allocated or not
            if (!pCurrent->IsFree)
                continue;
            
            //Does it fit into the block
            if (sizeInBytes > pCurrent->SizeInBytes)
                continue;
            
            //Align the offset
            paddedDeviceOffset = Math::AlignUp<uint64>(pCurrent->DeviceMemoryOffset, alignment);
            
            //Take granularity into account
            if (pCurrent->pPrevious != nullptr && granularity > 1)
            {
                VKNMemoryBlock* pPrevious = pCurrent->pPrevious;
                if (IsOnSamePage(pPrevious->DeviceMemoryOffset, pPrevious->SizeInBytes, paddedDeviceOffset, granularity))
                {
                    paddedDeviceOffset = Math::AlignUp(paddedDeviceOffset, granularity);
                }
            }
            
            //Calculate padding
            paddedSizeInBytes = sizeInBytes + (paddedDeviceOffset - pCurrent->DeviceMemoryOffset);
            
            //Does it still fit
            if (paddedSizeInBytes > pCurrent->SizeInBytes)
                continue;
            
            //Avoid granularity conflict
            if (granularity > 1 && pCurrent->pNext != nullptr)
            {
                VKNMemoryBlock* pNext = pCurrent->pNext;
                if (IsOnSamePage(paddedDeviceOffset, sizeInBytes, pNext->DeviceMemoryOffset, granularity))
                {
                    continue;
                }
            }
            
            pBestFit = pCurrent;
			break;
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
        if (pBestFit->SizeInBytes > paddedSizeInBytes)
        {
            //Create a new block after allocation
            VKNMemoryBlock* pBlock = DBG_NEW VKNMemoryBlock();
            pBlock->ID                  = m_BlockCount++;
            pBlock->SizeInBytes			= pBestFit->SizeInBytes - paddedSizeInBytes;
			pBlock->PaddedSizeInBytes	= pBlock->SizeInBytes;
            pBlock->DeviceMemoryOffset  = pBestFit->DeviceMemoryOffset + paddedSizeInBytes;
            pBlock->IsFree              = true;
            
            //Set pointers
            pBlock->pNext     = pBestFit->pNext;
            pBlock->pPrevious = pBestFit;
            if (pBestFit->pNext)
                pBestFit->pNext->pPrevious = pBlock;
            pBestFit->pNext = pBlock;
        }
        
        //Update bestfit
        pBestFit->SizeInBytes		= paddedSizeInBytes;
		pBestFit->PaddedSizeInBytes = paddedSizeInBytes;
        pBestFit->IsFree = false;
        
        //Setup allocation
        allocation.BlockID            = pBestFit->ID;
        allocation.PageID			  = m_ID;
        allocation.DeviceMemory       = m_DeviceMemory;
        allocation.DeviceMemoryOffset = paddedDeviceOffset;
        allocation.SizeInBytes		  = sizeInBytes;
        if (m_Usage == RESOURCE_USAGE_DYNAMIC)
            allocation.pHostMemory = m_pHostMemory + allocation.DeviceMemoryOffset;
        else
            allocation.pHostMemory = nullptr;

#if defined (LAMBDA_ALLOCATOR_DEBUG)
		{
			LOG_DEBUG_INFO("Vulkan: Memory Page '%d'\n", m_ID);
			for (VKNMemoryBlock* pCurrent = m_pHead; pCurrent != nullptr; pCurrent = pCurrent->pNext)
			{
				LOG_DEBUG_INFO("----Block %d----\n", pCurrent->ID);
				LOG_DEBUG_INFO("Starts at: %llu\n", pCurrent->DeviceMemoryOffset);
				LOG_DEBUG_INFO("Free: %s\n", pCurrent->IsFree ? "true" : "false");
				if (pCurrent->pPrevious)
				{
					VKNMemoryBlock* pPrevious = pCurrent->pPrevious;
					if ((pPrevious->DeviceMemoryOffset + pPrevious->PaddedSizeInBytes) > pCurrent->DeviceMemoryOffset)
					{
						LOG_DEBUG_WARNING("Overlapping memory in page '%d' between blocks '%d' and '%d'\n", m_ID, pPrevious->ID, pCurrent->ID);
					}
				}
				LOG_DEBUG_INFO("   End at: %llu\n", pCurrent->DeviceMemoryOffset + pCurrent->PaddedSizeInBytes);
				LOG_DEBUG_INFO("----------------\n", pCurrent->ID);
			}
		}
#endif

		return true;
	}


    bool VKNMemoryPage::IsOnSamePage(VkDeviceSize aOffset, VkDeviceSize aSize, VkDeviceSize bOffset, VkDeviceSize pageSize)
    {
        LAMBDA_ASSERT( aOffset + aSize <= bOffset && aSize > 0 && pageSize > 0 );
        
        VkDeviceSize aEnd       = aOffset + (aSize-1);
        VkDeviceSize aEndPage   = aEnd & ~(pageSize-1);
        VkDeviceSize bStart     = bOffset;
        VkDeviceSize bStartPage = bStart & ~(pageSize-1);
        return aEndPage == bStartPage;
    }


	void VKNMemoryPage::Map(VKNDevice* pDevice)
	{
		//If not mapped -> map
		if (!m_IsMapped)
		{
			void* pMemory = nullptr;
			vkMapMemory(pDevice->GetVkDevice(), m_DeviceMemory, 0, VK_WHOLE_SIZE, 0, &pMemory);

			m_pHostMemory   = reinterpret_cast<uint8*>(pMemory);
			m_IsMapped      = true;
		}
	}


	void VKNMemoryPage::Unmap(VKNDevice* pDevice)
	{
		//If mapped -> unmap
		if (m_IsMapped)
		{
			vkUnmapMemory(pDevice->GetVkDevice(), m_DeviceMemory);
			m_pHostMemory = nullptr;
			m_IsMapped    = false;
		}
	}


	void VKNMemoryPage::Deallocate(VKNAllocation& allocation)
	{
        LOG_DEBUG_INFO("Vulkan: Deallocated Block ID=%u\n", allocation.BlockID);
        
        //Try to find the correct block
        VKNMemoryBlock* pCurrent = m_pHead;
		for (pCurrent = m_pHead; pCurrent != nullptr; pCurrent = pCurrent->pNext)
		{
			//Did we find the correct block
			if (pCurrent->ID == uint32(allocation.BlockID))
				break;
		}

		if (!pCurrent)
        {
            LOG_DEBUG_ERROR("Vulkan: Block owning allocation was not found\n");
            return;
        }
		
		//Set this block to free
		pCurrent->IsFree = true;

		//Merge previous with current
        if (pCurrent->pPrevious)
        {
            VKNMemoryBlock* pPrevious = pCurrent->pPrevious;
            if (pPrevious->IsFree)
            {
                //Set size
				pPrevious->SizeInBytes			+= pCurrent->SizeInBytes;
				pPrevious->PaddedSizeInBytes	+= pCurrent->PaddedSizeInBytes;

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
            VKNMemoryBlock* pNext = pCurrent->pNext;    
            if (pNext->IsFree)
            {
			   //Set size
			   pCurrent->SizeInBytes		+= pNext->SizeInBytes;
			   pCurrent->PaddedSizeInBytes	+= pNext->PaddedSizeInBytes;

                //Set pointers
                if (pNext->pNext)
                    pNext->pNext->pPrevious = pCurrent;
                pCurrent->pNext = pNext->pNext;

                //Remove block
                delete pNext;
            }
        }
	}


	void VKNMemoryPage::Destroy(VKNDevice* pDevice)
	{
		LAMBDA_ASSERT(pDevice != nullptr);

		if (m_DeviceMemory != VK_NULL_HANDLE)
		{
			//Unmap
			Unmap(pDevice);

			//Print memoryleaks
#if defined(LAMBDA_DEBUG)
			VKNMemoryBlock* pDebug = m_pHead;
			LOG_DEBUG_WARNING("Allocated blocks left in MemoryPage %u:\n", m_ID);
			while (pDebug)
			{
				LOG_DEBUG_WARNING("    VulkanBlock: ID=%u, Offset=%u, Size=%u, IsFree=%s\n", pDebug->ID, pDebug->DeviceMemoryOffset, pDebug->SizeInBytes, pDebug->IsFree ? "True" : "False");
				pDebug = pDebug->pNext;
			}
#endif

			//Delete first block
			SafeDelete(m_pHead);

			//Free memory
			vkFreeMemory(pDevice->GetVkDevice(), m_DeviceMemory, nullptr);
			m_DeviceMemory = VK_NULL_HANDLE;

			LOG_SYSTEM(LOG_SEVERITY_WARNING, "Vulkan: Deallocated MemoryPage\n");
		}

		delete this;
	}

	//------------------
	//VKNDeviceAllocator
	//------------------

    constexpr size_t numFrames = 5;

	VKNDeviceAllocator::VKNDeviceAllocator(VKNDevice* pDevice)
		: m_pDevice(pDevice),
		m_MaxAllocations(0),
        m_TotalReserved(0),
        m_TotalAllocated(0),
        m_FrameIndex(0),
		m_Pages(),
		m_MemoryToDeallocate()
	{
        //Resize the number of garbage memory vectors
        m_MemoryToDeallocate.resize(numFrames);
        
        //Setup from properties of the device
		VkPhysicalDeviceProperties properties = pDevice->GetPhysicalDeviceProperties();
		m_MaxAllocations = properties.limits.maxMemoryAllocationCount;
        m_BufferImageGranularity = properties.limits.bufferImageGranularity;
	}


	VKNDeviceAllocator::~VKNDeviceAllocator()
	{
		//Cleanup all garbage memory before deleting
		for (uint32 i = 0; i < numFrames; i++)
			EmptyGarbageMemory();

		//Delete allocator
		LOG_SYSTEM(LOG_SEVERITY_WARNING, "Vulkan: Deleting DeviceAllocator. Number of Pages: %u\n", m_Pages.size());
		for (auto page : m_Pages)
		{
			if (page)
				page->Destroy(m_pDevice);
		}

		LOG_DEBUG_INFO("Vulkan: Destroyed DeviceAllocator\n");
	}


	bool VKNDeviceAllocator::Allocate(VKNAllocation& allocation, const VkMemoryRequirements& memoryRequirements, ResourceUsage usage)
	{
		//Set memoryproperty based on resource usage
		VkMemoryPropertyFlags properties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
		if (usage == RESOURCE_USAGE_DYNAMIC)
			properties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

		//Add to total
		m_TotalAllocated += memoryRequirements.size;

		//Get needed memorytype
		uint32 memoryType = FindMemoryType(m_pDevice->GetVkPhysicalDevice(), memoryRequirements.memoryTypeBits, properties);

        
		//Try allocating from existing page
		for (auto page : m_Pages)
		{
			if (page->GetMemoryType() == memoryType)
			{
				if (page->Allocate(allocation, memoryRequirements.size, memoryRequirements.alignment, m_BufferImageGranularity))
				{
					LOG_SYSTEM(LOG_SEVERITY_WARNING, "[VULKAN DEVICE ALLOCATOR] Allocated '%d' bytes. Memory-Type: %d. Total Allocated: %.2fMB. Total Reserved %.2fMB\n", memoryRequirements.size, memoryType, float(m_TotalAllocated) / mb, float(m_TotalReserved) / mb);
					return true;
				}
			}
		}
        
		LAMBDA_ASSERT_PRINT(m_Pages.size() < m_MaxAllocations, "Vulkan: Max number of allocations already reached\n");
		
		//If allocated is large, make a dedicated allocation
		uint64 bytesToReserve = MB(128);
		if (memoryRequirements.size > bytesToReserve)
			bytesToReserve = memoryRequirements.size;

		//Add to total
		m_TotalReserved += bytesToReserve;
		
		//Allocate new page
		VKNMemoryPage* pPage = DBG_NEW VKNMemoryPage(m_pDevice, uint32(m_Pages.size()), bytesToReserve, memoryType, usage);
		m_Pages.emplace_back(pPage);

		LOG_SYSTEM(LOG_SEVERITY_WARNING, "[VULKAN DEVICE ALLOCATOR] Allocated Memory-Page. Allocationcount: '%llu/%llu'. Memory-Type: %d. Total Allocated: %.2fMB. Total Reserved %.2fMB\n", m_Pages.size(), m_MaxAllocations, memoryType, float(m_TotalAllocated) / mb, float(m_TotalReserved) / mb);

        return pPage->Allocate(allocation, memoryRequirements.size, memoryRequirements.alignment, m_BufferImageGranularity);
	}


	void VKNDeviceAllocator::Deallocate(VKNAllocation& allocation)
	{
        //Set it to be removed
        if (size_t(allocation.PageID) < m_Pages.size() && allocation.DeviceMemory != VK_NULL_HANDLE)
            m_MemoryToDeallocate[m_FrameIndex].emplace_back(allocation);

        //Invalidate memory
        allocation.BlockID            = -1;
        allocation.PageID			  = -1;
        allocation.DeviceMemoryOffset = 0;
        allocation.SizeInBytes		  = 0;
        allocation.DeviceMemory       = VK_NULL_HANDLE;
        allocation.pHostMemory        = nullptr;
	}


	void VKNDeviceAllocator::EmptyGarbageMemory()
	{
        //Move on a frame
        m_FrameIndex = (m_FrameIndex+1) % numFrames;
        
        //Clean memory
		auto& memoryBlocks = m_MemoryToDeallocate[m_FrameIndex];
		if (!memoryBlocks.empty())
		{
            //Deallocate all the blocks
			for (auto& memory : memoryBlocks)
            {
                if (size_t(memory.PageID) < m_Pages.size() && memory.DeviceMemory != VK_NULL_HANDLE)
                {
                    VKNMemoryPage* pPage = m_Pages[memory.PageID];
                    pPage->Deallocate(memory);
                    
                    m_TotalAllocated -= memory.SizeInBytes;

					LOG_SYSTEM(LOG_SEVERITY_WARNING, "[VULKAN DEVICE ALLOCATOR] Deallocated '%d' bytes. Total Allocated: %.2fMB. Total Reserved %.2fMB\n", memory.SizeInBytes, float(m_TotalAllocated) / mb, float(m_TotalReserved) / mb);
                }
            }
            
			memoryBlocks.clear();
		}

		//Remove empty pages
		for (auto it = m_Pages.begin(); it != m_Pages.end();)
		{
			if ((*it)->IsEmpty())
			{
				//Erase from vector
				m_TotalReserved -= (*it)->GetSize();

				(*it)->Destroy(m_pDevice);
				it = m_Pages.erase(it);

				LOG_SYSTEM(LOG_SEVERITY_WARNING, "[VULKAN DEVICE ALLOCATOR] Destroyed Memory-Page. Allocationcount: '%llu/%llu'. Total Allocated: %.2fMB. Total Reserved %.2fMB\n", m_Pages.size(), m_MaxAllocations, float(m_TotalAllocated) / mb, float(m_TotalReserved) / mb);
			}
			else
			{
				it++;
			}
		}
	}

	//-------------------------
	//VKNDescriptorSetAllocator
	//-------------------------

	VKNDescriptorSetAllocator::VKNDescriptorSetAllocator(VKNDevice* pDevice, uint32 uniformBufferCount, uint32 dynamicUniformBufferCount, uint32 samplerCount, uint32 sampledImageCount, uint32 combinedImageSamplerCount, uint32 numSets)
		: m_pDevice(pDevice),
		m_Pool(VK_NULL_HANDLE),
		m_NumSets(0),
		m_UniformBufferCount(uniformBufferCount),
		m_DynamicUniformBufferCount(dynamicUniformBufferCount),
		m_SamplerCount(samplerCount),
		m_SampledImageCount(sampledImageCount),
		m_CombinedImageSamplerCount(combinedImageSamplerCount),
		m_SetCount(numSets)
	{
		Init();
	}
	
	
	VkDescriptorSet VKNDescriptorSetAllocator::Allocate(VkDescriptorSetLayout descriptorSetLayout)
	{
		//Check for more descriptor
		if (m_NumSets < 1)
			Init();

		VkDescriptorSet set = VK_NULL_HANDLE;

		//Allocate descriptorsets
		VkDescriptorSetAllocateInfo descriptorAllocInfo = {};
		descriptorAllocInfo.sType				= VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		descriptorAllocInfo.pNext				= nullptr;
		descriptorAllocInfo.descriptorPool		= m_Pool;
		descriptorAllocInfo.descriptorSetCount	= 1;
		descriptorAllocInfo.pSetLayouts			= &descriptorSetLayout;
		if (vkAllocateDescriptorSets(m_pDevice->GetVkDevice(), &descriptorAllocInfo, &set))
		{
			LOG_DEBUG_ERROR("Vulkan: Failed to allocate DescriptorSets\n");
			return VK_NULL_HANDLE;
		}
		else
		{
			LOG_DEBUG_INFO("Vulkan: Allocated DescriptorSets\n");
			m_NumSets--;

			return set;
		}
	}
	
	
	void VKNDescriptorSetAllocator::Destroy(VkDevice device)
	{
		LAMBDA_ASSERT(device != VK_NULL_HANDLE);

		if (m_Pool != VK_NULL_HANDLE)
			m_pDevice->SafeReleaseVulkanResource<VkDescriptorPool>(m_Pool);

		delete this;
	}


	void VKNDescriptorSetAllocator::Init()
	{
		VkDescriptorPool pool = VK_NULL_HANDLE;

		//Setup descriptorcounts
		std::vector<VkDescriptorPoolSize> poolSizes;
		if (m_UniformBufferCount > 0)
		{
			VkDescriptorPoolSize poolSize = {};
			poolSize.type				= VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			poolSize.descriptorCount	= m_UniformBufferCount * m_SetCount;
			poolSizes.push_back(poolSize);
		}
		if (m_DynamicUniformBufferCount > 0)
		{
			VkDescriptorPoolSize poolSize = {};
			poolSize.type				= VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
			poolSize.descriptorCount	= m_DynamicUniformBufferCount * m_SetCount;
			poolSizes.push_back(poolSize);
		}
		if (m_SampledImageCount > 0)
		{
			VkDescriptorPoolSize poolSize = {};
			poolSize.type				= VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
			poolSize.descriptorCount	= m_SampledImageCount * m_SetCount;
			poolSizes.push_back(poolSize);
		}
		if (m_SamplerCount > 0)
		{
			VkDescriptorPoolSize poolSize = {};
			poolSize.type				= VK_DESCRIPTOR_TYPE_SAMPLER;
			poolSize.descriptorCount	= m_SamplerCount * m_SetCount;
			poolSizes.push_back(poolSize);
		}
		if (m_CombinedImageSamplerCount > 0)
		{
			VkDescriptorPoolSize poolSize = {};
			poolSize.type				= VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			poolSize.descriptorCount	= m_CombinedImageSamplerCount * m_SetCount;
			poolSizes.push_back(poolSize);
		}

		//Create descriptorpool
		VkDescriptorPoolCreateInfo descriptorPoolInfo = {};
		descriptorPoolInfo.sType			= VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		descriptorPoolInfo.flags			= 0;
		descriptorPoolInfo.pNext			= nullptr;
		descriptorPoolInfo.poolSizeCount	= uint32(poolSizes.size());
		descriptorPoolInfo.pPoolSizes		= poolSizes.data();
		descriptorPoolInfo.maxSets			= m_SetCount;
		if (vkCreateDescriptorPool(m_pDevice->GetVkDevice(), &descriptorPoolInfo, nullptr, &pool) != VK_SUCCESS)
		{
			if (m_Pool == VK_NULL_HANDLE)
			{
				LOG_DEBUG_ERROR("Vulkan: Failed to create DescriptorPool\n");
			}
			else
			{
				LOG_DEBUG_ERROR("Vulkan: Failed to reallocate DescriptorPool\n");
			}
		}
		else
		{
			if (m_Pool == VK_NULL_HANDLE)
			{
				LOG_DEBUG_INFO("Vulkan: Created DescriptorPool\n");
			}
			else
			{
				LOG_DEBUG_INFO("Vulkan: Reallocated DescriptorPool\n");
				m_pDevice->SafeReleaseVulkanResource<VkDescriptorPool>(m_Pool);
			}

			m_Pool = pool;
			m_NumSets = m_SetCount;
		}
	}
}
