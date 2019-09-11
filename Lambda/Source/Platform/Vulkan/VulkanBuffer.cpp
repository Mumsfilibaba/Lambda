#include "LambdaPch.h"
#include "VulkanBuffer.h"
#include "VulkanUtilities.h"
#include "VulkanGraphicsDevice.h"

namespace Lambda
{
    VulkanBuffer::VulkanBuffer(const VulkanGraphicsDevice* pVkDevice, IVulkanAllocator* pAllocator, const BufferDesc& desc)
		: m_pAllocator(pAllocator),
		m_Device(VK_NULL_HANDLE),
		m_Buffer(VK_NULL_HANDLE),
		m_Memory(),
		m_Desc()
    {
		LAMBDA_ASSERT(pVkDevice != nullptr);
		LAMBDA_ASSERT(pAllocator != nullptr);
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
        

		//Allocate memory
		VkMemoryRequirements memRequirements = {};
		vkGetBufferMemoryRequirements(m_Device, m_Buffer, &memRequirements);

		//Set memoryproperty based on resource usage
		VkMemoryPropertyFlags properties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
		if (desc.Usage == RESOURCE_USAGE_DYNAMIC)
		{
			properties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
		}

		VkPhysicalDevice physicalDevice = pVkDevice->GetPhysicalDevice();
		uint32 memoryType = FindMemoryType(physicalDevice, memRequirements.memoryTypeBits, properties);
		m_Memory = m_pAllocator->Allocate(memRequirements.size, memRequirements.alignment, memoryType);
        if (m_Memory.Memory != VK_NULL_HANDLE)
		{
            vkBindBufferMemory(m_Device, m_Buffer, m_Memory.Memory, m_Memory.Offset);
        }
    }
    
    
    void VulkanBuffer::Map(void** ppMem)
    {
		LAMBDA_ASSERT(ppMem != nullptr);
		m_pAllocator->Map(m_Memory, ppMem);
    }
    
    
    void VulkanBuffer::Unmap()
    {
		m_pAllocator->Unmap(m_Memory);
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
