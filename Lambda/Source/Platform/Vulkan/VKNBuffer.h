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

    class VKNBuffer final : public DeviceObjectBase<VKNDevice, IBuffer>
    {
    public:
        LAMBDA_NO_COPY(VKNBuffer);
        
        VKNBuffer(VKNDevice* pDevice, const BufferDesc& desc);
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
        VKNAllocation  m_Memory;
        VkBuffer   m_Buffer;
		uint64	   m_SizePerFrame;
		uint64	   m_SizePerUpdate;
        uint64	   m_TotalSize;
        uint64	   m_FrameOffset;
		uint64	   m_DynamicOffset;
        BufferDesc m_Desc;
		bool	   m_IsDirty;
    };

    //---------------
    //VKNUploadBuffer
    //---------------

	struct UploadAllocation
	{
		VkBuffer	Buffer			= VK_NULL_HANDLE;
		uint64		DeviceOffset	= 0;
		uint64		SizeInBytes		= 0;
		void*		pHostMemory		= nullptr;
	};


    class VKNUploadBuffer final
    {
    public:
        LAMBDA_NO_COPY(VKNUploadBuffer);
        
        VKNUploadBuffer(VKNDevice* pDevice, uint64 sizeInBytes);
        ~VKNUploadBuffer();

		UploadAllocation Allocate(uint64 bytesToAllocate, uint64 alignment);
        void Reset();
        
		inline VkBuffer GetVkBuffer() const { return m_Buffer; }
    private:
        bool Init(uint64 sizeInBytes);
        void Reallocate(uint64 sizeInBytes);
    private:
        VKNDevice* m_pDevice;
        VkBuffer  m_Buffer;
        VKNAllocation m_Memory;
        uint64    m_Offset;
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
		void RegisterBuffer(VKNBuffer* pBuffer);
		void UnregisterBuffer(VKNBuffer* pBuffer);
	private:
        uint64  m_FrameIndex;
        std::vector<VKNBuffer*>	m_Buffers;
	public:
		static VKNBufferManager& GetInstance();
	private:
		static VKNBufferManager* s_pInstance;
	};
}
