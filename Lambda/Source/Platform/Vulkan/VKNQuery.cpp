#include "LambdaPch.h"
#include "VKNQuery.h"
#include "VKNDevice.h"

namespace Lambda
{
	VKNQuery::VKNQuery(VKNDevice* pVkDevice, const QueryDesc& desc)
		: QueryBase<VKNDevice>(pVkDevice, desc),
		m_VkQueryPool(VK_NULL_HANDLE),
        m_CurrentQuery(0),
        m_TimeStampPeriod(0)
    {
		//Add a ref to the refcounter
		this->AddRef();
        Init(desc);
        
		//Get period of a timestamp
		if (m_Desc.Type == QUERY_TYPE_TIMESTAMP)
		{
            m_TimeStampPeriod	= pVkDevice->GetVkPhysicalDeviceProperties().limits.timestampPeriod;
			uint32 validBits = pVkDevice->GetVkGraphicsQueueProperties().timestampValidBits;
			
			m_ValidBitsMask = 0;
			for (uint64 i = 0; i < validBits; i++)
				m_ValidBitsMask |= (1LLU << i);
		}
    }


	VKNQuery::~VKNQuery()
	{
		if (m_VkQueryPool != VK_NULL_HANDLE)
			m_pDevice->SafeReleaseVkResource<VkQueryPool>(m_VkQueryPool);

		LOG_DEBUG_INFO("[Vulkan] Destroyed Query\n");
	}
    
    
    void VKNQuery::Init(const QueryDesc& desc)
    {
        //Create
        VkQueryPoolCreateInfo queryInfo = {};
        queryInfo.sType                 = VK_STRUCTURE_TYPE_QUERY_POOL_CREATE_INFO;
        queryInfo.flags                 = 0;
        queryInfo.pNext                 = nullptr;
        queryInfo.queryCount            = desc.QueryCount;
        queryInfo.pipelineStatistics    = 0;
        if (desc.Type == QUERY_TYPE_TIMESTAMP)
            queryInfo.queryType = VK_QUERY_TYPE_TIMESTAMP;
        
        if (vkCreateQueryPool(m_pDevice->GetVkDevice(), &queryInfo, nullptr, &m_VkQueryPool) != VK_SUCCESS)
        {
            LOG_DEBUG_ERROR("[Vulkan] Failed to create querypool\n");
        }
        else
        {
            LOG_DEBUG_INFO("[Vulkan] Created querypool\n");
        }
    }
    
    
    void VKNQuery::GetResults(uint64* pResults, uint32 numResults, uint32 startQuery)
    {
        LAMBDA_ASSERT(pResults != nullptr);
        
        if (vkGetQueryPoolResults(m_pDevice->GetVkDevice(), m_VkQueryPool, startQuery, numResults, numResults * sizeof(uint64), pResults, sizeof(uint64), VK_QUERY_RESULT_64_BIT) == VK_SUCCESS)
		{
			if (m_Desc.Type == QUERY_TYPE_TIMESTAMP)
			{
				for (uint32 i = 0; i < numResults; i++)
					pResults[i] = (pResults[i] & m_ValidBitsMask) * uint64(m_TimeStampPeriod);
			}
		}
    }

    
    void* VKNQuery::GetNativeHandle() const
    {
        return reinterpret_cast<void*>(m_VkQueryPool);
    }
    
    
    uint32 VKNQuery::GetQueryIndex() const
    {
        return m_CurrentQuery;
    }
    
    
    void VKNQuery::NextQuery()
    {
        m_CurrentQuery++;
    }
    
    
    void VKNQuery::Reset()
    {
        m_CurrentQuery = 0;
    }
}
