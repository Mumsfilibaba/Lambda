#pragma once
#include "Graphics/Core/IBuffer.h"
#include <string>
#include "VKNAllocator.h"

namespace Lambda
{
	class VKNDevice;

	//---------
	//VKNBuffer
	//---------

    class VKNBuffer final : public VKNDeviceObject<IBuffer>
    {
    public:
        LAMBDA_NO_COPY(VKNBuffer);
        
        VKNBuffer(VKNDevice* pDevice, IVKNAllocator* pAllocator, const BufferDesc& desc);
        ~VKNBuffer();
        
        virtual void Map(void** ppMem) override final;
        virtual void Unmap() override final;
        virtual void* GetNativeHandle() const override final;
        virtual const BufferDesc& GetDesc() const override final;

		void AdvanceFrame();
		void DynamicUpdate(const ResourceData* pData);
		void Reallocate(uint32 sizeInBytes);
		
		inline VkBuffer GetVkBuffer() const		{ return m_Buffer; }
		inline bool   IsDirty() const			{ return m_IsDirty; }
		inline void   SetIsClean()				{ m_IsDirty = false; }
		inline uint64 GetDynamicOffset() const	{ return  m_DynamicOffset + m_FrameOffset; }
    private:
        void Init(const BufferDesc& desc);
    private:
		IVKNAllocator*	m_pAllocator;
        VKNMemory		m_Memory;
        VkBuffer		m_Buffer;
		uint64			m_SizePerFrame;
		uint64			m_SizePerUpdate;
        uint64			m_TotalSize;
        uint64			m_FrameOffset;
		uint64			m_DynamicOffset;
        BufferDesc		m_Desc;
		bool			m_IsDirty;
    };

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
        
		inline uint64 GetDeviceOffset() const { return uint64(m_pCurrent - m_Memory.pHostMemory); }
		inline VkBuffer GetVkBuffer() const { return m_Buffer; }
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
