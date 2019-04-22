#pragma once
#include "Defines.h"
#include "Types.h"
#if defined(LAMBDA_PLAT_WINDOWS)
	#include "DX12CommandList.h"
namespace Lambda
{
	class DX12CommandQueue final
	{
	public:
		LAMBDA_NO_COPY(DX12CommandQueue);

		DX12CommandQueue();
		DX12CommandQueue(ID3D12Device* pDevice, D3D12_COMMAND_LIST_TYPE type, uint64 initalFence);
		~DX12CommandQueue();

		bool Init(ID3D12Device* pDevice, D3D12_COMMAND_LIST_TYPE type, uint64 initalFence);
		void Signal(uint64 fenceValue) const;
		void WaitForFenceValue(uint64 fenceValue) const;
		void ExecuteCommandLists(DX12CommandList* const* ppLists, uint32 numLists) const;
		
		ID3D12CommandQueue* GetQueue() const;
		uint64 GetCompletedFenceValue() const;

	private:
		Microsoft::WRL::ComPtr<ID3D12CommandQueue> m_Queue;
		Microsoft::WRL::ComPtr<ID3D12Fence> m_Fence;
		mutable std::vector<ID3D12CommandList*> m_PendingLists;
		HANDLE m_GPUWaitEvent;
		D3D12_COMMAND_LIST_TYPE m_Type;
	};


	inline void DX12CommandQueue::ExecuteCommandLists(DX12CommandList* const* ppLists, uint32 numLists) const
	{
		//Convert lists
		for (uint32 i = 0; i < numLists; i++)
		{
			ID3D12CommandList* pList = ppLists[i]->GetList();
			m_PendingLists.push_back(pList);
		}

		//Execute
		ID3D12CommandList** ppDXLists = m_PendingLists.data();
		m_Queue->ExecuteCommandLists(numLists, ppDXLists);
		m_PendingLists.clear();
	}
	
	
	inline ID3D12CommandQueue* DX12CommandQueue::GetQueue() const
	{
		return m_Queue.Get();
	}


	inline uint64 DX12CommandQueue::GetCompletedFenceValue() const
	{
		return m_Fence->GetCompletedValue();
	}


	inline void DX12CommandQueue::Signal(uint64 fenceValue) const
	{
		m_Queue->Signal(m_Fence.Get(), fenceValue);
	}


	inline void DX12CommandQueue::WaitForFenceValue(uint64 fenceValue) const
	{
		m_Fence->SetEventOnCompletion(fenceValue, m_GPUWaitEvent);
		WaitForSingleObjectEx(m_GPUWaitEvent, INFINITE, FALSE);
	}
}
#endif