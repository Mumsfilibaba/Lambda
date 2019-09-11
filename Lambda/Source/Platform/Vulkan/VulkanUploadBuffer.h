#pragma once
#include "Defines.h"
#include "Types.h"
#include "VulkanAllocator.h"

namespace Lambda
{
    class VulkanGraphicsDevice;
    
    
    class VulkanUploadBuffer final
    {
    public:
        LAMBDA_NO_COPY(VulkanUploadBuffer);
        
        VulkanUploadBuffer(const VulkanGraphicsDevice* pVkDevice, IVulkanAllocator* pAllocator, uint64 sizeInBytes);
        ~VulkanUploadBuffer() = default;
        
		void Map();
		void Unmap();

        void* Allocate(uint64 bytesToAllocate);
        void Reset();
        void Destroy(VkDevice device);
        
        uint64 GetOffset() const;
        VkBuffer GetBuffer() const;
        
	private:
        bool Init(const VulkanGraphicsDevice* pVkDevice, uint64 sizeInBytes);
    
	private:
		IVulkanAllocator* const m_pAllocator;
		VulkanAllocation m_Memory;
        uint8* m_pStart;
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
        return uint64(m_pCurrent - m_pStart);
    }
}
