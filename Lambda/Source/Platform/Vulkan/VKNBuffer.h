#pragma once
#include "Graphics/IBuffer.h"
#include <string>
#include "VKNAllocator.h"

namespace Lambda
{
	//---------
	//VKNBuffer
	//---------

    class VKNBuffer final : public IBuffer
    {
    public:
        LAMBDA_NO_COPY(VKNBuffer);
        
        VKNBuffer(IVKNAllocator* pAllocator, const BufferDesc& desc);
        ~VKNBuffer() = default;
        
        virtual void Map(void** ppMem) override final;
        virtual void Unmap() override final;
        virtual void* GetNativeHandle() const override final;
        virtual BufferDesc GetDesc() const override final;

		bool IsDirty() const;
		void SetIsClean();
		void AdvanceFrame();
		void DynamicUpdate(const ResourceData* pData);
		void Reallocate(uint32 sizeInBytes);
		uint64 GetDynamicOffset() const;
        //Release releases vulkan resources while destroy also calls 'delete this'
        void Release(VkDevice device);
        void Destroy(VkDevice device);
    private:
        void Init(const BufferDesc& desc);
    private:
		IVKNAllocator* const m_pAllocator;
        VKNMemory		     m_Memory;
        VkBuffer		     m_Buffer;
		uint64			     m_SizePerFrame;
		uint64			     m_SizePerUpdate;
        uint64               m_TotalSize;
        uint64               m_FrameOffset;
		uint64			     m_DynamicOffset;
        BufferDesc		     m_Desc;
		bool			     m_IsDirty;
    };


	inline bool VKNBuffer::IsDirty() const
	{
		return m_IsDirty;
	}


	inline void VKNBuffer::SetIsClean()
	{
		m_IsDirty = false;
	}


	inline uint64 VKNBuffer::GetDynamicOffset() const
	{
		return m_DynamicOffset + m_FrameOffset;
	}

    //---------------
    //VKNUploadBuffer
    //---------------

    class VKNUploadBuffer final
    {
    public:
        LAMBDA_NO_COPY(VKNUploadBuffer);
        
        VKNUploadBuffer(IVKNAllocator* pAllocator, uint64 sizeInBytes);
        ~VKNUploadBuffer() = default;

        void* Allocate(uint64 bytesToAllocate);
        void Reset();
        void Destroy(VkDevice device);
        
        uint64 GetDeviceOffset() const;
        VkBuffer GetBuffer() const;
    private:
        bool Init(uint64 sizeInBytes);
        void Reallocate(uint64 sizeInBytes);
    private:
        IVKNAllocator* const m_pAllocator;
        uint8*               m_pCurrent;
        VKNMemory            m_Memory;
        VkBuffer             m_Buffer;
        uint64               m_BytesLeft;
        uint64               m_SizeInBytes;
    };
    
    
    inline VkBuffer VKNUploadBuffer::GetBuffer() const
    {
        return m_Buffer;
    }
    
    
    inline uint64 VKNUploadBuffer::GetDeviceOffset() const
    {
        return uint64(m_pCurrent - m_Memory.pHostMemory);
    }

	//----------------
	//VKNBufferManager
	//----------------

	class VKNBufferManager final
	{
	public:
		LAMBDA_NO_COPY(VKNBufferManager);

		VKNBufferManager();
		~VKNBufferManager();

		void AdvanceFrame();
        void DestroyBuffer(VkBuffer buffer);
		void RegisterBuffer(VKNBuffer* pBuffer);
		void UnregisterBuffer(VKNBuffer* pBuffer);
    private:
        void EmptyGarbageBuffers();
	private:
        uint64                              m_FrameIndex;
        std::vector<VKNBuffer*>				m_Buffers;
        std::vector<std::vector<VkBuffer>>  m_BuffersToDelete;
	public:
		static VKNBufferManager& GetInstance();
	private:
		static VKNBufferManager* s_pInstance;
	};
}
