#include "LambdaPch.h"
#include "Utilities/MathHelper.h"
#include "VKNDevice.h"
#include "VKNAllocator.h"
#include "VKNUtilities.h"

namespace Lambda
{
	//--------------
	//VKNMemoryChunk
	//--------------

	constexpr float mb = 1024.0f * 1024.0f;

	VKNMemoryChunk::VKNMemoryChunk(uint32 id, VkDeviceSize sizeInBytes, uint32 memoryType, ResourceUsage usage)
		: m_Usage(usage),
		m_ID(id),
		m_MemoryType(memoryType),
		m_SizeInBytes(sizeInBytes),
        m_BlockCount(0),
		m_pBlockHead(nullptr),
		m_pHostMemory(nullptr),
		m_IsMapped(false)
	{
		Init();
	}


	void VKNMemoryChunk::Init()
	{
		//Allocate device memory
		VkMemoryAllocateInfo allocInfo  = {};
		allocInfo.sType				    = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.pNext				    = nullptr;
		allocInfo.allocationSize	    = m_SizeInBytes;
		allocInfo.memoryTypeIndex	    = m_MemoryType;

		VKNDevice& device = VKNDevice::Get();
		if (vkAllocateMemory(device.GetVkDevice(), &allocInfo, nullptr, &m_DeviceMemory) != VK_SUCCESS)
		{
			LOG_DEBUG_ERROR("Vulkan: Failed to allocate MemoryChunk\n");
			return;
		}
		else
		{
			LOG_DEBUG_WARNING("Vulkan: Allocated '%d' bytes for MemoryChunk\n", allocInfo.allocationSize);
		}

		//Setup first block
        m_pBlockHead = DBG_NEW VKNMemoryBlock();
        m_pBlockHead->pNext					= nullptr;
        m_pBlockHead->pPrevious				= nullptr;
        m_pBlockHead->IsFree				= true;
        m_pBlockHead->ID					= m_BlockCount++;
		m_pBlockHead->Size					= m_SizeInBytes;
		m_pBlockHead->DeviceMemoryOffset    = 0;
		
		//If this is CPU visible -> Map
		if (m_Usage == RESOURCE_USAGE_DYNAMIC)
		{
			Map();
		}
	}


	bool VKNMemoryChunk::Allocate(VKNMemory& allocation, VkDeviceSize sizeInBytes, VkDeviceSize alignment, VkDeviceSize granularity)
	{
        VkDeviceSize padding = 0;
        VkDeviceSize paddedDeviceOffset = 0;
        VkDeviceSize paddedSizeInBytes  = 0;
        VKNMemoryBlock* pBestFit = nullptr;
        
        //Find enough free space, and find the block that best fits
        for (VKNMemoryBlock* pCurrent = m_pBlockHead; pCurrent != nullptr; pCurrent = pCurrent->pNext)
        {
            //Check if the block is allocated or not
            if (!pCurrent->IsFree)
                continue;
            
            //Does it fit into the block
            if (sizeInBytes > pCurrent->Size)
                continue;
            
            //Align the offset
            paddedDeviceOffset = Math::AlignUp<uint64>(pCurrent->DeviceMemoryOffset, alignment);
            
            //Take granularity into account
            if (pCurrent->pPrevious != nullptr && granularity > 1)
            {
                VKNMemoryBlock* pPrevious = pCurrent->pPrevious;
                if (IsOnSamePage(pPrevious->DeviceMemoryOffset, pPrevious->Size, paddedDeviceOffset, granularity))
                {
                    paddedDeviceOffset = Math::AlignUp(paddedDeviceOffset, granularity);
                }
            }
            
            //Calculate padding
            padding = paddedDeviceOffset - pCurrent->DeviceMemoryOffset;
            paddedSizeInBytes = sizeInBytes + padding;
            
            //Does it still fit
            if (paddedSizeInBytes > pCurrent->Size)
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
            
            //Set bestfit
            if (pBestFit)
            {
                if (pCurrent->Size > pBestFit->Size)
                {
                    continue;
                }
            }
            
            pBestFit = pCurrent;
        }

        //Did we find a suitable block to make the allocation?
        if (pBestFit == nullptr)
            return false;
        
        
        //        Free block
        //|--------------------------|
        //padding Allocation Remaining
        //|------|----------|--------|
        if (pBestFit->Size > paddedSizeInBytes)
        {
            //Create a new block after allocation
            VKNMemoryBlock* pBlock = DBG_NEW VKNMemoryBlock();
            pBlock->ID                  = m_BlockCount++;
            pBlock->Size                = pBestFit->Size - paddedSizeInBytes;
            pBlock->DeviceMemoryOffset  = pBestFit->DeviceMemoryOffset + sizeInBytes;
            pBlock->IsFree              = true;
            
            //Set pointers
            pBlock->pNext     = pBestFit->pNext;
            pBlock->pPrevious = pBestFit;
            if (pBestFit->pNext)
                pBestFit->pNext->pPrevious = pBlock;
            pBestFit->pNext = pBlock;
        }
        
        //Update bestfit
        pBestFit->Size   = sizeInBytes;
        pBestFit->IsFree = false;
        
        //Setup allocation
        allocation.BlockID            = pBestFit->ID;
        allocation.ChunkID            = m_ID;
        allocation.DeviceMemory       = m_DeviceMemory;
        allocation.DeviceMemoryOffset = paddedDeviceOffset;
        allocation.Size               = sizeInBytes;
        if (m_Usage == RESOURCE_USAGE_DYNAMIC)
            allocation.pHostMemory = m_pHostMemory + allocation.DeviceMemoryOffset;
        else
            allocation.pHostMemory = nullptr;

		return true;
	}


