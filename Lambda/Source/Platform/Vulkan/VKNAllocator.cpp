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

	VKNMemoryChunk::VKNMemoryChunk(uint32 id, uint64 sizeInBytes, uint32 memoryType, ResourceUsage usage)
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

		VKNDevice& device = VKNDevice::GetInstance();
		if (vkAllocateMemory(device.GetDevice(), &allocInfo, nullptr, &m_DeviceMemory) != VK_SUCCESS)
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


	bool VKNMemoryChunk::Allocate(VKNMemory* pAllocation, uint64 sizeInBytes, uint64 alignment)
	{
        LAMBDA_ASSERT(pAllocation != nullptr);
        
        VkDeviceSize offset         = 0;
        VkDeviceSize padding        = 0;
        VKNMemoryBlock* pBestFit = nullptr;
        
        //Find enough free space, and find the block that best fits
        for (VKNMemoryBlock* pCurrent = m_pBlockHead; pCurrent != nullptr; pCurrent = pCurrent->pNext)
        {
            //Check if the block is allocated or not
            if (!pCurrent->IsFree)
                continue;
            
            //Align the offset
            offset  = Math::AlignUp<uint64>(pCurrent->DeviceMemoryOffset, alignment);
            padding = offset - pCurrent->DeviceMemoryOffset;
            
            if (pCurrent->Size <= padding + sizeInBytes)
                continue;
            
            //Set bestfit
            if (pBestFit)
            {
                if (pBestFit->Size <= pCurrent->Size)
                    continue;
            }
            
            pBestFit = pCurrent;
        }
        

        //Did we find a suitable block to make the allocation?
        if (!pBestFit)
        {
            return false;
		}

        
        //Create a block for the padding (Allocating a texture in a block can result in a padding of 256 bytes which is enough for a buffer)
        if (padding > 0)
        {
			//Save the old block (Will be padding)
            VKNMemoryBlock* pPadding		= pBestFit;

			//Create a new block that fill house the allocation
			pBestFit						= DBG_NEW VKNMemoryBlock();
			pBestFit->ID                    = m_BlockCount++;
			pBestFit->DeviceMemoryOffset    = offset;
			pBestFit->Size                  = pPadding->Size - padding;
			pBestFit->IsFree                = true;
            
			//Resize padding block
			pPadding->Size = padding;

			//Set pointers
			pBestFit->pNext		= pPadding->pNext;
			pBestFit->pPrevious = pPadding;

            pPadding->pNext     = pBestFit;
        }
        
        
        if (pBestFit->Size > sizeInBytes)
        {
            //Create a new block after allocation
            VKNMemoryBlock* pBlock = DBG_NEW VKNMemoryBlock();
            pBlock->ID                  = m_BlockCount++;
            pBlock->Size                = pBestFit->Size - sizeInBytes;
            pBlock->DeviceMemoryOffset  = pBestFit->DeviceMemoryOffset + sizeInBytes;
            pBlock->IsFree              = true;
            
			//Set pointers
            pBlock->pNext       = pBestFit->pNext;
            pBlock->pPrevious   = pBestFit;
            
            pBestFit->Size      = sizeInBytes;
            pBestFit->pNext     = pBlock;
        }

        
        //Setup allocation
        pAllocation->BlockID            = pBestFit->ID;
        pAllocation->ChunkID            = m_ID;
        pAllocation->DeviceMemory       = m_DeviceMemory;
        pAllocation->DeviceMemoryOffset = pBestFit->DeviceMemoryOffset;
        pAllocation->Size               = pBestFit->Size;
        if (m_Usage == RESOURCE_USAGE_DYNAMIC)
        {
            pAllocation->pHostMemory = m_pHostMemory + pAllocation->DeviceMemoryOffset;
        }
        else
        {
            pAllocation->pHostMemory = nullptr;
        }

        //Set bestfit to be used
        pBestFit->IsFree = false;
        
		return true;
	}


	void VKNMemoryChunk::Map()
	{
		//If not mapped -> map
		if (!m_IsMapped)
		{
			VKNDevice& device = VKNDevice::GetInstance();

			void* pMemory = nullptr;
			vkMapMemory(device.GetDevice(), m_DeviceMemory, 0, VK_WHOLE_SIZE, 0, &pMemory);

			m_pHostMemory   = reinterpret_cast<uint8*>(pMemory);
			m_IsMapped      = true;
		}
	}


	void VKNMemoryChunk::Unmap()
	{
		//If mapped -> unmap
		if (m_IsMapped)
		{
			VKNDevice& device = VKNDevice::GetInstance();
			vkUnmapMemory(device.GetDevice(), m_DeviceMemory);

			m_pHostMemory   = nullptr;
			m_IsMapped      = false;
		}
	}


	void VKNMemoryChunk::Deallocate(VKNMemory* pAllocation)
	{
        //Try to find the correct block
        VKNMemoryBlock* pCurrent = m_pBlockHead;
		while (pCurrent)
        {
			if (pCurrent->ID == uint32(pAllocation->BlockID))
			{
				break;
			}
			
			pCurrent = pCurrent->pNext;
        }

		if (!pCurrent)
		{
			return;
		}
		
		//Set this block to free
		pCurrent->IsFree = true;

		//Try to merge blocks
		VKNMemoryBlock* pNext		= pCurrent->pNext;
		VKNMemoryBlock* pPrevious	= pCurrent->pPrevious;
		if (pNext)
		{
			//Merge blocks
			if (pNext->IsFree)
			{
				//Set size
				pNext->Size					= pNext->Size + pCurrent->Size;
				pNext->DeviceMemoryOffset	= pNext->DeviceMemoryOffset - pCurrent->Size;

				//Set pointers
				pNext->pPrevious = pPrevious;
				if (pPrevious)
					pPrevious->pNext = pNext;	
				if (m_pBlockHead == pCurrent)
					m_pBlockHead = pNext;

				//Remove block
				delete pCurrent;

				//Set values to continue the function
				pCurrent	= pNext;
				pNext		= pCurrent->pNext;
			}
		}
		if (pPrevious)
		{
			//Merge blocks
			if (pPrevious->IsFree)
			{
				//Set size
				pPrevious->Size = pPrevious->Size + pCurrent->Size;

				//Set pointers
				pPrevious->pNext = pNext;
				if (pNext)
					pNext->pPrevious = pPrevious;
				if (m_pBlockHead == pCurrent)
					m_pBlockHead = pPrevious;

				//Remove block
				delete pCurrent;

				//Set values to continue the function
				pCurrent = nullptr;
			}
		}

		//Invalidate memory
		pAllocation->BlockID			= -1;
		pAllocation->ChunkID			= -1;
		pAllocation->DeviceMemoryOffset = 0;
		pAllocation->Size				= 0;
		pAllocation->DeviceMemory		= VK_NULL_HANDLE;
		pAllocation->pHostMemory		= nullptr;
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


	uint32 VKNMemoryChunk::GetMemoryType() const
	{
		return m_MemoryType;
	}


	//------------
	//VKNAllocator
	//------------


	VKNAllocator::VKNAllocator()
		: m_MaxAllocations(0),
		m_TotalAllocated(0),
		m_TotalReserved(0),
		m_Chunks(),
		m_MemoryToDeallocate()
	{
		VKNDevice& device = VKNDevice::GetInstance();

		uint32 frameCount = device.GetDeviceSettings().FramesAhead;
		m_MemoryToDeallocate.resize(frameCount);

		VkPhysicalDeviceProperties properties = device.GetPhysicalDeviceProperties();
		m_MaxAllocations = properties.limits.maxMemoryAllocationCount;
	}


	bool VKNAllocator::Allocate(VKNMemory* pAllocation, const VkMemoryRequirements& memoryRequirements, ResourceUsage usage)
	{
		//Set memoryproperty based on resource usage
		VkMemoryPropertyFlags properties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
		if (usage == RESOURCE_USAGE_DYNAMIC)
		{
			properties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
		}

		//Add to total
		m_TotalAllocated += memoryRequirements.size;

		//Get needed memorytype
		VKNDevice& device = VKNDevice::GetInstance();
		uint32 memoryType = FindMemoryType(device.GetPhysicalDevice(), memoryRequirements.memoryTypeBits, properties);

		//Try allocating from existing chunk
		for (auto chunk : m_Chunks)
		{
			if (chunk->GetMemoryType() == memoryType)
			{
				if (chunk->Allocate(pAllocation, memoryRequirements.size, memoryRequirements.alignment))
				{
					return true;
				}
			}
		}
        
		LAMBDA_ASSERT_PRINT(m_Chunks.size() < m_MaxAllocations, "Max number of allocations already reached\n");
		
		//If allocated is large, make a dedicated allocation
		uint64 bytesToReserve = MB(256);
		if (memoryRequirements.size > bytesToReserve)
		{
			bytesToReserve = memoryRequirements.size;
		}

		//Add to total
		m_TotalReserved += bytesToReserve;
		
		//Allocate new chunk
		VKNMemoryChunk* pChunk = DBG_NEW VKNMemoryChunk(uint32(m_Chunks.size()), bytesToReserve, memoryType, usage);
		m_Chunks.emplace_back(pChunk);
		
		LOG_SYSTEM(LOG_SEVERITY_WARNING, " [DEVICE ALLOCATOR] Allocated Memory chunk. Number of allocations: '%llu'. Max allocations: '%llu'. Memory type: %d, \n", m_Chunks.size(), m_MaxAllocations, memoryType);
		LOG_SYSTEM(LOG_SEVERITY_WARNING, " [DEVICE ALLOCATOR] TotalAllocated: %.2fMB. TotalReserved %.2fMB\n", float(m_TotalAllocated) / mb, float(m_TotalReserved) / mb);

		return m_Chunks.back()->Allocate(pAllocation, memoryRequirements.size, memoryRequirements.alignment);
	}


	void VKNAllocator::Deallocate(VKNMemory* pAllocation)
	{
        if (size_t(pAllocation->ChunkID) < m_Chunks.size() && pAllocation->DeviceMemory != VK_NULL_HANDLE)
        {
            VKNMemoryChunk* pChunk = m_Chunks[pAllocation->ChunkID];
            pChunk->Deallocate(pAllocation);

			m_TotalAllocated -= pAllocation->Size;
        }
	}

	
	void VKNAllocator::DefferedDeallocate(VKNMemory* pAllocation, uint64 frameCount)
	{
		//Set it to be removed
        if (size_t(pAllocation->ChunkID) < m_Chunks.size() && pAllocation->DeviceMemory != VK_NULL_HANDLE)
        {
            m_MemoryToDeallocate[frameCount].emplace_back(*pAllocation);
        }

		//Invalidate memory
		pAllocation->BlockID			= -1;
		pAllocation->ChunkID			= -1;
		pAllocation->DeviceMemoryOffset = 0;
		pAllocation->Size				= 0;
		pAllocation->DeviceMemory		= VK_NULL_HANDLE;
		pAllocation->pHostMemory		= nullptr;
	}


	void VKNAllocator::CleanGarbageMemory(uint64 frameCount)
	{
		auto& memoryBlocks = m_MemoryToDeallocate[frameCount];
		if (memoryBlocks.size() > 0)
		{
			for (auto& memory : memoryBlocks)
			{
				Deallocate(&memory);
			}
			memoryBlocks.clear();
		}
	}


	void VKNAllocator::Destroy(VkDevice device)
	{
		LAMBDA_ASSERT(device != VK_NULL_HANDLE);

		//Cleanup all garbage memory before deleting
		for (auto& memoryBlocks : m_MemoryToDeallocate)
		{
			if (memoryBlocks.size() > 0)
			{
				for (auto& memory : memoryBlocks)
				{
					Deallocate(&memory);
				}
				memoryBlocks.clear();
			}
		}

		//Delete allocator
		LOG_SYSTEM(LOG_SEVERITY_WARNING, "Vulkan: Deleting DeviceAllocator. Number of chunks: %u\n", m_Chunks.size());
		for (auto chunk : m_Chunks)
		{
			if (chunk)
			{
				chunk->Destroy(device);
			}
		}

		delete this;
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


	VKNDescriptorSetAllocator::VKNDescriptorSetAllocator(uint32 uniformBufferCount, uint32 dynamicUniformBufferCount, uint32 samplerCount, uint32 sampledImageCount, uint32 numSets)
		: m_Pool(VK_NULL_HANDLE),
		m_NumSets(0),
		m_UniformBufferCount(uniformBufferCount),
		m_DynamicUniformBufferCount(dynamicUniformBufferCount),
		m_SamplerCount(samplerCount),
		m_SampledImageCount(sampledImageCount),
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

		VKNDevice& device = VKNDevice::GetInstance();
		if (vkAllocateDescriptorSets(device.GetDevice(), &descriptorAllocInfo, &set))
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
		constexpr uint32 poolCount = 4;
		VkDescriptorPoolSize poolSizes[poolCount] = {};
		poolSizes[0].type				= VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		poolSizes[0].descriptorCount	= m_UniformBufferCount * m_SetCount;
		poolSizes[1].type				= VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
		poolSizes[1].descriptorCount	= m_DynamicUniformBufferCount * m_SetCount;
		poolSizes[2].type				= VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
		poolSizes[2].descriptorCount	= m_SampledImageCount * m_SetCount;
		poolSizes[3].type				= VK_DESCRIPTOR_TYPE_SAMPLER;
		poolSizes[3].descriptorCount	= m_SamplerCount * m_SetCount;

		//Create descriptorpool
		VkDescriptorPoolCreateInfo descriptorPoolInfo = {};
		descriptorPoolInfo.sType			= VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		descriptorPoolInfo.flags			= 0;
		descriptorPoolInfo.pNext			= nullptr;
		descriptorPoolInfo.poolSizeCount	= poolCount;
		descriptorPoolInfo.pPoolSizes		= poolSizes;
		descriptorPoolInfo.maxSets			= m_SetCount;

		VKNDevice& device = VKNDevice::GetInstance();
		if (vkCreateDescriptorPool(device.GetDevice(), &descriptorPoolInfo, nullptr, &pool) != VK_SUCCESS)
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

		VKNDevice& device = VKNDevice::GetInstance();

		DeviceSettings settings = device.GetDeviceSettings();
		m_FrameCount = settings.FramesAhead;

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
			VKNDevice& device = VKNDevice::GetInstance();
			for (auto& pool : oldpools)
			{
				vkDestroyDescriptorPool(device.GetDevice(), pool, nullptr);
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
