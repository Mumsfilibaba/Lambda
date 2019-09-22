#pragma once
#include "Graphics/IQuery.h"
#include "VKNAllocator.h"

namespace Lambda
{
    //--------
    //VKNQuery
    //--------
    
    class VKNQuery final : public IQuery
    {
    public:
        LAMBDA_NO_COPY(VKNQuery);
        
        VKNQuery(const QueryDesc& desc);
        ~VKNQuery() = default;
        
        virtual void        GetResults(uint64* pResults, uint32 numResults, uint32 startQuery) override final;
        virtual void*       GetNativeHandle() const override final;
        virtual QueryDesc   GetDesc() const override final;
        
        uint32  GetQueryIndex() const;
        void    NextQuery();
        void    Reset();
        void    Destroy(VkDevice device);
        
    private:
        void Init(const QueryDesc& desc);
        
    private:
        uint32      m_CurrentQuery;
        float		m_TimeStampPeriod;
        VkQueryPool m_QueryPool;
        QueryDesc   m_Desc;
    };
}
