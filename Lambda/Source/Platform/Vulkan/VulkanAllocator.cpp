#include "LambdaPch.h"
#include "Utilities/MathHelper.h"
#include "VulkanAllocator.h"
#include "VulkanUtilities.h"

namespace Lambda
{
	//-----------------
	//VulkanMemoryChunk
	//-----------------

	VulkanMemoryChunk::VulkanMemoryChunk(VkDevice device, uint32 id, uint64 sizeInBytes, uint64 memoryType, ResourceUsage usage)
		: m_Memory(VK_NULL_HANDLE),
		m_Usage(usage),
		m_ID(id),
		m_MemoryType(memoryType),
		m_SizeInBytes(sizeInBytes),
		m_Blocks(),
		m_pMemory(nullptr),
		m_IsMapped(false)
	{
		LAMBDA_ASSERT(device != VK_NULL_HANDLE);
		Init(device);
	}


	void VulkanMemoryChunk::Init(VkDevice device)
	{
		//Allocate device memory
		VkMemoryAllocateInfo allocInfo = {};
		allocInfo.sType				= VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.pNext				= nullptr;
		allocInfo.allocationSize	= m_SizeInBytes;
		allocInfo.memoryTypeIndex	= m_MemoryType;

		if (vkAllocateMemory(device, &allocInfo, nullptr, &m_Memory) != VK_SUCCESS)
		{
			LOG_DEBUG_ERROR("Vulkan: Failed to allocate MemoryChunk\n");
			return;
		}
		else
		{
			LOG_DEBUG_WARNING("Vulkan: Allocated '%d' bytes for MemoryChunk\n", allocInfo.allocationSize);
		}

		//Setup first block
		VulkanMemory allocation = {};
		allocation.ID		= m_ID;
		allocation.Size		= m_SizeInBytes;
		allocation.Offset	= 0;
		allocation.Memory	= m_Memory;

		if (m_Usage == RESOURCE_USAGE_DYNAMIC)
		{
			Map(device);
		}
		else
		{
			m_pMemory = nullptr;
		}

		allocation.pMemory = m_pMemory;
		m_Blocks.push_back(allocation);
	}


	bool VulkanMemoryChunk::CanAllocate(uint64 sizeInBytes, uint64 alignment)
	{
		//Check if this block can satisfy an allocation
		for (auto& block : m_Blocks)
		{
			uint64 requiredOffset = Math::AlignUp<uint64>(block.Offset, alignment);
			if ((requiredOffset + sizeInBytes) <= (block.Offset + block.Size))
			{
				return true;
			}
		}

		return false;
	}


	VulkanMemory VulkanMemoryChunk::Allocate(uint64 sizeInBytes, uint64 alignment)
	{
		//Find enough free space
		int32 blockIndex = -1;
		for (size_t i = 0; i < m_Blocks.size(); i++)
		{
			uint64 requiredOffset = Math::AlignUp<uint64>(m_Blocks[i].Offset, alignment);
			if ((requiredOffset + sizeInBytes) <= (m_Blocks[i].Offset + m_Blocks[i].Size))
			{
				blockIndex = i;
				break;
			}
		}

		//Did we find a block?
		VulkanMemory allocation = {};
		if (blockIndex < 0)
		{
			LOG_DEBUG_ERROR("Vulkan: Not enough space in allocation\n");

			allocation.ID		= -1;
			allocation.Size		= 0;
			allocation.Offset	= 0;
			allocation.Memory	= VK_NULL_HANDLE;
			allocation.pMemory	= nullptr;
			return allocation;
		}

		//Setup memory from block
		allocation.ID		= m_ID;
		allocation.Size		= sizeInBytes;
		allocation.Offset	= Math::AlignUp<uint64>(m_Blocks[blockIndex].Offset, alignment);
		allocation.Memory	= m_Memory;
		if (m_Usage == RESOURCE_USAGE_DYNAMIC)
		{
			allocation.pMemory = m_pMemory + allocation.Offset;
		}

		//Add padding as a new block
		uint64 alignmentPadding = allocation.Offset - m_Blocks[blockIndex].Offset;
		if (alignmentPadding > 0)
		{
			VulkanMemory newBlock = {};
			newBlock.ID		= m_ID;
			newBlock.Memory	= m_Memory;
			newBlock.Size	= alignmentPadding;
			newBlock.Offset = m_Blocks[blockIndex].Offset;
			if (m_Usage == RESOURCE_USAGE_DYNAMIC)
			{
				newBlock.pMemory = m_pMemory + newBlock.Offset;
			}

			m_Blocks.push_back(newBlock);
		}

		//Change size of block
		m_Blocks[blockIndex].Size	-= alignmentPadding + allocation.Size;
		m_Blocks[blockIndex].Offset = allocation.Offset + allocation.Size;
		return allocation;
	}


