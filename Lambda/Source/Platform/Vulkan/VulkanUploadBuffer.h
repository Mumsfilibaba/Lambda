#pragma once
#include "Defines.h"
#include "Types.h"
#include "VulkanAllocator.h"

namespace Lambda
{
    class VulkanUploadBuffer final
    {
    public:
        LAMBDA_NO_COPY(VulkanUploadBuffer);
        
        VulkanUploadBuffer(VkDevice device, IVulkanAllocator* pAllocator, uint64 sizeInBytes);
        ~VulkanUploadBuffer() = default;

        void* Allocate(uint64 bytesToAllocate);
        void Reset();
        void Destroy(VkDevice device);
        
        uint64 GetOffset() const;
        VkBuffer GetBuffer() const;
        
	private:
        bool Init(VkDevice device, uint64 sizeInBytes);
    
	private:
		IVulkanAllocator* const m_pAllocator;
		VulkanMemory m_Memory;
        uint8* m_pCurrent;
        VkBuffer m_Buffer;
		uint64 m_SizeInBytes;
    };
    
    
    inline VkBuffer VulkanUploadBuffer::GetBuffer() const
    {
        return m_Buffer;
    }
    
    
    inline uint64 VulkanUploadBuffer::GetOffset() const
    {
        return uint64(m_pCurrent - m_Memory.pMemory);
    }
}