    bool VKNMemoryChunk::IsOnSamePage(VkDeviceSize aOffset, VkDeviceSize aSize, VkDeviceSize bOffset, VkDeviceSize pageSize)
    {
        LAMBDA_ASSERT( aOffset + aSize <= bOffset && aSize > 0 && pageSize > 0 );
        
        VkDeviceSize aEnd       = aOffset + (aSize-1);
        VkDeviceSize aEndPage   = aEnd & ~(pageSize-1);
        VkDeviceSize bStart     = bOffset;
        VkDeviceSize bStartPage = bStart & ~(pageSize-1);
        return aEndPage == bStartPage;
    }


	void VKNMemoryChunk::Map()
	{
		//If not mapped -> map
		if (!m_IsMapped)
		{
			VKNDevice& device = VKNDevice::Get();

			void* pMemory = nullptr;
			vkMapMemory(device.GetVkDevice(), m_DeviceMemory, 0, VK_WHOLE_SIZE, 0, &pMemory);

			m_pHostMemory   = reinterpret_cast<uint8*>(pMemory);
			m_IsMapped      = true;
		}
	}


	void VKNMemoryChunk::Unmap()
	{
		//If mapped -> unmap
		if (m_IsMapped)
		{
			VKNDevice& device = VKNDevice::Get();
			vkUnmapMemory(device.GetVkDevice(), m_DeviceMemory);

			m_pHostMemory = nullptr;
			m_IsMapped    = false;
		}
	}


