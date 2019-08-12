#include "LambdaPch.h"
#include "DX12GraphicsDevice.h"
#include "DX12CommandList.h"
#include "DX12PipelineState.h"
#include "DX12Buffer.h"
#include "DX12Texture2D.h"
#include "DX12SamplerState.h"
#include "Utilities/TextureHelper.h"
#include "Utilities/StringHelper.h"

#if defined(LAMBDA_PLAT_WINDOWS)
#define SAMPLER_DESCRIPTOR_INDEX 0
#define SRV_DESCRIPTOR_INDEX 1
#define CBV_DESCRIPTOR_INDEX 2
#define UAV_DESCRIPTOR_INDEX 3

namespace Lambda
{
	DX12CommandList::DX12CommandList(ID3D12Device* pDevice, CommandListType type, const DX12DescriptorHandle& nullSampler, const DX12DescriptorHandle& nullSRV, const DX12DescriptorHandle& nullUAV, const DX12DescriptorHandle& nullCBV)
		: m_Allocator(nullptr),
		m_List(nullptr),
		m_Device(nullptr),
		m_BufferAllocator(),
		m_TextureAllocator(),
		m_ResourceAllocator(),
		m_SamplerAllocator(),
		m_ResourceCache(),
		m_SamplerCache(),
		m_ResourceTracker(),
		m_hResourceDescriptorTables(),
		m_hSamplerDescriptorTables(),
		m_SamplerDescriptorSize(0),
		m_ResourceDescriptorSize(0),
		m_Type(COMMAND_LIST_TYPE_UNKNOWN)
	{
		assert(pDevice != nullptr);

		Init(pDevice, type, nullSampler, nullSRV, nullUAV, nullCBV);
	}


	void DX12CommandList::ClearRenderTarget(ITexture2D* pRenderTarget, float color[4])
	{
		//Flush barriers - needs to be transitioned outside function
		m_ResourceTracker.FlushBarriers(m_List.Get());

		//Clear rendertarget
		DX12Texture2D* pTarget = reinterpret_cast<DX12Texture2D*>(pRenderTarget);
		m_List->ClearRenderTargetView(pTarget->GetDescriptorHandle().CPU, color, 0, nullptr);
	}


	void DX12CommandList::ClearDepthStencil(ITexture2D * pDepthStencil, float depth, uint8 stencil)
	{
		//Flush barriers - needs to be transitioned outside function
		m_ResourceTracker.FlushBarriers(m_List.Get());

		//Clear depthstencil
		DX12Texture2D* pDSV = reinterpret_cast<DX12Texture2D*>(pDepthStencil);
		m_List->ClearDepthStencilView(pDSV->GetDescriptorHandle().CPU, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, depth, stencil, 0, nullptr);
	}


