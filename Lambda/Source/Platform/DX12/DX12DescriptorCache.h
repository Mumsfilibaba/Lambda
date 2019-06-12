#pragma once
#include "Defines.h"
#include "Types.h"
#include <vector>
#if defined(LAMBDA_PLAT_WINDOWS)
	#include "DX12DescriptorHandle.h"

namespace Lambda
{
	class DX12DescriptorCache final
	{
	public:
		LAMBDA_NO_COPY(DX12DescriptorCache);

		DX12DescriptorCache();
		~DX12DescriptorCache() = default;

		void CopyDescriptors(ID3D12Device* pDevice, D3D12_DESCRIPTOR_HEAP_TYPE type);
		void PushDescriptor(D3D12_CPU_DESCRIPTOR_HANDLE hDest, D3D12_CPU_DESCRIPTOR_HANDLE hSrc);
		void Clear();

	private:
		std::vector<UINT> m_RangeCounts;
		std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> m_SrcRanges;
		std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> m_DstRanges;
	};


	inline void DX12DescriptorCache::CopyDescriptors(ID3D12Device* pDevice, D3D12_DESCRIPTOR_HEAP_TYPE type)
	{
		UINT numRanges = (UINT)m_RangeCounts.size();
		UINT* pRangeSizes = m_RangeCounts.data();
		D3D12_CPU_DESCRIPTOR_HANDLE* pSrcRangeStart = m_SrcRanges.data();
		D3D12_CPU_DESCRIPTOR_HANDLE* pDstRangeStart = m_DstRanges.data();
		pDevice->CopyDescriptors(numRanges, pDstRangeStart, pRangeSizes, numRanges, pSrcRangeStart, pRangeSizes, type);
	}


	inline void DX12DescriptorCache::PushDescriptor(D3D12_CPU_DESCRIPTOR_HANDLE hDest, D3D12_CPU_DESCRIPTOR_HANDLE hSrc)
	{
		m_RangeCounts.push_back(1);
		m_SrcRanges.push_back(hSrc);
		m_DstRanges.push_back(hDest);
	}


	inline void DX12DescriptorCache::Clear()
	{
		m_RangeCounts.clear();
		m_SrcRanges.clear();
		m_DstRanges.clear();
	}
}
#endif
