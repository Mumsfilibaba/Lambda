#include "LambdaPch.h"
#include "VulkanBuffer.h"
#include "VulkanHelpers.inl"

namespace Lambda
{
    VulkanBuffer::VulkanBuffer(VkDevice device, VkPhysicalDevice adapter, const ResourceData* pInitalData, const BufferDesc& desc)
        : m_Buffer(VK_NULL_HANDLE),
        m_BufferMemory(VK_NULL_HANDLE),
        m_Desc()
    {
        Init(device, adapter, pInitalData, desc);
    }
    
    
    void VulkanBuffer::Init(VkDevice device, VkPhysicalDevice adapter, const ResourceData* pInitalData, const BufferDesc& desc)
    {
        assert(device != VK_NULL_HANDLE);
        
        //Setup buffer
        VkBufferCreateInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        info.pNext = nullptr;
        info.flags = 0;
        info.size = desc.SizeInBytes;
        info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        
        if (desc.Flags == BUFFER_FLAGS_VERTEX_BUFFER)
            info.usage |= VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
        if (desc.Flags == BUFFER_FLAGS_INDEX_BUFFER)
            info.usage |= VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
        if (desc.Flags == BUFFER_FLAGS_CONSTANT_BUFFER)
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
            
            //Set desc
            m_Desc = desc;
        }
        
        //Get memory reqiurements
        VkMemoryRequirements memoryRequirements = {};
        vkGetBufferMemoryRequirements(device, m_Buffer, &memoryRequirements);
               
        //Find memorytype
        uint32 memoryType = FindMemoryType(adapter, memoryRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
        
        //Setup for allocation on GPU
        VkMemoryAllocateInfo allocInfo = {};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.pNext = nullptr;
        allocInfo.allocationSize = memoryRequirements.size;
        allocInfo.memoryTypeIndex = memoryType;
        
        //Allocate memory on GPU
        if (vkAllocateMemory(device, &allocInfo, nullptr, &m_BufferMemory) != VK_SUCCESS)
        {
            LOG_DEBUG_ERROR("Vulkan: Failed to allocate memory on GPU for Buffer\n");
            return;
        }
        else
        {
            LOG_DEBUG_INFO("Vulkan: Allocated '%u' bytes on the GPU for Buffer\n", allocInfo.allocationSize);
            
            //Bind memory to buffer
            vkBindBufferMemory(device, m_Buffer, m_BufferMemory, 0);
        }
    }
    
    
    void VulkanBuffer::Map(void** ppMem)
    {
        assert(ppMem != nullptr);
    }
    
    
    void VulkanBuffer::Unmap()
    {
        
    }
    
    
    BufferDesc VulkanBuffer::GetDesc() const
    {
        return m_Desc;
    }
    
    
    void VulkanBuffer::Destroy(VkDevice device)
    {
        assert(device != VK_NULL_HANDLE);
        
        //Destroy vulkan buffer
        if (m_Buffer != VK_NULL_HANDLE)
        {
            vkDestroyBuffer(device, m_Buffer, nullptr);
            m_Buffer = VK_NULL_HANDLE;
        }
        
        //Free memory
        if (m_BufferMemory != VK_NULL_HANDLE)
        {
            vkFreeMemory(device, m_BufferMemory, nullptr);
            m_BufferMemory = VK_NULL_HANDLE;
        }
        
        //Destroy me
        delete this;
    }
}
