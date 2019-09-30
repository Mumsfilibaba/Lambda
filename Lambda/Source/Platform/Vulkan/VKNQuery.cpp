#include "LambdaPch.h"
#include "VKNQuery.h"
#include "VKNDevice.h"

namespace Lambda
{
	VKNQuery::VKNQuery(VKNDevice* pDevice, const QueryDesc& desc)
		: m_pDevice(pDevice),
		m_QueryPool(VK_NULL_HANDLE),
        m_CurrentQuery(0),
        m_TimeStampPeriod(0),
        m_Desc()
    {
		//Add a ref to the refcounter
		this->AddRef();

        Init(desc);
        
		VKNDevice& device = VKNDevice::Get();
        if (m_Desc.Type == QUERY_TYPE_TIMESTAMP)
        {
            m_TimeStampPeriod = device.GetPhysicalDeviceProperties().limits.timestampPeriod;
        }
    }


	VKNQuery::~VKNQuery()
	{
		if (m_QueryPool != VK_NULL_HANDLE)
		{
			vkDestroyQueryPool(m_pDevice->GetDevice(), m_QueryPool, nullptr);
			m_QueryPool = VK_NULL_HANDLE;
		}

		LOG_DEBUG_INFO("Vulkan: Destroyed Query\n");
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
        {
            queryInfo.queryType = VK_QUERY_TYPE_TIMESTAMP;
        }
        
        if (vkCreateQueryPool(m_pDevice->GetDevice(), &queryInfo, nullptr, &m_QueryPool) != VK_SUCCESS)
        {
            LOG_DEBUG_ERROR("Vulkan: Failed to create querypool\n");
        }
        else
        {
            LOG_DEBUG_INFO("Vulkan: Created querypool\n");
            m_Desc = desc;
        }
    }
    
    
    void VKNQuery::GetResults(uint64* pResults, uint32 numResults, uint32 startQuery)
    {
        LAMBDA_ASSERT(pResults != nullptr);
        
        if (vkGetQueryPoolResults(m_pDevice->GetDevice(), m_QueryPool, startQuery, numResults, numResults * sizeof(uint64), pResults, sizeof(uint64), VK_QUERY_RESULT_WITH_AVAILABILITY_BIT) == VK_SUCCESS)
		{
			if (m_Desc.Type == QUERY_TYPE_TIMESTAMP)
			{
				for (uint32 i = 0; i < numResults; i++)
				{
					pResults[i] *= uint64(m_TimeStampPeriod);
				}
			}
		}
    }

    
    void* VKNQuery::GetNativeHandle() const
    {
        return reinterpret_cast<void*>(m_QueryPool);
    }
    
    
    const QueryDesc& VKNQuery::GetDesc() const
    {
        return m_Desc;
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
