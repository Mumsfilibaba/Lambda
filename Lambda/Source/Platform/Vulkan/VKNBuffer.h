#pragma once
#include "Graphics/Core/CBufferBase.h"
#include <string>
#include "Memory/VKNDeviceAllocator.h"
#include "Memory/VKNDynamicMemoryAllocator.h"

namespace Lambda
{
	class VKNDevice;

	//---------
	//VKNBuffer
	//---------

    class VKNBuffer final : public CBufferBase<VKNDevice>
    {
		friend class VKNDeviceContext;
		using TBuffer = CBufferBase<VKNDevice>;

    public:
        LAMBDA_NO_COPY(VKNBuffer);
        
        VKNBuffer(VKNDevice* pVkDevice, const SResourceData* pInitalData, const SBufferDesc& desc);
        ~VKNBuffer();
        
        virtual void* GetNativeHandle() const override final;
		virtual void SetName(const char* pName) override final;

        
		_forceinline VkBuffer GetVkBuffer() const
		{
			if (m_Desc.Usage == USAGE_DYNAMIC)
				return m_DynamicMemory.Buffer;

			return m_VkBuffer;
		}


		_forceinline VkDeviceSize GetDynamicOffset() const
		{ 
			return m_DynamicMemory.BufferOffset; 
		}
		

		_forceinline  VkDeviceSize GetAlignment() const
		{ 
			return m_DynamicOffsetAlignment; 
		}
    private:
        void Init(const SResourceData* pInitalData, const SBufferDesc& desc);
    private:
		VKNDynamicAllocation m_DynamicMemory;
        VKNAllocation m_Memory;
        VkBuffer m_VkBuffer;
		VkDeviceSize m_DynamicOffsetAlignment;
    };
}
