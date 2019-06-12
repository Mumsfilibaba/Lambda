#include "LambdaPch.h"
#if defined(LAMBDA_PLAT_WINDOWS)
	#include "DX12DescriptorCache.h"

namespace Lambda
{
	DX12DescriptorCache::DX12DescriptorCache()
		: m_DstRanges(),
		m_SrcRanges(),
		m_RangeCounts()
	{
	}
}
#endif
