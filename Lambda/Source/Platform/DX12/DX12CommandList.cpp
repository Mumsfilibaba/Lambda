#include <LambdaPch.h>
#include "DX12GraphicsDevice.h"
#include "DX12CommandList.h"
#include "DX12RenderTarget.h"
#include "DX12PipelineState.h"
#include "DX12Buffer.h"
#include "DX12Texture2D.h"

#if defined(LAMBDA_PLAT_WINDOWS)
namespace Lambda
{
	ICommandList* ICommandList::Create(CommandListType type)
	{
		IGraphicsDevice* pContext = IGraphicsDevice::GetInstance();
		assert(pContext != nullptr);

		ID3D12Device5* pDevice = reinterpret_cast<ID3D12Device5*>(pContext->GetNativeHandle());
		return DBG_NEW DX12CommandList(pDevice, type);
	}

	inline D3D12_RESOURCE_STATES ParseResourceState(ResourceState state)
	{
		switch (state)
		{
			break;
		case RESOURCE_STATE_RENDERTARGET: return D3D12_RESOURCE_STATE_RENDER_TARGET;
		case RESOURCE_STATE_PRESENT_COMMON: return D3D12_RESOURCE_STATE_COMMON;
		case RESOURCE_STATE_COPY_DEST: return D3D12_RESOURCE_STATE_COPY_DEST;
		case RESOURCE_STATE_COPY_SRC: return D3D12_RESOURCE_STATE_COPY_SOURCE;
		case RESOURCE_STATE_UNKNOWN:
		default: return D3D12_RESOURCE_STATE_COMMON;
		}
	}


	DX12CommandList::DX12CommandList(ID3D12Device5* pDevice, CommandListType type)
		: m_References(0)
	{
		assert(pDevice != nullptr);

		AddRef();
		Init(pDevice, type);
	}


	DX12CommandList::~DX12CommandList()
	{
	}


	void DX12CommandList::ClearRenderTarget(IRenderTarget* pRenderTarget, float color[4])
	{
		DX12RenderTarget* pTarget = reinterpret_cast<DX12RenderTarget*>(pRenderTarget);
		m_List->ClearRenderTargetView(pTarget->GetDescriptorHandle(), color, 0, nullptr);
	}


	void DX12CommandList::ClearDepthStencil(ITexture2D * pDepthStencil, float depth, uint8 stencil)
	{
		DX12Texture2D* pDSV = reinterpret_cast<DX12Texture2D*>(pDepthStencil);
		m_List->ClearDepthStencilView(pDSV->GetDescriptorHandle().CPU, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, depth, stencil, 0, nullptr);
	}


	void DX12CommandList::SetRenderTarget(IRenderTarget* pRenderTarget, ITexture2D* pDepthStencil)
	{
		DX12RenderTarget* pTarget = reinterpret_cast<DX12RenderTarget*>(pRenderTarget);
		D3D12_CPU_DESCRIPTOR_HANDLE rtv = pTarget->GetDescriptorHandle();

		DX12Texture2D* pDsv = reinterpret_cast<DX12Texture2D*>(pDepthStencil);
		if (pDsv != nullptr)
		{
			m_List->OMSetRenderTargets(1, &rtv, false, nullptr);
		}
		else
		{
			D3D12_CPU_DESCRIPTOR_HANDLE dsv = pDsv->GetDescriptorHandle().CPU;
			m_List->OMSetRenderTargets(1, &rtv, false, &dsv);
		}
	}


	void DX12CommandList::SetViewport(const Viewport& viewport)
	{
		D3D12_VIEWPORT port = { viewport.TopX, viewport.TopY, viewport.Width, viewport.Height, viewport.MinDepth, viewport.MaxDepth };
		m_List->RSSetViewports(1, &port);
	}


	void DX12CommandList::SetScissorRect(const Math::Rectangle& scissorRect)
	{
		D3D12_RECT rect = { (LONG)scissorRect.TopLeft.x, (LONG)scissorRect.TopLeft.y, (LONG)scissorRect.BottomRight.x, (LONG)scissorRect.BottomRight.y };
		m_List->RSSetScissorRects(1, &rect);
	}


