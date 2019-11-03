#include "LambdaPch.h"
#include "DX12DeviceContext.h"
#include "Utilities/TextureHelper.h"
#include "Utilities/StringHelper.h"
#include "Utilities/MathHelper.h"
#if defined(LAMBDA_PLAT_WINDOWS)
	#include "DX12Device.h"
	#include "DX12PipelineState.h"
	#include "DX12Buffer.h"
	#include "DX12Texture.h"
	#include "DX12SamplerState.h"
	#define SAMPLER_DESCRIPTOR_INDEX 0
	#define SRV_DESCRIPTOR_INDEX 1
	#define CBV_DESCRIPTOR_INDEX 2
	#define UAV_DESCRIPTOR_INDEX 3

namespace Lambda
{
	//-----------------
	//DX12DeviceContext
	//-----------------

	DX12DeviceContext::DX12DeviceContext(DX12Device* pDevice, DeviceContextType type, const DX12DescriptorHandle& nullSampler, const DX12DescriptorHandle& nullSRV, const DX12DescriptorHandle& nullUAV, const DX12DescriptorHandle& nullCBV)
		: DeviceObjectBase<DX12Device, IDeviceContext>(pDevice),
		m_Allocator(nullptr),
		m_List(nullptr),
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
		m_Type(DEVICE_CONTEXT_TYPE_UNKNOWN)
	{
		Init(type, nullSampler, nullSRV, nullUAV, nullCBV);
	}


