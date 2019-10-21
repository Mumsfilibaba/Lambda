#pragma once
#include "Graphics/Core/BufferBase.h"
#include <string>
#include "VKNAllocator.h"

namespace Lambda
{
	class VKNDevice;

	//---------
	//VKNBuffer
	//---------

    class VKNBuffer final : public BufferBase<VKNDevice>
    {
    public:
        LAMBDA_NO_COPY(VKNBuffer);
        
        VKNBuffer(VKNDevice* pDevice, const BufferDesc& desc);
        ~VKNBuffer();
        
        virtual void* GetNativeHandle() const override final;

		void Map(void** ppMem);
        void Unmap();

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
        VkBuffer m_Buffer;
        VKNAllocation m_Memory;
		VKNDynamicAllocation m_DynamicState;
		uint64 m_SizePerFrame;
		uint64 m_SizePerUpdate;
        uint64 m_TotalSize;
        uint64 m_FrameOffset;
		uint64 m_DynamicOffset;
		bool m_IsDirty;
    };

	//-------------------
	//VKNUploadAllocation
	//-------------------

	struct VKNUploadAllocation
	{
		VkBuffer	Buffer			= VK_NULL_HANDLE;
		uint64		DeviceOffset	= 0;
		uint64		SizeInBytes		= 0;
		void*		pHostMemory		= nullptr;
	};

	//---------------
	//VKNUploadBuffer
	//---------------

    class VKNUploadBuffer final
    {
    public:
        LAMBDA_NO_COPY(VKNUploadBuffer);
        
        VKNUploadBuffer(VKNDevice* pDevice, uint64 sizeInBytes);
        ~VKNUploadBuffer();

		VKNUploadAllocation Allocate(uint64 bytesToAllocate, uint64 alignment);
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
