#include <LambdaPch.h>
#include "DX12GraphicsDevice.h"
#include "DX12CommandList.h"
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

	inline D3D12_RESOURCE_STATES ConvertResourceState(ResourceState state)
	{
		switch (state)
		{
		case RESOURCE_STATE_RENDERTARGET: return D3D12_RESOURCE_STATE_RENDER_TARGET;
		case RESOURCE_STATE_PRESENT_COMMON: return D3D12_RESOURCE_STATE_COMMON;
		case RESOURCE_STATE_COPY_DEST: return D3D12_RESOURCE_STATE_COPY_DEST;
		case RESOURCE_STATE_COPY_SRC: return D3D12_RESOURCE_STATE_COPY_SOURCE;
		case RESOURCE_STATE_DEPTH_WRITE: return D3D12_RESOURCE_STATE_DEPTH_WRITE;
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
		SafeDelete(m_pBufferAllocator);
		SafeDelete(m_pResourceAllocator);
	}


	void DX12CommandList::ClearRenderTarget(ITexture2D* pRenderTarget, float color[4])
	{
		DX12Texture2D* pTarget = reinterpret_cast<DX12Texture2D*>(pRenderTarget);
		m_List->ClearRenderTargetView(pTarget->GetDescriptorHandle().CPU, color, 0, nullptr);
	}


	void DX12CommandList::ClearDepthStencil(ITexture2D * pDepthStencil, float depth, uint8 stencil)
	{
		DX12Texture2D* pDSV = reinterpret_cast<DX12Texture2D*>(pDepthStencil);
		m_List->ClearDepthStencilView(pDSV->GetDescriptorHandle().CPU, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, depth, stencil, 0, nullptr);
	}


	void DX12CommandList::SetRenderTarget(ITexture2D* pRenderTarget, ITexture2D* pDepthStencil)
	{
		DX12Texture2D* pTarget = reinterpret_cast<DX12Texture2D*>(pRenderTarget);
		D3D12_CPU_DESCRIPTOR_HANDLE rtv = pTarget->GetDescriptorHandle().CPU;

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
		DX12GraphicsPipelineState* pState = reinterpret_cast<DX12GraphicsPipelineState*>(pPSO);
		m_List->SetPipelineState(pState->GetPipelineState());
		m_List->SetGraphicsRootSignature(pState->GetRootSignature());
	}


	void DX12CommandList::SetVertexBuffer(IBuffer* pBuffer, uint32 slot)
	{
		D3D12_VERTEX_BUFFER_VIEW view = reinterpret_cast<DX12Buffer*>(pBuffer)->GetVertexBufferView();
		m_List->IASetVertexBuffers(slot, 1, &view);
	}


	void DX12CommandList::TransitionResource(IBuffer* pResource, ResourceState resourceState)
	{
		D3D12_RESOURCE_STATES after = ConvertResourceState(resourceState);
		DX12Buffer* pBuffer = reinterpret_cast<DX12Buffer*>(pResource);
		InternalTransitionResource(pBuffer->GetResource(), D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES, pBuffer->GetResourceState(), after);
		pBuffer->SetResourceState(after);
	}


	void DX12CommandList::TransitionResource(ITexture2D* pResource, ResourceState resourceState)
	{
		D3D12_RESOURCE_STATES after = ConvertResourceState(resourceState);
		DX12Texture2D* pTexture = reinterpret_cast<DX12Texture2D*>(pResource);
		InternalTransitionResource(pTexture->GetResource(), D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES, pTexture->GetResourceState(), after);

		pTexture->SetResourceState(after);
	}


	void DX12CommandList::VSSetConstantBuffers(const IBuffer * const * ppBuffers, uint32 numBuffers, uint32 startSlot)
	{
		//Cache size for resources
		uint32 size = m_pResourceAllocator->GetDescriptorSize();
		for (uint32 i = 0; i < numBuffers; i++)
		{
			//Get dest descriptor for vertex shaders
			D3D12_CPU_DESCRIPTOR_HANDLE hDest = m_hVSDescriptorStart.CPU;
			hDest.ptr += size * (startSlot + i);
			m_DstDescriptorRanges.push_back(hDest);

			//Set src descriptor and count in range
			m_SrcDescriptorRanges.push_back(reinterpret_cast<const DX12Buffer*>(ppBuffers[i])->GetDescriptorHandle().CPU);
			m_DescriptorRangeCounts.push_back(1);
		}
	}


	void DX12CommandList::HSSetConstantBuffers(const IBuffer * const * ppBuffers, uint32 numBuffers, uint32 startSlot)
	{
		//Cache size for resources
		uint32 size = m_pResourceAllocator->GetDescriptorSize();
		for (uint32 i = 0; i < numBuffers; i++)
		{
			//Get dest descriptor for hull shaders
			D3D12_CPU_DESCRIPTOR_HANDLE hDest = m_hHSDescriptorStart.CPU;
			hDest.ptr += size * (startSlot + i);
			m_DstDescriptorRanges.push_back(hDest);

			//Set src descriptor and count in range
			m_SrcDescriptorRanges.push_back(reinterpret_cast<const DX12Buffer*>(ppBuffers[i])->GetDescriptorHandle().CPU);
			m_DescriptorRangeCounts.push_back(1);
		}
	}


	void DX12CommandList::DSSetConstantBuffers(const IBuffer * const * ppBuffers, uint32 numBuffers, uint32 startSlot)
	{
		//Cache size for resources
		uint32 size = m_pResourceAllocator->GetDescriptorSize();
		for (uint32 i = 0; i < numBuffers; i++)
		{
			//Get dest descriptor for domain shaders
			D3D12_CPU_DESCRIPTOR_HANDLE hDest = m_hDSDescriptorStart.CPU;
			hDest.ptr += size * (startSlot + i);
			m_DstDescriptorRanges.push_back(hDest);

			//Set src descriptor and count in range
			m_SrcDescriptorRanges.push_back(reinterpret_cast<const DX12Buffer*>(ppBuffers[i])->GetDescriptorHandle().CPU);
			m_DescriptorRangeCounts.push_back(1);
		}
	}


	void DX12CommandList::GSSetConstantBuffers(const IBuffer * const * ppBuffers, uint32 numBuffers, uint32 startSlot)
	{
		//Cache size for resources
		uint32 size = m_pResourceAllocator->GetDescriptorSize();
		for (uint32 i = 0; i < numBuffers; i++)
		{
			//Get dest descriptor for geometry shaders
			D3D12_CPU_DESCRIPTOR_HANDLE hDest = m_hGSDescriptorStart.CPU;
			hDest.ptr += size * (startSlot + i);
			m_DstDescriptorRanges.push_back(hDest);

			//Set src descriptor and count in range
			m_SrcDescriptorRanges.push_back(reinterpret_cast<const DX12Buffer*>(ppBuffers[i])->GetDescriptorHandle().CPU);
			m_DescriptorRangeCounts.push_back(1);
		}
	}


	void DX12CommandList::PSSetConstantBuffers(const IBuffer * const * ppBuffers, uint32 numBuffers, uint32 startSlot)
	{
		//Cache size for resources
		uint32 size = m_pResourceAllocator->GetDescriptorSize();
		for (uint32 i = 0; i < numBuffers; i++)
		{
			//Get dest descriptor for pixel shaders
			D3D12_CPU_DESCRIPTOR_HANDLE hDest = m_hPSDescriptorStart.CPU;
			hDest.ptr += size * (startSlot + i);
			m_DstDescriptorRanges.push_back(hDest);

			//Set src descriptor and count in range
			m_SrcDescriptorRanges.push_back(reinterpret_cast<const DX12Buffer*>(ppBuffers[i])->GetDescriptorHandle().CPU);
			m_DescriptorRangeCounts.push_back(1);
		}
	}


	void DX12CommandList::UpdateBuffer(IBuffer* pResource, const ResourceData* pData)
	{
		DX12Allocation allocation = m_pBufferAllocator->Allocate(pData->SizeInBytes, D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT);
		memcpy(allocation.pCPU, pData->pData, pData->SizeInBytes);
		m_List->CopyBufferRegion(reinterpret_cast<DX12Buffer*>(pResource)->GetResource(), 0, allocation.pPageResource, allocation.Offset, pData->SizeInBytes);
	}


	void DX12CommandList::CopyBuffer(IBuffer* pDst, IBuffer* pSrc)
	{
		m_List->CopyResource(reinterpret_cast<DX12Buffer*>(pDst)->GetResource(), reinterpret_cast<DX12Buffer*>(pSrc)->GetResource());
	}


	void DX12CommandList::DrawInstanced(uint32 vertexCountPerInstance, uint32 instanceCount, uint32 startVertexLocation, uint32 startInstanceLocation)
	{
		InternalCopyAndSetDescriptors();
		m_List->DrawInstanced(vertexCountPerInstance, instanceCount, startVertexLocation, startInstanceLocation);
	}


	void DX12CommandList::Close()
	{
		m_List->Close();
	}


	void DX12CommandList::Reset()
	{
		//Reset commandallocator and commandlist
		m_Allocator->Reset();
		m_List->Reset(m_Allocator.Get(), nullptr);
		
		//Reset allocators
		m_pBufferAllocator->Reset();
		m_pResourceAllocator->Reset();

		//Allocate new descriptors after reset
		m_hVSDescriptorStart = m_pResourceAllocator->Allocate(8);
		m_hHSDescriptorStart = m_pResourceAllocator->Allocate(8);
		m_hDSDescriptorStart = m_pResourceAllocator->Allocate(8);
		m_hGSDescriptorStart = m_pResourceAllocator->Allocate(8);
		m_hPSDescriptorStart = m_pResourceAllocator->Allocate(8);
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
		using namespace Microsoft::WRL;

		//Create commandallocator
		D3D12_COMMAND_LIST_TYPE cType = D3D12_COMMAND_LIST_TYPE_DIRECT;
		if (type == COMMAND_LIST_TYPE_GRAPHICS)
			cType = D3D12_COMMAND_LIST_TYPE_DIRECT;
		else if (type == COMMAND_LIST_TYPE_COMPUTE)
			cType = D3D12_COMMAND_LIST_TYPE_COMPUTE;

		HRESULT hr = pDevice->CreateCommandAllocator(cType, IID_PPV_ARGS(&m_Allocator));
		if (FAILED(hr))
		{
			LOG_DEBUG_ERROR("DX12: Failed to create CommandAllocator.\n");
			return;
		}
		else
		{
			LOG_DEBUG_INFO("DX12: Created CommandList.\n");
		}

		//Create commandlist
		ComPtr<ID3D12GraphicsCommandList> commandList = nullptr;
		hr = pDevice->CreateCommandList(1, cType, m_Allocator.Get(), nullptr, IID_PPV_ARGS(&commandList));
		if (FAILED(hr))
		{
			LOG_DEBUG_ERROR("DX12: Failed to create CommandList.\n");
			return;
		}
		else
		{
			hr = commandList.As<ID3D12GraphicsCommandList4>(&m_List);
			if (FAILED(hr))
			{
				LOG_DEBUG_ERROR("DX12: Failed to retrive ID3D12GraphicsCommandList4.\n");
				return;
			}
			else
			{
				//Set device
				m_Device = pDevice;

				//Start list in a closed state
				m_List->Close();

				//Create uploadallocators
				m_pBufferAllocator = DBG_NEW DX12LinearAllocator(pDevice);

				//Create frame descriptor allocators
				m_pResourceAllocator = DBG_NEW DX12LinearDescriptorAllocator(pDevice, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 256, true);

				//Allocate descriptors for different stages
				m_hVSDescriptorStart = m_pResourceAllocator->Allocate(8);
				m_hHSDescriptorStart = m_pResourceAllocator->Allocate(8);
				m_hDSDescriptorStart = m_pResourceAllocator->Allocate(8);
				m_hGSDescriptorStart = m_pResourceAllocator->Allocate(8);
				m_hPSDescriptorStart = m_pResourceAllocator->Allocate(8);
			}
		}
	}


	void DX12CommandList::InternalCopyAndSetDescriptors()
	{
		//Perform copy of descriptors
		UINT numRanges = (UINT)m_DescriptorRangeCounts.size();
		UINT* pRangeSizes = m_DescriptorRangeCounts.data();
		D3D12_CPU_DESCRIPTOR_HANDLE* pDstRangeStart = m_DstDescriptorRanges.data();
		D3D12_CPU_DESCRIPTOR_HANDLE* pSrcRangeStart = m_SrcDescriptorRanges.data();
		m_Device->CopyDescriptors(numRanges, pDstRangeStart, pRangeSizes, numRanges, pSrcRangeStart, pRangeSizes, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

		//Set heap and descriptortables at predefine slots. (See default rootsignature in DX12PipelineState)
		ID3D12DescriptorHeap* pHeap = m_pResourceAllocator->GetHeap();
		m_List->SetDescriptorHeaps(1, &pHeap);
		m_List->SetGraphicsRootDescriptorTable(0, m_hVSDescriptorStart.GPU);
		m_List->SetGraphicsRootDescriptorTable(2, m_hHSDescriptorStart.GPU);
		m_List->SetGraphicsRootDescriptorTable(4, m_hDSDescriptorStart.GPU);
		m_List->SetGraphicsRootDescriptorTable(6, m_hGSDescriptorStart.GPU);
		m_List->SetGraphicsRootDescriptorTable(8, m_hPSDescriptorStart.GPU);

		//Clear ranges
		m_DstDescriptorRanges.clear();
		m_SrcDescriptorRanges.clear();
		m_DescriptorRangeCounts.clear();

		m_hVSDescriptorStart = m_pResourceAllocator->Allocate(8);
		m_hHSDescriptorStart = m_pResourceAllocator->Allocate(8);
		m_hDSDescriptorStart = m_pResourceAllocator->Allocate(8);
		m_hGSDescriptorStart = m_pResourceAllocator->Allocate(8);
		m_hPSDescriptorStart = m_pResourceAllocator->Allocate(8);
	}


	void DX12CommandList::InternalTransitionResource(ID3D12Resource* pResource, uint32 subresource, D3D12_RESOURCE_STATES before, D3D12_RESOURCE_STATES after)
	{
		if (before == after)
			return;

		D3D12_RESOURCE_BARRIER barrier = {};
		barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		barrier.Transition.pResource = pResource;
		barrier.Transition.Subresource = subresource;
		barrier.Transition.StateBefore = before;
		barrier.Transition.StateAfter = after;

		m_List->ResourceBarrier(1, &barrier);
	}
}
#endif