	void VKNMemoryChunk::Deallocate(VKNMemory& allocation)
	{
        LOG_DEBUG_INFO("Vulkan: Deallocated block ID=%u\n", allocation.BlockID);
        
        //Try to find the correct block
        VKNMemoryBlock* pCurrent = m_pBlockHead;
		while (pCurrent)
        {
            //Did we find the correct block
			if (pCurrent->ID == uint32(allocation.BlockID))
				break;
            //Continue looking
            pCurrent = pCurrent->pNext;
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
                pPrevious->Size += pCurrent->Size;

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
               pCurrent->Size += pNext->Size;

                //Set pointers
                if (pNext->pNext)
                    pNext->pNext->pPrevious = pCurrent;
                pCurrent->pNext = pNext->pNext;

                //Remove block
                delete pNext;
            }
        }
	}


	void VKNMemoryChunk::Destroy(VkDevice device)
	{
		LAMBDA_ASSERT(device != VK_NULL_HANDLE);

		if (m_DeviceMemory != VK_NULL_HANDLE)
		{
			//Unmap
			Unmap();

			//Print memoryleaks
#if defined(LAMBDA_DEBUG)
			VKNMemoryBlock* pDebug = m_pBlockHead;
			LOG_DEBUG_WARNING("Allocated blocks left in MemoryChunk %u:\n", m_ID);
			while (pDebug)
			{
				LOG_DEBUG_WARNING("    VulkanBlock: ID=%u, Offset=%u, Size=%u, IsFree=%s\n", pDebug->ID, pDebug->DeviceMemoryOffset, pDebug->Size, pDebug->IsFree ? "True" : "False");
				pDebug = pDebug->pNext;
			}
#endif

			//Delete first block
			SafeDelete(m_pBlockHead);

			//Free memory
			vkFreeMemory(device, m_DeviceMemory, nullptr);
			m_DeviceMemory = VK_NULL_HANDLE;

			LOG_SYSTEM(LOG_SEVERITY_WARNING, "Vulkan: Deallocated MemoryChunk\n");
		}

		delete this;
	}

	//------------
	//VKNAllocator
	//------------

    constexpr size_t numFrames = 5;

	VKNAllocator::VKNAllocator(VKNDevice* pDevice)
		: DeviceObjectBase<VKNDevice, IVKNAllocator>(pDevice),
		m_MaxAllocations(0),
        m_TotalReserved(0),
        m_TotalAllocated(0),
        m_FrameIndex(0),
		m_Chunks(),
		m_MemoryToDeallocate()
	{
		//Add a ref to the refcounter
		this->AddRef();

        //Resize the number of garbage memory vectors
        m_MemoryToDeallocate.resize(numFrames);
        
        //Setup from properties of the device
		VkPhysicalDeviceProperties properties = pDevice->GetPhysicalDeviceProperties();
		m_MaxAllocations = properties.limits.maxMemoryAllocationCount;
        m_BufferImageGranularity = properties.limits.bufferImageGranularity;
	}


	VKNAllocator::~VKNAllocator()
	{
		//Cleanup all garbage memory before deleting
		for (uint32 i = 0; i < numFrames; i++)
			EmptyGarbageMemory();

		//Delete allocator
		LOG_SYSTEM(LOG_SEVERITY_WARNING, "Vulkan: Deleting DeviceAllocator. Number of chunks: %u\n", m_Chunks.size());
		for (auto chunk : m_Chunks)
		{
			if (chunk)
				chunk->Destroy(m_pDevice->GetVkDevice());
		}

		LOG_DEBUG_INFO("Vulkan: Destroyed buffer\n");
	}


	bool VKNAllocator::Allocate(VKNMemory& allocation, const VkMemoryRequirements& memoryRequirements, ResourceUsage usage)
	{
		//Set memoryproperty based on resource usage
		VkMemoryPropertyFlags properties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
		if (usage == RESOURCE_USAGE_DYNAMIC)
			properties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

		//Add to total
		m_TotalAllocated += memoryRequirements.size;

		//Get needed memorytype
		uint32 memoryType = FindMemoryType(m_pDevice->GetVkPhysicalDevice(), memoryRequirements.memoryTypeBits, properties);

        LOG_SYSTEM(LOG_SEVERITY_WARNING, "[VULKAN DEVICE ALLOCATOR] Allocated Memory chunk. Number of allocations: '%llu'. Max allocations: '%llu'. Memory type: %d. TotalAllocated: %.2fMB. TotalReserved %.2fMB\n", m_Chunks.size(), m_MaxAllocations, memoryType, float(m_TotalAllocated) / mb, float(m_TotalReserved) / mb);
        
		//Try allocating from existing chunk
		for (auto chunk : m_Chunks)
		{
			if (chunk->GetMemoryType() == memoryType)
			{
				if (chunk->Allocate(allocation, memoryRequirements.size, memoryRequirements.alignment, m_BufferImageGranularity))
				{
					return true;
				}
			}
		}
        
		LAMBDA_ASSERT_PRINT(m_Chunks.size() < m_MaxAllocations, "Max number of allocations already reached\n");
		
		//If allocated is large, make a dedicated allocation
		uint64 bytesToReserve = MB(256);
		if (memoryRequirements.size > bytesToReserve)
			bytesToReserve = memoryRequirements.size;

		//Add to total
		m_TotalReserved += bytesToReserve;
		
		//Allocate new chunk
		VKNMemoryChunk* pChunk = DBG_NEW VKNMemoryChunk(uint32(m_Chunks.size()), bytesToReserve, memoryType, usage);
		m_Chunks.emplace_back(pChunk);

        return pChunk->Allocate(allocation, memoryRequirements.size, memoryRequirements.alignment, m_BufferImageGranularity);
	}


	void VKNAllocator::Deallocate(VKNMemory& allocation)
	{
        //Set it to be removed
        if (size_t(allocation.ChunkID) < m_Chunks.size() && allocation.DeviceMemory != VK_NULL_HANDLE)
            m_MemoryToDeallocate[m_FrameIndex].emplace_back(allocation);

        //Invalidate memory
        allocation.BlockID            = -1;
        allocation.ChunkID            = -1;
        allocation.DeviceMemoryOffset = 0;
        allocation.Size               = 0;
        allocation.DeviceMemory       = VK_NULL_HANDLE;
        allocation.pHostMemory        = nullptr;
	}


	void VKNAllocator::EmptyGarbageMemory()
	{
        //Move on a frame
        m_FrameIndex = (m_FrameIndex+1) % numFrames;
        
        //Clean memory
		auto& memoryBlocks = m_MemoryToDeallocate[m_FrameIndex];
		if (memoryBlocks.size() > 0)
		{
            //Deallocate all the blocks
			for (auto& memory : memoryBlocks)
            {
                if (size_t(memory.ChunkID) < m_Chunks.size() && memory.DeviceMemory != VK_NULL_HANDLE)
                {
                    VKNMemoryChunk* pChunk = m_Chunks[memory.ChunkID];
                    pChunk->Deallocate(memory);
                    
                    m_TotalAllocated -= memory.Size;
                }
            }
            
			memoryBlocks.clear();
		}
	}


	uint64 VKNAllocator::GetTotalReserved() const
	{
		return m_TotalReserved;
	}


	uint64 VKNAllocator::GetTotalAllocated() const
	{
		return m_TotalAllocated;
	}
		
	//-------------------------
	//VKNDescriptorSetAllocator
	//-------------------------

	VKNDescriptorSetAllocator::VKNDescriptorSetAllocator(uint32 uniformBufferCount, uint32 dynamicUniformBufferCount, uint32 samplerCount, uint32 sampledImageCount, uint32 combinedImageSamplerCount, uint32 numSets)
		: m_Pool(VK_NULL_HANDLE),
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

		VKNDevice& device = VKNDevice::Get();
		if (vkAllocateDescriptorSets(device.GetVkDevice(), &descriptorAllocInfo, &set))
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
		{
			vkDestroyDescriptorPool(device, m_Pool, nullptr);
			m_Pool = VK_NULL_HANDLE;
		}

		delete this;
	}


	void VKNDescriptorSetAllocator::Init()
	{
		VkDescriptorPool pool = VK_NULL_HANDLE;

		//Setup descriptorcounts
		constexpr uint32 poolCount = 5;
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

		VKNDevice& device = VKNDevice::Get();
		if (vkCreateDescriptorPool(device.GetVkDevice(), &descriptorPoolInfo, nullptr, &pool) != VK_SUCCESS)
		{
			if (m_Pool == VK_NULL_HANDLE)
			{
				LOG_DEBUG_ERROR("Vulkan: Failed to create DescriptorPool\n");
			}
			else
			{
				LOG_DEBUG_ERROR("Vulkan: Failed to reallocate DescriptorPool\n");
			}

			return;
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

				VKNDescriptorPoolManager& descriptorPoolManager = VKNDescriptorPoolManager::GetInstance();
				descriptorPoolManager.DeallocatePool(m_Pool);
			}

			m_Pool = pool;
			m_NumSets = m_SetCount;
		}
	}

	//------------------------
	//VKNDescriptorPoolManager
	//------------------------

	VKNDescriptorPoolManager* VKNDescriptorPoolManager::s_pInstance = nullptr;

	VKNDescriptorPoolManager::VKNDescriptorPoolManager()
		: m_FrameCount(0),
		m_CurrentFrame(0),
		m_OldPools()
	{
		LAMBDA_ASSERT(s_pInstance == nullptr);
		s_pInstance = this;

		VKNDevice& device = VKNDevice::Get();
		const DeviceDesc& deviceDesc = device.GetDesc();
		m_FrameCount = FRAMES_AHEAD;

		m_OldPools.resize(m_FrameCount);
	}


	void VKNDescriptorPoolManager::DeallocatePool(VkDescriptorPool pool)
	{
		if (pool != VK_NULL_HANDLE)
		{
			auto& oldpools = m_OldPools[m_CurrentFrame];
			oldpools.push_back(pool);
		}
	}


	void VKNDescriptorPoolManager::Cleanup()
	{
		//Advance frame
		m_CurrentFrame = (m_CurrentFrame + 1) % m_FrameCount;
		
		//Cleanup old pools
		auto& oldpools = m_OldPools[m_CurrentFrame];
		if (oldpools.size() > 0)
		{
			VKNDevice& device = VKNDevice::Get();
			for (auto& pool : oldpools)
			{
				vkDestroyDescriptorPool(device.GetVkDevice(), pool, nullptr);
				pool = VK_NULL_HANDLE;
			}
		}
	}
	
	
	VKNDescriptorPoolManager& VKNDescriptorPoolManager::GetInstance()
	{
		LAMBDA_ASSERT(s_pInstance != nullptr);
		return *s_pInstance;
	}
}
