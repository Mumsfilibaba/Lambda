#include "LambdaPch.h"
#include "VulkanBuffer.h"
#include "VulkanUtilities.h"
#include "VulkanGraphicsDevice.h"

namespace Lambda
{
    VulkanBuffer::VulkanBuffer(const VulkanGraphicsDevice* pVkDevice, const BufferDesc& desc)
		: m_Device(VK_NULL_HANDLE),
		m_Buffer(VK_NULL_HANDLE),
		m_BufferMemory(VK_NULL_HANDLE),
		m_Desc()
    {
		LAMBDA_ASSERT(pVkDevice != VK_NULL_HANDLE);
        Init(pVkDevice, desc);
    }
    
    
    void VulkanBuffer::Init(const VulkanGraphicsDevice* pVkDevice, const BufferDesc& desc)
    {
        //Create buffer
        VkBufferCreateInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        info.pNext = nullptr;
        info.flags = 0;
        info.queueFamilyIndexCount = 0;
        info.pQueueFamilyIndices = nullptr;
        info.size = desc.SizeInBytes;
        info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        
		info.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
        if (desc.Flags & BUFFER_FLAGS_VERTEX_BUFFER)
            info.usage |= VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
        if (desc.Flags & BUFFER_FLAGS_INDEX_BUFFER)
            info.usage |= VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
        if (desc.Flags & BUFFER_FLAGS_CONSTANT_BUFFER)
            info.usage |= VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
        
		VkDevice device = reinterpret_cast<VkDevice>(pVkDevice->GetNativeHandle());
        if (vkCreateBuffer(device, &info, nullptr, &m_Buffer) != VK_SUCCESS)
        {
            LOG_DEBUG_ERROR("Vulkan: Failed to create Buffer\n");
            return;
        }
        else
        {
            LOG_DEBUG_INFO("Vulkan: Created Buffer\n");
            m_Desc = desc;
			m_Device = device;
        }
         

		m_BufferMemory = pVkDevice->AllocateBuffer(m_Buffer, desc.Usage);
        if (m_BufferMemory != VK_NULL_HANDLE)
		{
            vkBindBufferMemory(device, m_Buffer, m_BufferMemory, 0);
        }
    }
    
    
    void VulkanBuffer::Map(void** ppMem)
    {
		LAMBDA_ASSERT(ppMem != nullptr);
        vkMapMemory(m_Device, m_BufferMemory, 0, m_Desc.SizeInBytes, 0, ppMem);
    }
    
    
    void VulkanBuffer::Unmap()
    {
        vkUnmapMemory(m_Device, m_BufferMemory);
    }
    
    
    void* VulkanBuffer::GetNativeHandle() const
    {
        return reinterpret_cast<void*>(m_Buffer);
    }

    
    BufferDesc VulkanBuffer::GetDesc() const
    {
        return m_Desc;
    }
    
    
    void VulkanBuffer::Release(VkDevice device)
    {
		LAMBDA_ASSERT(device != VK_NULL_HANDLE);
        
        if (m_Buffer != VK_NULL_HANDLE)
        {
            vkDestroyBuffer(device, m_Buffer, nullptr);
            m_Buffer = VK_NULL_HANDLE;
        }
        if (m_BufferMemory != VK_NULL_HANDLE)
        {
            vkFreeMemory(device, m_BufferMemory, nullptr);
            m_BufferMemory = VK_NULL_HANDLE;
        }
    }
    
    
    void VulkanBuffer::Destroy(VkDevice device)
    {
        Release(device);
        delete this;
    }
}
