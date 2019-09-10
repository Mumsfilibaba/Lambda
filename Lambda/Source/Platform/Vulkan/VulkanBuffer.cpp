#include "LambdaPch.h"
#include "VulkanBuffer.h"
#include "VulkanUtilities.h"
#include "VulkanGraphicsDevice.h"

namespace Lambda
{
    VulkanBuffer::VulkanBuffer()
        : m_Buffer(VK_NULL_HANDLE),
        m_BufferMemory(VK_NULL_HANDLE),
        m_Desc()
    {
    }
    
    
    VulkanBuffer::VulkanBuffer(VkDevice device, VkPhysicalDevice adapter, const BufferDesc& desc)
        : m_Buffer(VK_NULL_HANDLE),
        m_BufferMemory(VK_NULL_HANDLE),
        m_Desc()
    {
		LAMBDA_ASSERT(device != VK_NULL_HANDLE);
        Init(device, adapter, desc);
    }
    
    
    void VulkanBuffer::Init(VkDevice device, VkPhysicalDevice adapter, const BufferDesc& desc)
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
        
        //TODO: Buffers should not have usage flags that they do not use
        info.usage |= VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
        if (desc.Flags & BUFFER_FLAGS_VERTEX_BUFFER)
            info.usage |= VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
        if (desc.Flags & BUFFER_FLAGS_INDEX_BUFFER)
            info.usage |= VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
        if (desc.Flags & BUFFER_FLAGS_CONSTANT_BUFFER)
            info.usage |= VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
        
        //Create buffer
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
        

        VkMemoryRequirements memoryRequirements = {};
        vkGetBufferMemoryRequirements(device, m_Buffer, &memoryRequirements);
               
        //Set memoryproperty based on resource usage
        VkMemoryPropertyFlags properties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
        if (desc.Usage == RESOURCE_USAGE_DYNAMIC)
            properties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
        
        
		//Allocate memory on GPU
        uint32 memoryType = FindMemoryType(adapter, memoryRequirements.memoryTypeBits, properties);
        VkMemoryAllocateInfo allocInfo = {};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.pNext = nullptr;
        allocInfo.allocationSize = memoryRequirements.size;
        allocInfo.memoryTypeIndex = memoryType;
         
        if (vkAllocateMemory(device, &allocInfo, nullptr, &m_BufferMemory) != VK_SUCCESS)
        {
            LOG_DEBUG_ERROR("Vulkan: Failed to allocate memory on GPU for Buffer\n");
            return;
        }
        else
        {
            LOG_DEBUG_INFO("Vulkan: Allocated '%u' bytes on the GPU for Buffer\n", allocInfo.allocationSize);
            vkBindBufferMemory(device, m_Buffer, m_BufferMemory, 0);
        }
    }
    
    
    void VulkanBuffer::Map(void** ppMem)
    {
		LAMBDA_ASSERT(ppMem != nullptr);
		LAMBDA_ASSERT(IGraphicsDevice::GetInstance() != nullptr);
        
        VkDevice device = reinterpret_cast<VkDevice>(IGraphicsDevice::GetInstance()->GetNativeHandle());
        vkMapMemory(device, m_BufferMemory, 0, m_Desc.SizeInBytes, 0, ppMem);
    }
    
    
    void VulkanBuffer::Unmap()
    {
		LAMBDA_ASSERT(IGraphicsDevice::GetInstance() != nullptr);
        
        VkDevice device = reinterpret_cast<VkDevice>(IGraphicsDevice::GetInstance()->GetNativeHandle());
        vkUnmapMemory(device, m_BufferMemory);
    }
    
    
    void* VulkanBuffer::GetNativeHandle() const
    {
        return (void*)m_Buffer;
    }
    
    
    uint64 VulkanBuffer::GetSizeInBytes() const
    {
        return m_Desc.SizeInBytes;
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
