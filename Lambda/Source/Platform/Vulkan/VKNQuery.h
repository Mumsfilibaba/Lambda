#pragma once
#include "Graphics/Core/CQueryBase.h"
#include "Vulkan.h"

namespace Lambda
{
	class VKNDevice;

    //--------
    //VKNQuery
    //--------
    
    class VKNQuery final : public CQueryBase<VKNDevice>
    {
    public:
        LAMBDA_NO_COPY(VKNQuery);
        
        VKNQuery(VKNDevice* pVkDevice, const SQueryDesc& desc);
        ~VKNQuery();
        
        virtual void GetResults(uint64* pResults, uint32 numResults, uint32 startQuery) override final;
        virtual void* GetNativeHandle() const override final;
        
        uint32 GetQueryIndex() const;
        void NextQuery();
        void Reset();
    private:
        void Init(const SQueryDesc& desc);
    private:
        VkQueryPool m_VkQueryPool;
		uint64 m_ValidBitsMask;
        uint32 m_CurrentQuery;
        float  m_TimeStampPeriod;
    };
}
