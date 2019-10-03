#pragma once
#include "Graphics/Core/IQuery.h"
#include "Graphics/Core/DeviceObjectBase.h"
#include "VKNAllocator.h"

namespace Lambda
{
    //--------
    //VKNQuery
    //--------
    
    class VKNQuery final : public DeviceObjectBase<VKNDevice, IQuery>
    {
    public:
        LAMBDA_NO_COPY(VKNQuery);
        
        VKNQuery(VKNDevice* pDevice, const QueryDesc& desc);
        ~VKNQuery();
        
        virtual void GetResults(uint64* pResults, uint32 numResults, uint32 startQuery) override final;
        virtual void* GetNativeHandle() const override final;
        virtual const QueryDesc& GetDesc() const override final;
        
        uint32 GetQueryIndex() const;
        void NextQuery();
        void Reset();
    private:
        void Init(const QueryDesc& desc);
    private:
        VkQueryPool m_QueryPool;
        uint32      m_CurrentQuery;
        float       m_TimeStampPeriod;
        QueryDesc   m_Desc;
    };
}
