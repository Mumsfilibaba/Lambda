#include "LambdaPch.h"
#include "VulkanBuffer.h"
#include "VulkanUtilities.h"
#include "VulkanGraphicsDevice.h"

namespace Lambda
{
	//------------
	//VulkanBuffer
	//------------

    VulkanBuffer::VulkanBuffer(VkDevice device, IVulkanAllocator* pAllocator, const BufferDesc& desc)
		: m_pAllocator(pAllocator),
		m_Buffer(VK_NULL_HANDLE),
		m_Memory(),
		m_Desc()
    {
		LAMBDA_ASSERT(pAllocator != nullptr);
		LAMBDA_ASSERT(device != VK_NULL_HANDLE);
        Init(device, desc);
    }
    
    
    void VulkanBuffer::Init(VkDevice device, const BufferDesc& desc)
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
        
		if (vkCreateBuffer(device, &info, nullptr, &m_Buffer) != VK_SUCCESS)
        {
            LOG_DEBUG_ERROR("Vulkan: Failed to create Buffer\n");
            return;
        }
        else
        {
            LOG_DEBUG_INFO("Vulkan: Created Buffer\n");
            m_Desc = desc;
        }
        
		//Allocate memory
		VkMemoryRequirements memoryRequirements = {};
		vkGetBufferMemoryRequirements(device, m_Buffer, &memoryRequirements);

		m_Memory = m_pAllocator->Allocate(memoryRequirements, desc.Usage);
        if (m_Memory.Memory != VK_NULL_HANDLE)
		{
            vkBindBufferMemory(device, m_Buffer, m_Memory.Memory, m_Memory.Offset);
        }
    }
    
    
    void VulkanBuffer::Map(void** ppMem)
    {
		LAMBDA_ASSERT(ppMem != nullptr);
		(*ppMem) = m_Memory.pMemory;
    }
    
    
    void VulkanBuffer::Unmap()
    {
		//Unmapping a vulkan buffer does nothing since a whole memoryblock is mapped and stays mapped during its lifetime.
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
			m_pAllocator->Deallocate(m_Memory);

            vkDestroyBuffer(device, m_Buffer, nullptr);
            m_Buffer = VK_NULL_HANDLE;
        }
    }
    
    
    void VulkanBuffer::Destroy(VkDevice device)
    {
        Release(device);
        delete this;
    }
}
