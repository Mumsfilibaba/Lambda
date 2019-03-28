#include <LambdaPch.h>
#include <System/Log.h>
#include "DX12GraphicsContext.h"
#include "DX12CommandList.h"

#if defined(LAMBDA_PLAT_WINDOWS)
namespace Lambda
{
	ICommandList* ICommandList::Create(CommandListType type)
	{
		IGraphicsContext* pContext = IGraphicsContext::GetInstance();
		assert(pContext != nullptr);

		ID3D12Device5* pDevice = reinterpret_cast<ID3D12Device5*>(pContext->GetNativeHandle());
		return DBG_NEW DX12CommandList(pDevice, type);
	}

	DX12CommandList::DX12CommandList(ID3D12Device5* pDevice, CommandListType type)
		: m_References(0)
	{
		AddRef();
		Init(pDevice, type);
	}

	DX12CommandList::~DX12CommandList()
	{
	}

	void DX12CommandList::Close()
	{
		m_List->Close();
	}

	void DX12CommandList::Reset()
	{
		m_Allocator->Reset();
		m_List->Reset(m_Allocator.Get(), nullptr);
	}

	uint32 DX12CommandList::Release()
	{
		uint32 refs = --m_References;
		if (refs < 1)
			delete this;
		
		return refs;
	}

	uint32 DX12CommandList::AddRef()
	{
		return ++m_References;
	}

	void DX12CommandList::Init(ID3D12Device5* pDevice, CommandListType type)
	{
		//Create commandlist
		D3D12_COMMAND_LIST_TYPE cType;
		if (type == COMMAND_LIST_TYPE_GRAPHICS)
			cType = D3D12_COMMAND_LIST_TYPE_DIRECT;
		else if (type == COMMAND_LIST_TYPE_COMPUTE)
			cType = D3D12_COMMAND_LIST_TYPE_COMPUTE;

		HRESULT hr = pDevice->CreateCommandList1(0, cType, D3D12_COMMAND_LIST_FLAG_NONE, IID_PPV_ARGS(&m_List));
		if (FAILED(hr))
		{
			LOG_DEBUG_ERROR("DX12: Failed to create CommandList\n");
			return;
		}

		hr = pDevice->CreateCommandAllocator(cType, IID_PPV_ARGS(&m_Allocator));
		if (FAILED(hr))
		{
			LOG_DEBUG_ERROR("DX12: Failed to create CommandAllocator\n");
		}
		else
		{
			LOG_DEBUG_INFO("DX12: Created CommandList");
		}
	}
}
#endif