#include "LambdaPch.h"
#include "VulkanUploadBuffer.h"
#include "VulkanGraphicsDevice.h"
#include "VulkanUtilities.h"

namespace Lambda
{    
    VulkanUploadBuffer::VulkanUploadBuffer(const VulkanGraphicsDevice* pVkDevice, uint64 sizeInBytes)
        : m_pStart(nullptr),
        m_pCurrent(nullptr),
        m_Buffer(VK_NULL_HANDLE),
        m_Memory(VK_NULL_HANDLE),
        m_SizeInBytes(0),
		m_IsMapped(false)
    {
		LAMBDA_ASSERT(pVkDevice != nullptr);
        Init(pVkDevice, sizeInBytes);
    }


	void VulkanUploadBuffer::Map(VkDevice device)
	{
		if (!m_IsMapped)
		{
			vkMapMemory(device, m_Memory, 0, m_SizeInBytes, 0, reinterpret_cast<void**>(&m_pStart));
			m_IsMapped = true;

			Reset();
		}
	}


	void VulkanUploadBuffer::Unmap(VkDevice device)
	{
		if (m_IsMapped)
		{
			vkUnmapMemory(device, m_Memory);
			m_IsMapped = false;
		}
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
        m_Memory = pVkDevice->AllocateBuffer(m_Buffer, RESOURCE_USAGE_DYNAMIC);
        if (m_Memory != VK_NULL_HANDLE)
        {
            vkBindBufferMemory(device, m_Buffer, m_Memory, 0);
			Map(device);
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
        
        //Destroy buffer
        if (m_Buffer != VK_NULL_HANDLE)
        {
            vkDestroyBuffer(device, m_Buffer, nullptr);
            m_Buffer = VK_NULL_HANDLE;
        }
        if (m_Memory != VK_NULL_HANDLE)
        {
			Unmap(device);

			vkFreeMemory(device, m_Memory, nullptr);
            m_Memory = VK_NULL_HANDLE;
        }
        
        LOG_DEBUG_INFO("Vulkan: Destroyed UploadBuffer\n");

		delete this;
    }
}
