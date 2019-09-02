#pragma once
#include "Defines.h"
#include "Types.h"
#include <vulkan/vulkan.h>
#include <vector>

namespace Lambda
{
    class VulkanUploadBuffer final
    {
    public:
        LAMBDA_NO_COPY(VulkanUploadBuffer);
        
        VulkanUploadBuffer();
        VulkanUploadBuffer(VkDevice device, VkPhysicalDevice adapter, uint64 sizeInBytes);
        ~VulkanUploadBuffer() = default;
        
        bool Init(VkDevice device, VkPhysicalDevice adapter, uint64 sizeInBytes);
        void* Allocate(uint64 bytesToAllocate);
        void Reset();
        void Destroy(VkDevice device);
        
        uint64 GetOffset() const;
        VkBuffer GetBuffer() const;
        
    private:
        uint8* m_pStart;
        uint8* m_pCurrent;
        VkBuffer m_Buffer;
        VkDeviceMemory m_Memory;
		uint64 m_SizeInBytes;
    };
    
    
    inline VkBuffer VulkanUploadBuffer::GetBuffer() const
    {
        return m_Buffer;
    }
    
    
    inline uint64 VulkanUploadBuffer::GetOffset() const
    {
        return uint64(m_pCurrent - m_pStart);
    }
}
