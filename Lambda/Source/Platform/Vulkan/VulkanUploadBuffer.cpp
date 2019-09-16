#include "LambdaPch.h"
#include "VulkanUploadBuffer.h"
#include "VulkanGraphicsDevice.h"
#include "VulkanUtilities.h"

namespace Lambda
{    
	//------------------
	//VulkanUploadBuffer
	//------------------

    VulkanUploadBuffer::VulkanUploadBuffer(VkDevice device, IVulkanAllocator* pAllocator, uint64 sizeInBytes)
        : m_pAllocator(pAllocator),
        m_pCurrent(nullptr),
        m_Buffer(VK_NULL_HANDLE),
        m_Memory(),
        m_SizeInBytes(0)
    {
		LAMBDA_ASSERT(pAllocator != nullptr);
		LAMBDA_ASSERT(device != VK_NULL_HANDLE);
        Init(device, sizeInBytes);
    }

    
    bool VulkanUploadBuffer::Init(VkDevice device, uint64 sizeInBytes)
    {
		//Create buffer
        VkBufferCreateInfo info = {};
        info.sType                  = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        info.pNext                  = nullptr;
        info.flags                  = 0;
        info.queueFamilyIndexCount  = 0;
        info.pQueueFamilyIndices    = nullptr;
        info.sharingMode            = VK_SHARING_MODE_EXCLUSIVE;
        info.size                   = sizeInBytes;
        info.usage                  = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
        
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
		VkMemoryRequirements memoryRequirements = {};
		vkGetBufferMemoryRequirements(device, m_Buffer, &memoryRequirements);
		if (m_pAllocator->Allocate(&m_Memory, memoryRequirements, RESOURCE_USAGE_DYNAMIC))
		{
			vkBindBufferMemory(device, m_Buffer, m_Memory.DeviceMemory, m_Memory.DeviceMemoryOffset);
            Reset();
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
        if (m_pCurrent + bytesToAllocate > m_Memory.pHostMemory + m_SizeInBytes)
        {
            LOG_DEBUG_ERROR("Vulkan: UploadBuffer has no space. Current space left is '%u' bytes, allocation requested '%u' bytes\n", uint64((m_Memory.pHostMemory + m_SizeInBytes) - m_pCurrent), bytesToAllocate);
            return nullptr;
        }
        else
        {
            void* pAllocation = reinterpret_cast<void*>(m_pCurrent);
            m_pCurrent += bytesToAllocate;
            return pAllocation;
        }
    }
    
    
    void VulkanUploadBuffer::Reset()
    {
        //Reset buffer by resetting current to the start
        m_pCurrent = m_Memory.pHostMemory;
    }
    
    
    void VulkanUploadBuffer::Destroy(VkDevice device)
    {
		LAMBDA_ASSERT(device != VK_NULL_HANDLE);
        
        if (m_Buffer != VK_NULL_HANDLE)
        {
			m_pAllocator->Deallocate(&m_Memory);

            vkDestroyBuffer(device, m_Buffer, nullptr);
            m_Buffer = VK_NULL_HANDLE;
        }
        
        LOG_DEBUG_INFO("Vulkan: Destroyed UploadBuffer\n");
		delete this;
    }
}