	void DX12CommandList::SetPrimtiveTopology(PrimitiveTopology topology)
	{
		D3D_PRIMITIVE_TOPOLOGY top = D3D_PRIMITIVE_TOPOLOGY_UNDEFINED;
		if (topology == PRIMITIVE_TOPOLOGY_TRIANGLELIST)
			top = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		else if (topology == PRIMITIVE_TOPOLOGY_TRIANGLESTRIP)
			top = D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;

		m_List->IASetPrimitiveTopology(top);
	}


	void DX12CommandList::SetGraphicsPipelineState(IGraphicsPipelineState* pPSO)
	{
		DX12GraphicsPipelineState* pDXState = reinterpret_cast<DX12GraphicsPipelineState*>(pPSO);
		m_List->SetPipelineState(pDXState->GetPipelineState());
		m_List->SetGraphicsRootSignature(pDXState->GetRootSignature());
	}


	void DX12CommandList::SetVertexBuffer(IBuffer* pBuffer, uint32 slot)
	{
		D3D12_VERTEX_BUFFER_VIEW view = reinterpret_cast<DX12Buffer*>(pBuffer)->GetVertexBufferView();
		m_List->IASetVertexBuffers(slot, 1, &view);
	}


	void DX12CommandList::TransitionResource(IBuffer* pResource, ResourceState resourceState)
	{
		D3D12_RESOURCE_STATES after = ParseResourceState(resourceState);
		DX12Buffer* pBuffer = reinterpret_cast<DX12Buffer*>(pResource);
		InternalTransitionResource(pBuffer->GetResource(), pBuffer->GetResourceState(), after);
		pBuffer->SetResourceState(after);
	}


	void DX12CommandList::TransitionResource(IRenderTarget* pRenderTarget, ResourceState resourceState)
	{
		D3D12_RESOURCE_STATES after = ParseResourceState(resourceState);
		DX12RenderTarget* pTarget = reinterpret_cast<DX12RenderTarget*>(pRenderTarget);
		InternalTransitionResource(pTarget->GetResource(), pTarget->GetResourceState(), after);
		
		pTarget->SetResourceState(after);
	}


	void DX12CommandList::CopyBuffer(IBuffer* pDst, IBuffer* pSrc)
	{
		m_List->CopyResource(reinterpret_cast<DX12Buffer*>(pDst)->GetResource(), reinterpret_cast<DX12Buffer*>(pSrc)->GetResource());
	}


	void DX12CommandList::DrawInstanced(uint32 vertexCountPerInstance, uint32 instanceCount, uint32 startVertexLocation, uint32 startInstanceLocation)
	{
		m_List->DrawInstanced(vertexCountPerInstance, instanceCount, startVertexLocation, startInstanceLocation);
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
		IOBJECT_IMPLEMENT_RELEASE(m_References);
	}


	uint32 DX12CommandList::AddRef()
	{
		return ++m_References;
	}


	void DX12CommandList::Init(ID3D12Device5* pDevice, CommandListType type)
	{
		//Create commandlist
		D3D12_COMMAND_LIST_TYPE cType = D3D12_COMMAND_LIST_TYPE_DIRECT;
		if (type == COMMAND_LIST_TYPE_GRAPHICS)
			cType = D3D12_COMMAND_LIST_TYPE_DIRECT;
		else if (type == COMMAND_LIST_TYPE_COMPUTE)
			cType = D3D12_COMMAND_LIST_TYPE_COMPUTE;

		HRESULT hr = pDevice->CreateCommandList1(1, cType, D3D12_COMMAND_LIST_FLAG_NONE, IID_PPV_ARGS(&m_List));
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
			LOG_DEBUG_INFO("DX12: Created CommandList\n");
		}
	}


	void DX12CommandList::InternalTransitionResource(ID3D12Resource* pResource, D3D12_RESOURCE_STATES before, D3D12_RESOURCE_STATES after)
	{
		D3D12_RESOURCE_BARRIER barrier = {};
		barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		barrier.Transition.pResource = pResource;
		barrier.Transition.Subresource = 0;
		barrier.Transition.StateBefore = before;
		barrier.Transition.StateAfter = after;

		m_List->ResourceBarrier(1, &barrier);
	}
}
#endif