	void DX12DeviceContext::Init(DeviceContextType type, const DX12DescriptorHandle& nullSampler, const DX12DescriptorHandle& nullSRV, const DX12DescriptorHandle& nullUAV, const DX12DescriptorHandle& nullCBV)
	{
		using namespace Microsoft::WRL;

		D3D12_COMMAND_LIST_TYPE listType = ConvertCommandListType(type);

		//Create commandallocator
		HRESULT hr = m_pDevice->GetDevice()->CreateCommandAllocator(listType, IID_PPV_ARGS(&m_Allocator));
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
		hr = m_pDevice->GetDevice()->CreateCommandList(1, listType, m_Allocator.Get(), nullptr, IID_PPV_ARGS(&commandList));
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
				//Set type
				m_Type = type;

				//Start list in a closed state
				m_List->Close();

				//Create uploadallocators
				m_BufferAllocator.Init(m_pDevice->GetDevice());
				m_TextureAllocator.Init(m_pDevice->GetDevice(), MB(16));

				//Create frame descriptor allocators
				m_ResourceAllocator.Init(m_pDevice->GetDevice(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 256, true);
				m_SamplerAllocator.Init(m_pDevice->GetDevice(), D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER, 256, true);

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


	void DX12DeviceContext::ClearRenderTarget(ITexture* pRenderTarget, float color[4])
	{
		//Flush barriers - needs to be transitioned outside function
		m_ResourceTracker.FlushBarriers(m_List.Get());

		//Clear rendertarget
		DX12Texture* pTarget = reinterpret_cast<DX12Texture*>(pRenderTarget);
		m_List->ClearRenderTargetView(pTarget->GetDescriptorHandle().CPU, color, 0, nullptr);
	}


	void DX12DeviceContext::ClearDepthStencil(ITexture * pDepthStencil, float depth, uint8 stencil)
	{
		//Flush barriers - needs to be transitioned outside function
		m_ResourceTracker.FlushBarriers(m_List.Get());

		//Clear depthstencil
		DX12Texture* pDSV = reinterpret_cast<DX12Texture*>(pDepthStencil);
		m_List->ClearDepthStencilView(pDSV->GetDescriptorHandle().CPU, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, depth, stencil, 0, nullptr);
	}


	void DX12DeviceContext::ResetQuery(IQuery* pQuery)
	{
	}


	void DX12DeviceContext::WriteTimeStamp(IQuery* pQuery)
	{
	}


	/*void DX12DeviceContext::SetRenderTarget(ITexture2D* pRenderTarget, ITexture2D* pDepthStencil)
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
	}*/


	void DX12DeviceContext::SetRendertargets(ITexture* const* ppRenderTargets, uint32 numRendertargets, ITexture* pDepthStencil)
	{
	}


	void DX12DeviceContext::SetViewports(const Viewport* pViewports, uint32 numViewports)
	{
		//D3D12_VIEWPORT port = { viewport.TopX, viewport.TopY, viewport.Width, viewport.Height, viewport.MinDepth, viewport.MaxDepth };
		//m_List->RSSetViewports( 1, &port );
	}


	void DX12DeviceContext::SetScissorRects(const Rectangle* pScissorRect, uint32 numScissorRects)
	{
		//D3D12_RECT rect = { (LONG)scissorRect.X, (LONG)scissorRect.Y, (LONG)scissorRect.X + (LONG)scissorRect.Width, (LONG)scissorRect.Y + (LONG)scissorRect.Height };
		//m_List->RSSetScissorRects(1, &rect);
	}


	/*void DX12DeviceContext::SetPrimtiveTopology(PrimitiveTopology topology)
	{
		D3D_PRIMITIVE_TOPOLOGY top = D3D_PRIMITIVE_TOPOLOGY_UNDEFINED;
		if (topology == PRIMITIVE_TOPOLOGY_TRIANGLELIST)
			top = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		else if (topology == PRIMITIVE_TOPOLOGY_TRIANGLESTRIP)
			top = D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;

		m_List->IASetPrimitiveTopology(top);
	}*/


	void DX12DeviceContext::SetPipelineState(IPipelineState* pPSO)
	{
		DX12PipelineState* pState = reinterpret_cast<DX12PipelineState*>(pPSO);
		m_List->SetPipelineState(pState->GetPipelineState());
		m_List->SetGraphicsRootSignature(pState->GetRootSignature());
	}

	
	void DX12DeviceContext::SetShaderVariableTable(IShaderVariableTable* pVariableTable)
	{
	}


	void DX12DeviceContext::SetVertexBuffers(IBuffer* const * ppBuffers, uint32 numBuffers, uint32 slot)
	{
		//D3D12_VERTEX_BUFFER_VIEW view = reinterpret_cast<DX12Buffer*>(pBuffer)->GetVertexBufferView();
		//m_List->IASetVertexBuffers(slot, 1, &view);
	}


	void DX12DeviceContext::SetIndexBuffer(IBuffer* pIndexBuffer, Format format)
	{
		D3D12_INDEX_BUFFER_VIEW view = reinterpret_cast<DX12Buffer*>(pIndexBuffer)->GetIndexBufferView();
		m_List->IASetIndexBuffer(&view);
	}

	
	void DX12DeviceContext::SetConstantBlocks(ShaderStage stage, uint32 offset, uint32 sizeInBytes, void* pData)
	{
	}


	DeviceContextType DX12DeviceContext::GetType() const
	{
		return m_Type;
	}


	void DX12DeviceContext::TransitionBuffer(const IBuffer* pResource, ResourceState resourceState)
	{
		const DX12Buffer* pBuffer = reinterpret_cast<const DX12Buffer*>(pResource);
		m_ResourceTracker.TransitionResource(pBuffer->GetResource(), D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES, ConvertResourceState(resourceState));
	}


	void DX12DeviceContext::TransitionTexture(const ITexture* pResource, ResourceState resourceState, uint32 startMipLevel, uint32 numMipLevels)
	{
		const DX12Texture* pTexture = reinterpret_cast<const DX12Texture*>(pResource);
		m_ResourceTracker.TransitionResource(pTexture->GetResource(), D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES, ConvertResourceState(resourceState));
	}


	/*void DX12DeviceContext::VSSetConstantBuffers(const IBuffer* const * ppBuffers, uint32 numBuffers, uint32 startSlot)
	{
		for (uint32 i = 0; i < numBuffers; i++)
		{
			InternalSetResourceDescriptor(m_hResourceDescriptorTables[0].CPU, reinterpret_cast<const DX12Buffer*>(ppBuffers[i])->GetDescriptorHandle().CPU, startSlot + i, 0);
		}
	}


	void DX12DeviceContext::VSSetTextures(const ITexture* const* ppTextures, uint32 numTextures, uint32 startSlot)
	{
		for (uint32 i = 0; i < numTextures; i++)
		{
			InternalSetResourceDescriptor(m_hResourceDescriptorTables[0].CPU, reinterpret_cast<const DX12Texture*>(ppTextures[i])->GetDescriptorHandle().CPU, startSlot + i, 1);
		}
	}


	void DX12DeviceContext::VSSetSamplers(const ISamplerState* const* ppSamplerStates, uint32 numSamplers, uint32 startSlot)
	{
		for (uint32 i = 0; i < numSamplers; i++)
		{
			InternalSetSamplerDescriptor(m_hSamplerDescriptorTables[0].CPU, reinterpret_cast<const DX12SamplerState*>(ppSamplerStates[i])->GetDescriptorHandle().CPU, startSlot + i);
		}
	}


	void DX12DeviceContext::HSSetConstantBuffers(const IBuffer* const * ppBuffers, uint32 numBuffers, uint32 startSlot)
	{
		for (uint32 i = 0; i < numBuffers; i++)
		{
			InternalSetResourceDescriptor(m_hResourceDescriptorTables[1].CPU, reinterpret_cast<const DX12Buffer*>(ppBuffers[i])->GetDescriptorHandle().CPU, startSlot + i, 0);
		}
	}


	void DX12DeviceContext::HSSetTextures(const ITexture* const* ppTextures, uint32 numTextures, uint32 startSlot)
	{
		for (uint32 i = 0; i < numTextures; i++)
		{
			InternalSetResourceDescriptor(m_hResourceDescriptorTables[1].CPU, reinterpret_cast<const DX12Texture*>(ppTextures[i])->GetDescriptorHandle().CPU, startSlot + i, 1);
		}
	}


	void DX12DeviceContext::HSSetSamplers(const ISamplerState* const* ppSamplerStates, uint32 numSamplers, uint32 startSlot)
	{
		for (uint32 i = 0; i < numSamplers; i++)
		{
			InternalSetSamplerDescriptor(m_hSamplerDescriptorTables[1].CPU, reinterpret_cast<const DX12SamplerState*>(ppSamplerStates[i])->GetDescriptorHandle().CPU, startSlot + i);
		}
	}


	void DX12DeviceContext::DSSetConstantBuffers(const IBuffer* const * ppBuffers, uint32 numBuffers, uint32 startSlot)
	{
		for (uint32 i = 0; i < numBuffers; i++)
		{
			InternalSetResourceDescriptor(m_hResourceDescriptorTables[2].CPU, reinterpret_cast<const DX12Buffer*>(ppBuffers[i])->GetDescriptorHandle().CPU, startSlot + i, 0);
		}
	}


	void DX12DeviceContext::DSSetTextures(const ITexture* const* ppTextures, uint32 numTextures, uint32 startSlot)
	{
		for (uint32 i = 0; i < numTextures; i++)
		{
			InternalSetResourceDescriptor(m_hResourceDescriptorTables[2].CPU, reinterpret_cast<const DX12Texture*>(ppTextures[i])->GetDescriptorHandle().CPU, startSlot + i, 1);
		}
	}


	void DX12DeviceContext::DSSetSamplers(const ISamplerState* const* ppSamplerStates, uint32 numSamplers, uint32 startSlot)
	{
		for (uint32 i = 0; i < numSamplers; i++)
		{
			InternalSetSamplerDescriptor(m_hSamplerDescriptorTables[2].CPU, reinterpret_cast<const DX12SamplerState*>(ppSamplerStates[i])->GetDescriptorHandle().CPU, startSlot + i);
		}
	}


	void DX12DeviceContext::GSSetConstantBuffers(const IBuffer* const * ppBuffers, uint32 numBuffers, uint32 startSlot)
	{
		for (uint32 i = 0; i < numBuffers; i++)
		{
			InternalSetResourceDescriptor(m_hResourceDescriptorTables[3].CPU, reinterpret_cast<const DX12Buffer*>(ppBuffers[i])->GetDescriptorHandle().CPU, startSlot + i, 0);
		}
	}


	void DX12DeviceContext::GSSetTextures(const ITexture* const* ppTextures, uint32 numTextures, uint32 startSlot)
	{
		for (uint32 i = 0; i < numTextures; i++)
		{
			InternalSetResourceDescriptor(m_hResourceDescriptorTables[3].CPU, reinterpret_cast<const DX12Texture*>(ppTextures[i])->GetDescriptorHandle().CPU, startSlot + i, 1);
		}
	}


	void DX12DeviceContext::GSSetSamplers(const ISamplerState* const* ppSamplerStates, uint32 numSamplers, uint32 startSlot)
	{
		for (uint32 i = 0; i < numSamplers; i++)
		{
			InternalSetSamplerDescriptor(m_hSamplerDescriptorTables[3].CPU, reinterpret_cast<const DX12SamplerState*>(ppSamplerStates[i])->GetDescriptorHandle().CPU, startSlot + i);
		}
	}


	void DX12DeviceContext::PSSetConstantBuffers(const IBuffer* const * ppBuffers, uint32 numBuffers, uint32 startSlot)
	{
		for (uint32 i = 0; i < numBuffers; i++)
		{
			InternalSetResourceDescriptor(m_hResourceDescriptorTables[4].CPU, reinterpret_cast<const DX12Buffer*>(ppBuffers[i])->GetDescriptorHandle().CPU, startSlot + i, 0);
		}
	}


	void DX12DeviceContext::PSSetTextures(const ITexture* const* ppTextures, uint32 numTextures, uint32 startSlot)
	{
		for (uint32 i = 0; i < numTextures; i++)
		{
			InternalSetResourceDescriptor(m_hResourceDescriptorTables[4].CPU, reinterpret_cast<const DX12Texture*>(ppTextures[i])->GetDescriptorHandle().CPU, startSlot + i, 1);
		}
	}


	void DX12DeviceContext::PSSetSamplers(const ISamplerState* const* ppSamplerStates, uint32 numSamplers, uint32 startSlot)
	{
		for (uint32 i = 0; i < numSamplers; i++)
		{
			InternalSetSamplerDescriptor(m_hSamplerDescriptorTables[4].CPU, reinterpret_cast<const DX12SamplerState*>(ppSamplerStates[i])->GetDescriptorHandle().CPU, startSlot + i);
		}
	}*/


	void DX12DeviceContext::UpdateBuffer(IBuffer* pResource, const ResourceData& data)
	{
		m_ResourceTracker.FlushBarriers(m_List.Get());

		//Upload bufferdata to GPU
		DX12Allocation allocation = m_BufferAllocator.Allocate(data.SizeInBytes, D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT);
		memcpy(allocation.pCPU, data.pData, data.SizeInBytes);
		
		//Perform GPU copy
		m_List->CopyBufferRegion(reinterpret_cast<DX12Buffer*>(pResource)->GetResource(), 0, allocation.pPageResource, allocation.Offset, data.SizeInBytes);
	}


	void DX12DeviceContext::UpdateTexture(ITexture* pResource, const ResourceData& data, uint32 subresource)
	{
		m_ResourceTracker.FlushBarriers(m_List.Get());

		//Upload texturedata to GPU
		DX12Allocation allocation = m_TextureAllocator.Allocate(data.SizeInBytes, D3D12_TEXTURE_DATA_PITCH_ALIGNMENT);
		memcpy(allocation.pCPU, data.pData, data.SizeInBytes);
		
		//Setup texture copy info
		DX12Texture* pDXTexture	= reinterpret_cast<DX12Texture*>(pResource);
		const TextureDesc& desc	= pDXTexture->GetDesc();

		//Setup dst
		D3D12_TEXTURE_COPY_LOCATION dst = {};
		dst.pResource			= pDXTexture->GetResource();
		dst.Type				= D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
		dst.SubresourceIndex	= subresource;

		//Setup src
		D3D12_SUBRESOURCE_FOOTPRINT srcPitchedDesc = { };
		srcPitchedDesc.Format	= ConvertFormat(desc.Format);
		srcPitchedDesc.Width	= desc.Width;
		srcPitchedDesc.Height	= desc.Height;
		srcPitchedDesc.Depth	= 1;
		srcPitchedDesc.RowPitch	= Math::AlignUp(srcPitchedDesc.Width * StrideInBytesFromResourceFormat(desc.Format), D3D12_TEXTURE_DATA_PITCH_ALIGNMENT);

		D3D12_PLACED_SUBRESOURCE_FOOTPRINT placedTexture2D = { 0 };
		placedTexture2D.Offset		= allocation.Offset;
		placedTexture2D.Footprint	= srcPitchedDesc;

		D3D12_TEXTURE_COPY_LOCATION src = {};
		src.pResource		= allocation.pPageResource;
		src.Type			= D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
		src.PlacedFootprint = placedTexture2D;
		
		//Perform GPU copy
		m_List->CopyTextureRegion(&dst, 0, 0, 0, &src, nullptr);
	}


	void DX12DeviceContext::CopyBuffer(IBuffer* pDst, IBuffer* pSrc)
	{
		m_List->CopyResource(reinterpret_cast<DX12Buffer*>(pDst)->GetResource(), reinterpret_cast<DX12Buffer*>(pSrc)->GetResource());
	}


	void DX12DeviceContext::MapBuffer(IBuffer* pBuffer, uint32 mapFlags, void** ppData)
	{
	}


	void DX12DeviceContext::UnmapBuffer(IBuffer* pBuffer)
	{
	}


	void DX12DeviceContext::ResolveTexture(ITexture* pDst, uint32 dstMipLevel, ITexture* pSrc, uint32 srcMipLevel)
	{
	}

	
	void DX12DeviceContext::GenerateMipLevels(ITexture* pTexture)
	{
	}

	
	void DX12DeviceContext::TransitionTextureStates(const TextureTransitionBarrier* pBarriers, uint32 numBarriers)
	{
	}


	void DX12DeviceContext::Draw(uint32 vertexCount, uint32 startVertex)
	{
	}


	void DX12DeviceContext::DrawIndexed(uint32 indexCount, uint32 startIndexLocation, uint32 baseVertexLocation)
	{
	}


	void DX12DeviceContext::DrawInstanced(uint32 vertexCountPerInstance, uint32 instanceCount, uint32 startVertexLocation, uint32 startInstanceLocation)
	{
		m_ResourceTracker.FlushBarriers(m_List.Get());

		InternalCopyAndSetDescriptors();
		m_List->DrawInstanced(vertexCountPerInstance, instanceCount, startVertexLocation, startInstanceLocation);
	}


	void DX12DeviceContext::DrawIndexedInstanced(uint32 indexCountPerInstance, uint32 instanceCount, uint32 startIndexLocation, uint32 baseVertexLocation, uint32 startInstanceLocation)
	{
		m_ResourceTracker.FlushBarriers(m_List.Get());

		InternalCopyAndSetDescriptors();
		m_List->DrawIndexedInstanced(indexCountPerInstance, instanceCount, startIndexLocation, baseVertexLocation, startInstanceLocation);
	}


	void DX12DeviceContext::ExecuteDefferedContext(IDeviceContext* pContext)
	{
	}


	void DX12DeviceContext::SetName(const char* pName)
	{
		m_List->SetName(StringToWidestring(pName).c_str());
	}


	void* DX12DeviceContext::GetNativeHandle() const
	{
		return m_List.Get();
	}


	void DX12DeviceContext::End()
	{
		m_ResourceTracker.FlushBarriers(m_List.Get());
		m_List->Close();
	}

	void DX12DeviceContext::Flush()
	{
	}

	void DX12DeviceContext::ClearState()
	{
	}


	void DX12DeviceContext::Begin()
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


	void DX12DeviceContext::AllocateDescriptors()
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


	void DX12DeviceContext::InternalCopyAndSetDescriptors()
	{
		//Perform copy of resourcedescriptors
		m_ResourceCache.CopyDescriptors(m_pDevice->GetDevice(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		m_SamplerCache.CopyDescriptors(m_pDevice->GetDevice(), D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);

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