	void DX12CommandList::SetRenderTarget(ITexture2D* pRenderTarget, ITexture2D* pDepthStencil)
	{
		//TODO: Multiple rendertargets

		//Flush barriers - needs to be transitioned outside function
		m_ResourceTracker.FlushBarriers(m_List.Get());

		//Set rendertargets
		DX12Texture2D* pTarget = reinterpret_cast<DX12Texture2D*>(pRenderTarget);
		D3D12_CPU_DESCRIPTOR_HANDLE rtv = pTarget->GetDescriptorHandle().CPU;

		DX12Texture2D* pDsv = reinterpret_cast<DX12Texture2D*>(pDepthStencil);
		if (pDsv == nullptr)
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


	void DX12CommandList::SetScissorRect(const Rectangle& scissorRect)
	{
		D3D12_RECT rect = { (LONG)scissorRect.X, (LONG)scissorRect.Y, (LONG)scissorRect.X + (LONG)scissorRect.Width, (LONG)scissorRect.Y + (LONG)scissorRect.Height };
		m_List->RSSetScissorRects(1, &rect);
	}


	/*void DX12CommandList::SetPrimtiveTopology(PrimitiveTopology topology)
	{
		D3D_PRIMITIVE_TOPOLOGY top = D3D_PRIMITIVE_TOPOLOGY_UNDEFINED;
		if (topology == PRIMITIVE_TOPOLOGY_TRIANGLELIST)
			top = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		else if (topology == PRIMITIVE_TOPOLOGY_TRIANGLESTRIP)
			top = D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;

		m_List->IASetPrimitiveTopology(top);
	}*/


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


	void DX12CommandList::SetIndexBuffer(IBuffer* pIndexBuffer)
	{
		D3D12_INDEX_BUFFER_VIEW view = reinterpret_cast<DX12Buffer*>(pIndexBuffer)->GetIndexBufferView();
		m_List->IASetIndexBuffer(&view);
	}


	CommandListType DX12CommandList::GetType() const
	{
		return m_Type;
	}


	void DX12CommandList::TransitionBuffer(const IBuffer* pResource, ResourceState resourceState)
	{
		const DX12Buffer* pBuffer = reinterpret_cast<const DX12Buffer*>(pResource);
		m_ResourceTracker.TransitionResource(pBuffer->GetResource(), D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES, ConvertResourceState(resourceState));
	}


	void DX12CommandList::TransitionTexture(const ITexture2D* pResource, ResourceState resourceState)
	{
		const DX12Texture2D* pTexture = reinterpret_cast<const DX12Texture2D*>(pResource);
		m_ResourceTracker.TransitionResource(pTexture->GetResource(), D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES, ConvertResourceState(resourceState));
	}


	void DX12CommandList::VSSetConstantBuffers(const IBuffer* const * ppBuffers, uint32 numBuffers, uint32 startSlot)
	{
		for (uint32 i = 0; i < numBuffers; i++)
		{
			InternalSetResourceDescriptor(m_hResourceDescriptorTables[0].CPU, reinterpret_cast<const DX12Buffer*>(ppBuffers[i])->GetDescriptorHandle().CPU, startSlot + i, 0);
		}
	}


	void DX12CommandList::VSSetTextures(const ITexture2D* const* ppTextures, uint32 numTextures, uint32 startSlot)
	{
		for (uint32 i = 0; i < numTextures; i++)
		{
			InternalSetResourceDescriptor(m_hResourceDescriptorTables[0].CPU, reinterpret_cast<const DX12Texture2D*>(ppTextures[i])->GetDescriptorHandle().CPU, startSlot + i, 1);
		}
	}


	void DX12CommandList::VSSetSamplers(const ISamplerState* const* ppSamplerStates, uint32 numSamplers, uint32 startSlot)
	{
		for (uint32 i = 0; i < numSamplers; i++)
		{
			InternalSetSamplerDescriptor(m_hSamplerDescriptorTables[0].CPU, reinterpret_cast<const DX12SamplerState*>(ppSamplerStates[i])->GetDescriptorHandle().CPU, startSlot + i);
		}
	}


	void DX12CommandList::HSSetConstantBuffers(const IBuffer* const * ppBuffers, uint32 numBuffers, uint32 startSlot)
	{
		for (uint32 i = 0; i < numBuffers; i++)
		{
			InternalSetResourceDescriptor(m_hResourceDescriptorTables[1].CPU, reinterpret_cast<const DX12Buffer*>(ppBuffers[i])->GetDescriptorHandle().CPU, startSlot + i, 0);
		}
	}


	void DX12CommandList::HSSetTextures(const ITexture2D* const* ppTextures, uint32 numTextures, uint32 startSlot)
	{
		for (uint32 i = 0; i < numTextures; i++)
		{
			InternalSetResourceDescriptor(m_hResourceDescriptorTables[1].CPU, reinterpret_cast<const DX12Texture2D*>(ppTextures[i])->GetDescriptorHandle().CPU, startSlot + i, 1);
		}
	}


	void DX12CommandList::HSSetSamplers(const ISamplerState* const* ppSamplerStates, uint32 numSamplers, uint32 startSlot)
	{
		for (uint32 i = 0; i < numSamplers; i++)
		{
			InternalSetSamplerDescriptor(m_hSamplerDescriptorTables[1].CPU, reinterpret_cast<const DX12SamplerState*>(ppSamplerStates[i])->GetDescriptorHandle().CPU, startSlot + i);
		}
	}


	void DX12CommandList::DSSetConstantBuffers(const IBuffer* const * ppBuffers, uint32 numBuffers, uint32 startSlot)
	{
		for (uint32 i = 0; i < numBuffers; i++)
		{
			InternalSetResourceDescriptor(m_hResourceDescriptorTables[2].CPU, reinterpret_cast<const DX12Buffer*>(ppBuffers[i])->GetDescriptorHandle().CPU, startSlot + i, 0);
		}
	}


	void DX12CommandList::DSSetTextures(const ITexture2D* const* ppTextures, uint32 numTextures, uint32 startSlot)
	{
		for (uint32 i = 0; i < numTextures; i++)
		{
			InternalSetResourceDescriptor(m_hResourceDescriptorTables[2].CPU, reinterpret_cast<const DX12Texture2D*>(ppTextures[i])->GetDescriptorHandle().CPU, startSlot + i, 1);
		}
	}


	void DX12CommandList::DSSetSamplers(const ISamplerState* const* ppSamplerStates, uint32 numSamplers, uint32 startSlot)
	{
		for (uint32 i = 0; i < numSamplers; i++)
		{
			InternalSetSamplerDescriptor(m_hSamplerDescriptorTables[2].CPU, reinterpret_cast<const DX12SamplerState*>(ppSamplerStates[i])->GetDescriptorHandle().CPU, startSlot + i);
		}
	}


	void DX12CommandList::GSSetConstantBuffers(const IBuffer* const * ppBuffers, uint32 numBuffers, uint32 startSlot)
	{
		for (uint32 i = 0; i < numBuffers; i++)
		{
			InternalSetResourceDescriptor(m_hResourceDescriptorTables[3].CPU, reinterpret_cast<const DX12Buffer*>(ppBuffers[i])->GetDescriptorHandle().CPU, startSlot + i, 0);
		}
	}


	void DX12CommandList::GSSetTextures(const ITexture2D* const* ppTextures, uint32 numTextures, uint32 startSlot)
	{
		for (uint32 i = 0; i < numTextures; i++)
		{
			InternalSetResourceDescriptor(m_hResourceDescriptorTables[3].CPU, reinterpret_cast<const DX12Texture2D*>(ppTextures[i])->GetDescriptorHandle().CPU, startSlot + i, 1);
		}
	}


	void DX12CommandList::GSSetSamplers(const ISamplerState* const* ppSamplerStates, uint32 numSamplers, uint32 startSlot)
	{
		for (uint32 i = 0; i < numSamplers; i++)
		{
			InternalSetSamplerDescriptor(m_hSamplerDescriptorTables[3].CPU, reinterpret_cast<const DX12SamplerState*>(ppSamplerStates[i])->GetDescriptorHandle().CPU, startSlot + i);
		}
	}


	void DX12CommandList::PSSetConstantBuffers(const IBuffer* const * ppBuffers, uint32 numBuffers, uint32 startSlot)
	{
		for (uint32 i = 0; i < numBuffers; i++)
		{
			InternalSetResourceDescriptor(m_hResourceDescriptorTables[4].CPU, reinterpret_cast<const DX12Buffer*>(ppBuffers[i])->GetDescriptorHandle().CPU, startSlot + i, 0);
		}
	}


	void DX12CommandList::PSSetTextures(const ITexture2D* const* ppTextures, uint32 numTextures, uint32 startSlot)
	{
		for (uint32 i = 0; i < numTextures; i++)
		{
			InternalSetResourceDescriptor(m_hResourceDescriptorTables[4].CPU, reinterpret_cast<const DX12Texture2D*>(ppTextures[i])->GetDescriptorHandle().CPU, startSlot + i, 1);
		}
	}


	void DX12CommandList::PSSetSamplers(const ISamplerState* const* ppSamplerStates, uint32 numSamplers, uint32 startSlot)
	{
		for (uint32 i = 0; i < numSamplers; i++)
		{
			InternalSetSamplerDescriptor(m_hSamplerDescriptorTables[4].CPU, reinterpret_cast<const DX12SamplerState*>(ppSamplerStates[i])->GetDescriptorHandle().CPU, startSlot + i);
		}
	}


	void DX12CommandList::UpdateBuffer(IBuffer* pResource, const ResourceData* pData)
	{
		m_ResourceTracker.FlushBarriers(m_List.Get());

		//Upload bufferdata to GPU
		DX12Allocation allocation = m_BufferAllocator.Allocate(pData->SizeInBytes, D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT);
		memcpy(allocation.pCPU, pData->pData, pData->SizeInBytes);
		
		//Perform GPU copy
		m_List->CopyBufferRegion(reinterpret_cast<DX12Buffer*>(pResource)->GetResource(), 0, allocation.pPageResource, allocation.Offset, pData->SizeInBytes);
	}


	void DX12CommandList::UpdateTexture(ITexture2D* pResource, const ResourceData* pData, uint32 subresource)
	{
		m_ResourceTracker.FlushBarriers(m_List.Get());

		//Upload texturedata to GPU
		DX12Allocation allocation = m_TextureAllocator.Allocate(pData->SizeInBytes, D3D12_TEXTURE_DATA_PITCH_ALIGNMENT);
		memcpy(allocation.pCPU, pData->pData, pData->SizeInBytes);
		
		//Setup texture copy info
		DX12Texture2D* pDX12Texture = reinterpret_cast<DX12Texture2D*>(pResource);
		Texture2DDesc desc = pDX12Texture->GetDesc();

		//Setup dst
		D3D12_TEXTURE_COPY_LOCATION dst = {};
		dst.pResource = pDX12Texture->GetResource();
		dst.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
		dst.SubresourceIndex = subresource;

		//Setup src
		D3D12_SUBRESOURCE_FOOTPRINT srcPitchedDesc = { };
		srcPitchedDesc.Format = ConvertFormat(desc.Format);
		srcPitchedDesc.Width = desc.Width;
		srcPitchedDesc.Height = desc.Height;
		srcPitchedDesc.Depth = 1;
		srcPitchedDesc.RowPitch = Math::AlignUp(srcPitchedDesc.Width * StrideInBytesFromResourceFormat(desc.Format), D3D12_TEXTURE_DATA_PITCH_ALIGNMENT);

		D3D12_PLACED_SUBRESOURCE_FOOTPRINT placedTexture2D = { 0 };
		placedTexture2D.Offset = allocation.Offset;
		placedTexture2D.Footprint = srcPitchedDesc;

		D3D12_TEXTURE_COPY_LOCATION src = {};
		src.pResource = allocation.pPageResource;
		src.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
		src.PlacedFootprint = placedTexture2D;
		
		//Perform GPU copy
		m_List->CopyTextureRegion(&dst, 0, 0, 0, &src, nullptr);
	}


	void DX12CommandList::CopyBuffer(IBuffer* pDst, IBuffer* pSrc)
	{
		m_List->CopyResource(reinterpret_cast<DX12Buffer*>(pDst)->GetResource(), reinterpret_cast<DX12Buffer*>(pSrc)->GetResource());
	}


	void DX12CommandList::DrawInstanced(uint32 vertexCountPerInstance, uint32 instanceCount, uint32 startVertexLocation, uint32 startInstanceLocation)
	{
		m_ResourceTracker.FlushBarriers(m_List.Get());

		InternalCopyAndSetDescriptors();
		m_List->DrawInstanced(vertexCountPerInstance, instanceCount, startVertexLocation, startInstanceLocation);
	}

	void DX12CommandList::DrawIndexedInstanced(uint32 indexCountPerInstance, uint32 instanceCount, uint32 startIndexLocation, uint32 baseVertexLocation, uint32 startInstanceLocation)
	{
	}


	void DX12CommandList::SetName(const char* pName)
	{
		m_List->SetName(StringToWidestring(pName).c_str());
	}


	void* DX12CommandList::GetNativeHandle() const
	{
		return m_List.Get();
	}


	void DX12CommandList::Close()
	{
		m_ResourceTracker.FlushBarriers(m_List.Get());
		m_List->Close();
	}


	void DX12CommandList::Reset()
	{
		//Reset commandallocator and commandlist
		if (FAILED(m_Allocator->Reset()))
		{
			LOG_DEBUG_ERROR("DX12: Failed to reset commandallocator\n");
		}

		if (FAILED(m_List->Reset(m_Allocator.Get(), nullptr)))
		{
			LOG_DEBUG_ERROR("DX12: Failed to reset commandlist\n");
		}
		
		//Reset allocators
		m_BufferAllocator.Reset();
		m_TextureAllocator.Reset();
		m_ResourceAllocator.Reset();
		m_SamplerAllocator.Reset();

		//Allocate new descriptors after reset
		AllocateDescriptors();
	}


	void DX12CommandList::Init(ID3D12Device* pDevice, CommandListType type, const DX12DescriptorHandle& nullSampler, const DX12DescriptorHandle& nullSRV, const DX12DescriptorHandle& nullUAV, const DX12DescriptorHandle& nullCBV)
	{
		using namespace Microsoft::WRL;

		//Create commandallocator
		D3D12_COMMAND_LIST_TYPE cType = D3D12_COMMAND_LIST_TYPE_DIRECT;
		if (type == COMMAND_LIST_TYPE_GRAPHICS)
			cType = D3D12_COMMAND_LIST_TYPE_DIRECT;
		else if (type == COMMAND_LIST_TYPE_COMPUTE)
			cType = D3D12_COMMAND_LIST_TYPE_COMPUTE;
		else if (type == COMMAND_LIST_TYPE_COPY)
			cType = D3D12_COMMAND_LIST_TYPE_COPY;

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

				//Set type
				m_Type = type;

				//Start list in a closed state
				m_List->Close();

				//Create uploadallocators
				m_BufferAllocator.Init(pDevice);
				m_TextureAllocator.Init(pDevice, MB(16));

				//Create frame descriptor allocators
				m_ResourceAllocator.Init(pDevice, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 256, true);
				m_SamplerAllocator.Init(pDevice, D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER, 256, true);

				//Cache descriptor size
				m_ResourceDescriptorSize = m_ResourceAllocator.GetDescriptorSize();
				m_SamplerDescriptorSize = m_SamplerAllocator.GetDescriptorSize();

				//Set nulldescriptors
				m_NullDescriptors[SAMPLER_DESCRIPTOR_INDEX] = nullSampler;
				m_NullDescriptors[CBV_DESCRIPTOR_INDEX] = nullCBV;
				m_NullDescriptors[SRV_DESCRIPTOR_INDEX] = nullSRV;
				m_NullDescriptors[UAV_DESCRIPTOR_INDEX] = nullUAV;

				//Allocate new descriptors
				AllocateDescriptors();
			}
		}
	}


	void DX12CommandList::AllocateDescriptors()
	{
		//Allocate descriptors
		DX12DescriptorHandle hResource = m_ResourceAllocator.Allocate(120);
		DX12DescriptorHandle hSampler = m_SamplerAllocator.Allocate(40);

		for (uint64 i = 0; i < 5; i++)
		{
			//Set descriptor tables for resources
			m_hResourceDescriptorTables[i] = DX12DescriptorHandle(hResource, (uint64)m_ResourceDescriptorSize * (i * 24));
			//Set descriptor tables for samplers
			m_hSamplerDescriptorTables[i] = DX12DescriptorHandle(hSampler, (uint64)m_SamplerDescriptorSize * (i * 8));
		}

		//Set all new descriptors to nulldescriptors
		for (uint32 shaderStage = 0; shaderStage < 5; shaderStage++)
		{
			for (uint32 slot = 0; slot < 8; slot++)
			{
				InternalSetSamplerDescriptor(	m_hSamplerDescriptorTables[shaderStage].CPU,	m_NullDescriptors[SAMPLER_DESCRIPTOR_INDEX].CPU,	slot);
				InternalSetResourceDescriptor(	m_hResourceDescriptorTables[shaderStage].CPU,	m_NullDescriptors[CBV_DESCRIPTOR_INDEX].CPU,	slot, 0);
				InternalSetResourceDescriptor(	m_hResourceDescriptorTables[shaderStage].CPU,	m_NullDescriptors[SRV_DESCRIPTOR_INDEX].CPU,	slot, 1);
				InternalSetResourceDescriptor(	m_hResourceDescriptorTables[shaderStage].CPU,	m_NullDescriptors[UAV_DESCRIPTOR_INDEX].CPU,	slot, 2);
			}
		}
	}


	void DX12CommandList::InternalCopyAndSetDescriptors()
	{
		//Perform copy of resourcedescriptors
		m_ResourceCache.CopyDescriptors(m_Device.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		m_SamplerCache.CopyDescriptors(m_Device.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);

		//Set heap and descriptortables at predefine slots. (See default rootsignature in DX12PipelineState)
		ID3D12DescriptorHeap* ppHeaps[] = { m_ResourceAllocator.GetHeap(), m_SamplerAllocator.GetHeap() };
		m_List->SetDescriptorHeaps(2, ppHeaps);

		//VS
		m_List->SetGraphicsRootDescriptorTable(0, m_hResourceDescriptorTables[0].GPU);
		m_List->SetGraphicsRootDescriptorTable(1, m_hSamplerDescriptorTables[0].GPU);

		//HS
		m_List->SetGraphicsRootDescriptorTable(2, m_hResourceDescriptorTables[1].GPU);
		m_List->SetGraphicsRootDescriptorTable(3, m_hSamplerDescriptorTables[1].GPU);

		//DS
		m_List->SetGraphicsRootDescriptorTable(4, m_hResourceDescriptorTables[2].GPU);
		m_List->SetGraphicsRootDescriptorTable(5, m_hSamplerDescriptorTables[2].GPU);

		//GS
		m_List->SetGraphicsRootDescriptorTable(6, m_hResourceDescriptorTables[3].GPU);
		m_List->SetGraphicsRootDescriptorTable(7, m_hSamplerDescriptorTables[3].GPU);
		
		//PS
		m_List->SetGraphicsRootDescriptorTable(8, m_hResourceDescriptorTables[4].GPU);
		m_List->SetGraphicsRootDescriptorTable(9, m_hSamplerDescriptorTables[4].GPU);

		//Clear caches
		m_ResourceCache.Clear();
		m_SamplerCache.Clear();

		//Allocate new descriptors after reset
		AllocateDescriptors();
	}
}
#endif
