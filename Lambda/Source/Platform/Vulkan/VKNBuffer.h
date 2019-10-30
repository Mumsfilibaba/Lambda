#pragma once
#include "Graphics/Core/BufferBase.h"
#include <string>
#include "Memory/VKNDeviceAllocator.h"
#include "Memory/VKNDynamicMemoryAllocator.h"

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
        void Init(const ResourceData* pInitalData, const BufferDesc& desc);
    private:
        VkBuffer m_VkBuffer;
        VKNAllocation m_Memory;
		VKNDynamicAllocation m_DynamicMemory;
		VkDeviceSize m_DynamicOffsetAlignment;
    };
}
