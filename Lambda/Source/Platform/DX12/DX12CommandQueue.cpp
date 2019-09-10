#include "LambdaPch.h"
#if defined(LAMBDA_PLAT_WINDOWS)
#include "DX12CommandQueue.h"

namespace Lambda
{
	DX12CommandQueue::DX12CommandQueue()
		: m_Fence(nullptr),
		m_Queue(nullptr),
		m_PendingLists(),
		m_Type(),
		m_GPUWaitEvent(0)
	{
	}
	
	
	DX12CommandQueue::DX12CommandQueue(ID3D12Device* pDevice, D3D12_COMMAND_LIST_TYPE type, uint64 initalFence)
		: m_Fence(nullptr),
		m_Queue(nullptr),
		m_PendingLists(),
		m_Type(),
		m_GPUWaitEvent(0)
	{
		Init(pDevice, type, initalFence);
	}
	
	
	DX12CommandQueue::~DX12CommandQueue()
	{
		CloseHandle(m_GPUWaitEvent);
	}


	bool DX12CommandQueue::Init(ID3D12Device* pDevice, D3D12_COMMAND_LIST_TYPE type, uint64 initalFence)
	{
		LAMBDA_ASSERT(pDevice != nullptr);

		//Create commandqueue
		D3D12_COMMAND_QUEUE_DESC qDesc = {};
		qDesc.Type = type;
		qDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		qDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
		qDesc.NodeMask = 1;
		
		HRESULT hr = pDevice->CreateCommandQueue(&qDesc, IID_PPV_ARGS(&m_Queue));
		if (FAILED(hr))
		{
			LOG_DEBUG_ERROR("DX12: Could not create CommandQueue.\n");
			return false;
		}
		else
		{
			LOG_DEBUG_INFO("DX12: Created CommandQueue.\n");

			m_GPUWaitEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
			if (m_GPUWaitEvent == 0)
			{
				LOG_DEBUG_INFO("DX12: Failed to create GPU-event handle.\n");
				return false;
			}
		}

		//Create fence
		hr = pDevice->CreateFence(initalFence, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_Fence));
		if (FAILED(hr))
		{
			LOG_DEBUG_ERROR("DX12: Could not create Fence.\n");
			return false;
		}
		else
		{
			LOG_DEBUG_INFO("DX12: Created Fence.\n");
		}

		return true;
	}
}
#endif