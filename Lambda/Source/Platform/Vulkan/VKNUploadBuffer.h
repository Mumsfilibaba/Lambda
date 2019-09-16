#pragma once
#include "Defines.h"
#include "Types.h"
#include "VKNAllocator.h"

namespace Lambda
{
	//---------------
	//VKNUploadBuffer
	//---------------

    class VKNUploadBuffer final
    {
    public:
        LAMBDA_NO_COPY(VKNUploadBuffer);
        
        VKNUploadBuffer(IVKNAllocator* pAllocator, uint64 sizeInBytes);
        ~VKNUploadBuffer() = default;

        void*   Allocate(uint64 bytesToAllocate);
        void    Reset();
        void    Destroy(VkDevice device);
        
        uint64      GetOffset() const;
        VkBuffer    GetBuffer() const;
        
	private:
        bool Init(uint64 sizeInBytes);
    
	private:
		IVKNAllocator* const	m_pAllocator;
		VKNMemory				m_Memory;
        uint8*					m_pCurrent;
        VkBuffer                m_Buffer;
		uint64                  m_SizeInBytes;
    };
    
    
    inline VkBuffer VKNUploadBuffer::GetBuffer() const
    {
        return m_Buffer;
    }
    
    
    inline uint64 VKNUploadBuffer::GetOffset() const
    {
        return uint64(m_pCurrent - m_Memory.pHostMemory);
    }
}
