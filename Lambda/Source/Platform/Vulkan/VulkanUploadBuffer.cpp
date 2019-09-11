#include "LambdaPch.h"
#include "VulkanUploadBuffer.h"
#include "VulkanGraphicsDevice.h"
#include "VulkanUtilities.h"

namespace Lambda
{    
    VulkanUploadBuffer::VulkanUploadBuffer(const VulkanGraphicsDevice* pVkDevice, IVulkanAllocator* pAllocator, uint64 sizeInBytes)
        : m_pAllocator(pAllocator),
		m_pStart(nullptr),
        m_pCurrent(nullptr),
        m_Buffer(VK_NULL_HANDLE),
        m_Memory(),
        m_SizeInBytes(0)
    {
		LAMBDA_ASSERT(pVkDevice != nullptr);
		LAMBDA_ASSERT(pAllocator != nullptr);
        Init(pVkDevice, sizeInBytes);
    }


	void VulkanUploadBuffer::Map()
	{
		m_pAllocator->Map(m_Memory, reinterpret_cast<void**>(&m_pStart));
		Reset();
	}


	void VulkanUploadBuffer::Unmap()
	{
		m_pAllocator->Unmap(m_Memory);
	}

    
    bool VulkanUploadBuffer::Init(const VulkanGraphicsDevice* pVkDevice, uint64 sizeInBytes)
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
        
        VkDevice device = reinterpret_cast<VkDevice>(pVkDevice->GetNativeHandle());
        if (vkCreateBuffer(device, &info, nullptr, &m_Buffer) != VK_SUCCESS)
        {
            LOG_DEBUG_ERROR("Vulkan: Failed to create buffer for UploadBuffer\n");
            return false;
        }
        else
        {
            LOG_DEBUG_INFO("Vulkan: Created buffer for UploadBuffer\n");
            m_SizeInBytes = sizeInBytes;
        }
        
        
		//Allocate memory
		VkPhysicalDevice physicalDevice = pVkDevice->GetPhysicalDevice();

		VkMemoryRequirements memRequirements = {};
		vkGetBufferMemoryRequirements(device, m_Buffer, &memRequirements);

		VkMemoryPropertyFlags properties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
		uint32 memoryType = FindMemoryType(physicalDevice, memRequirements.memoryTypeBits, properties);
		
		m_Memory = m_pAllocator->Allocate(memRequirements.size, memRequirements.alignment, memoryType);
		if (m_Memory.Memory != VK_NULL_HANDLE)
		{
			vkBindBufferMemory(device, m_Buffer, m_Memory.Memory, m_Memory.Offset);
			
			Map();
			return true;
		}
		else
		{
			return false;
		}
    }
    
    
    void* VulkanUploadBuffer::Allocate(uint64 bytesToAllocate)
    {
        //Do we have enough space?
        if ((m_pStart + m_SizeInBytes) - (m_pCurrent + bytesToAllocate) < 0)
        {
            LOG_DEBUG_ERROR("Vulkan: UploadBuffer has no space. Current space left is '%u' bytes, allocation requested '%u' bytes\n", uint64((m_pStart + m_SizeInBytes) - m_pCurrent), bytesToAllocate);
            return nullptr;
        }
        else
        {
            void* pMem = reinterpret_cast<void*>(m_pCurrent);
            m_pCurrent += bytesToAllocate;

            return pMem;
        }
    }
    
    
    void VulkanUploadBuffer::Reset()
    {
        //Reset buffer by resetting current to the start
        m_pCurrent = m_pStart;
    }
    
    
    void VulkanUploadBuffer::Destroy(VkDevice device)
    {
		LAMBDA_ASSERT(device != VK_NULL_HANDLE);
        
        if (m_Buffer != VK_NULL_HANDLE)
        {
			m_pAllocator->Deallocate(m_Memory);

            vkDestroyBuffer(device, m_Buffer, nullptr);
            m_Buffer = VK_NULL_HANDLE;
        }
        
        LOG_DEBUG_INFO("Vulkan: Destroyed UploadBuffer\n");
		delete this;
    }
}
