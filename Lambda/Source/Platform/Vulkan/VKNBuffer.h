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
		friend class VKNDeviceContext;
		using TBuffer = BufferBase<VKNDevice>;

    public:
        LAMBDA_NO_COPY(VKNBuffer);
        
        VKNBuffer(VKNDevice* pDevice, const ResourceData* pInitalData, const BufferDesc& desc);
        ~VKNBuffer();
        
        virtual void* GetNativeHandle() const override final;
		virtual void SetName(const char* pName) override final;

		VkBuffer GetVkBuffer() const;
		inline VkDeviceSize GetDynamicOffset() const	{ return m_DynamicState.BufferOffset; }
		inline VkDeviceSize GetAlignment() const		{ return m_DynamicOffsetAlignment; }
    private:
        void Init(const ResourceData* pInitalData, const BufferDesc& desc);
    private:
        VkBuffer m_VkBuffer;
        VKNAllocation m_Memory;
		VKNDynamicAllocation m_DynamicState;
		VkDeviceSize m_DynamicOffsetAlignment;
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
}
