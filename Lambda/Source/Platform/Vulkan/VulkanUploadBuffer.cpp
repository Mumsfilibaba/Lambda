#include "LambdaPch.h"
#include "VulkanUploadBuffer.h"
#include "VulkanUtilities.h"

namespace Lambda
{
    VulkanUploadBuffer::VulkanUploadBuffer()
		: m_pStart(nullptr),
		m_pCurrent(nullptr),
		m_Buffer(VK_NULL_HANDLE),
		m_Memory(VK_NULL_HANDLE),
		m_SizeInBytes(0)
    {
    }
    
    
    VulkanUploadBuffer::VulkanUploadBuffer(VkDevice device, VkPhysicalDevice adapter, uint64 sizeInBytes)
        : m_pStart(nullptr),
        m_pCurrent(nullptr),
        m_Buffer(VK_NULL_HANDLE),
        m_Memory(VK_NULL_HANDLE),
        m_SizeInBytes(0)
    {
        Init(device, adapter, sizeInBytes);
    }

    
    bool VulkanUploadBuffer::Init(VkDevice device, VkPhysicalDevice adapter, uint64 sizeInBytes)
    {
        assert(device != VK_NULL_HANDLE);
        assert(adapter != VK_NULL_HANDLE);
        
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
        
        VkMemoryRequirements memReq = {};
        vkGetBufferMemoryRequirements(device, m_Buffer, &memReq);
        
		//Allocate memory
        uint32 memoryType = FindMemoryType(adapter, memReq.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
        VkMemoryAllocateInfo allocInfo = {};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.pNext = nullptr;
        allocInfo.memoryTypeIndex = memoryType;
        allocInfo.allocationSize = memReq.size;

        if (vkAllocateMemory(device, &allocInfo, nullptr, &m_Memory) != VK_SUCCESS)
        {
            LOG_DEBUG_ERROR("Vulkan: Failed to allocate UploadBuffer\n");
            return false;
        }
        else
        {
            LOG_DEBUG_INFO("Vulkan: Allocated '%u' bytes for UploadBuffer\n", m_SizeInBytes);
            
            vkBindBufferMemory(device, m_Buffer, m_Memory, 0);
            
            vkMapMemory(device, m_Memory, 0, m_SizeInBytes, 0, reinterpret_cast<void**>(&m_pStart));
            m_pCurrent = m_pStart;
            
            return true;
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
        assert(device != VK_NULL_HANDLE);
        
        //Destroy buffer
        if (m_Buffer != VK_NULL_HANDLE)
        {
            vkDestroyBuffer(device, m_Buffer, nullptr);
            m_Buffer = VK_NULL_HANDLE;
        }
        if (m_Memory != VK_NULL_HANDLE)
        {
            vkUnmapMemory(device, m_Memory);
         
			vkFreeMemory(device, m_Memory, nullptr);
            m_Memory = VK_NULL_HANDLE;
        }
        
        LOG_DEBUG_INFO("Vulkan: Destroyed UploadBuffer\n");
    }
}