	void VulkanMemoryChunk::Map(VkDevice device)
	{
		//If not mapped -> map
		if (!m_IsMapped)
		{
			void* pMemory = nullptr;
			vkMapMemory(device, m_Memory, 0, VK_WHOLE_SIZE, 0, &pMemory);

			m_pMemory = reinterpret_cast<uint8*>(pMemory);
			m_IsMapped = true;
		}
	}


	void VulkanMemoryChunk::Unmap(VkDevice device)
	{
		//If mapped -> unmap
		if (m_IsMapped)
		{
			vkUnmapMemory(device, m_Memory);

			m_pMemory = nullptr;
			m_IsMapped = false;
		}
	}


	void VulkanMemoryChunk::Deallocate(const VulkanMemory& allocation)
	{
		//Search through the blocks and see if they come after or before eachother
		for (auto& block : m_Blocks)
		{
			if (block.Offset + block.Size == allocation.Offset)
			{
				block.Size += allocation.Size;
				return;
			}
			else if (allocation.Offset + allocation.Size == block.Offset)
			{
				block.Size		+= allocation.Size;
				block.Offset	-= allocation.Size;
				return;
			}
		}

		//Otherwise just push block
		m_Blocks.push_back(allocation);
	}


	void VulkanMemoryChunk::Destroy(VkDevice device)
	{
		LAMBDA_ASSERT(device != VK_NULL_HANDLE);

		if (m_Memory != VK_NULL_HANDLE)
		{
			Unmap(device);

			vkFreeMemory(device, m_Memory, nullptr);
			m_Memory = VK_NULL_HANDLE;

			LOG_SYSTEM(LOG_SEVERITY_WARNING, "Vulkan: Deallocated MemoryChunk\n");
		}

		delete this;
	}


	uint32 VulkanMemoryChunk::GetMemoryType() const
	{
		return m_MemoryType;
	}


	//---------------------
	//VulkanDeviceAllocator
	//---------------------


	VulkanDeviceAllocator::VulkanDeviceAllocator(VkDevice device, VkPhysicalDevice physicalDevice)
		: m_Device(device),
		m_PhysicalDevice(physicalDevice),
		m_MaxAllocations(0),
		m_Chunks()
	{
		LAMBDA_ASSERT(device != VK_NULL_HANDLE);
		LAMBDA_ASSERT(physicalDevice != VK_NULL_HANDLE);

		VkPhysicalDeviceProperties properties = {};
		vkGetPhysicalDeviceProperties(m_PhysicalDevice, &properties);

		m_MaxAllocations = properties.limits.maxMemoryAllocationCount;
	}


	VulkanMemory VulkanDeviceAllocator::Allocate(const VkMemoryRequirements& memoryRequirements, ResourceUsage usage)
	{
		LAMBDA_ASSERT_PRINT(memoryRequirements.size < MB(256), "Allocations must be smaller than 256MB\n");

		//Set memoryproperty based on resource usage
		VkMemoryPropertyFlags properties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
		if (usage == RESOURCE_USAGE_DYNAMIC)
		{
			properties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
		}

		//Try allocating from existing chunk
		uint32 memoryType = FindMemoryType(m_PhysicalDevice, memoryRequirements.memoryTypeBits, properties);
		for (auto chunk : m_Chunks)
		{
			if (chunk->GetMemoryType() == memoryType)
			{
				if (chunk->CanAllocate(memoryRequirements.size, memoryRequirements.alignment))
				{
					return chunk->Allocate(memoryRequirements.size, memoryRequirements.alignment);
				}
			}
		}

		//Allocate new chunk
		LAMBDA_ASSERT_PRINT(m_Chunks.size() < m_MaxAllocations, "Max number of allocations already reached\n");

		VulkanMemoryChunk* pChunk = DBG_NEW VulkanMemoryChunk(m_Device, uint32(m_Chunks.size()), MB(256), memoryType, usage);
		m_Chunks.emplace_back(pChunk);
		
		LOG_SYSTEM(LOG_SEVERITY_WARNING, " [DEVICE ALLOCATOR] Allocated Memory chunk. Number of allocations: '%llu'. Max allocations: '%llu'. Memory type: %d, \n", m_Chunks.size(), m_MaxAllocations, memoryType);

		return m_Chunks.back()->Allocate(memoryRequirements.size, memoryRequirements.alignment);
	}


	void VulkanDeviceAllocator::Deallocate(const VulkanMemory& allocation)
	{
		VulkanMemoryChunk* pChunk = m_Chunks[allocation.ID];
		pChunk->Deallocate(allocation);
	}


	void VulkanDeviceAllocator::Destroy(VkDevice device)
	{
		LAMBDA_ASSERT(device != VK_NULL_HANDLE);

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
}