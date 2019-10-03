#include "LambdaPch.h"
#if defined(LAMBDA_PLAT_WINDOWS)
	#include "DX12Device.h"
	#include "DX12Texture.h"

namespace Lambda
{
	DX12Texture::DX12Texture(DX12Device* pDevice, ID3D12Resource* pResource)
		: DeviceObjectBase<DX12Device, ITexture>(pDevice),
		m_Texture(nullptr),
		m_hDescriptor(),
		m_Desc()
	{
		//Init
		InitFromResource(pResource);
	}


	DX12Texture::DX12Texture(DX12Device* pDevice, const TextureDesc& desc)
		: DeviceObjectBase<DX12Device, ITexture>(pDevice),
		m_Texture(nullptr),
		m_hDescriptor(),
		m_Desc()
	{
		//Init
		Init(desc);
	}


	const TextureDesc& DX12Texture::GetDesc() const
	{
		return m_Desc;
	}


	void* DX12Texture::GetNativeHandle() const
	{
		return m_Texture.Get();
	}


	void DX12Texture::Init(const TextureDesc& desc)
	{
		//Create texture2d resource
		{
			//Choose heap
			CD3DX12_HEAP_PROPERTIES heapProp = {};
			D3D12_RESOURCE_STATES state = D3D12_RESOURCE_STATE_COMMON;

			//We want a default heap
			if (desc.Usage == RESOURCE_USAGE_DEFAULT)
			{
				heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
			}


			//Set flags
			D3D12_RESOURCE_FLAGS flags = D3D12_RESOURCE_FLAG_NONE;
			if (desc.Flags & TEXTURE_FLAGS_RENDER_TARGET)
				flags |= D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
			if (desc.Flags & TEXTURE_FLAGS_DEPTH_STENCIL)
				flags |= D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
			if (!(desc.Flags & TEXTURE_FLAGS_SHADER_RESOURCE))
				flags |= D3D12_RESOURCE_FLAG_DENY_SHADER_RESOURCE;


			//Get format
			DXGI_FORMAT format = ConvertFormat(desc.Format);

			//Optimized clearvalue
			D3D12_CLEAR_VALUE* pClearValue = nullptr;
			D3D12_CLEAR_VALUE clearValue = {};
			clearValue.Format = format;
			clearValue.Color[0] = 0.0f;
			clearValue.Color[1] = 0.0f;
			clearValue.Color[2] = 0.0f;
			clearValue.Color[3] = 0.0f;
			clearValue.DepthStencil.Depth = 1.0f;
			clearValue.DepthStencil.Stencil = 0;

			//only if we support rendertargets or depthstencil
			if (flags & D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET || flags & D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL)
			{
				pClearValue = &clearValue;
			}


			//Create resource
			CD3DX12_RESOURCE_DESC rDesc = CD3DX12_RESOURCE_DESC::Tex2D(format, desc.Width, desc.Height, (UINT16)desc.ArraySize, (UINT16)desc.MipLevels, (UINT16)desc.SampleCount, 0, flags);
			HRESULT hr = m_pDevice->GetDevice()->CreateCommittedResource(&heapProp, D3D12_HEAP_FLAG_NONE, &rDesc, state, pClearValue, IID_PPV_ARGS(&m_Texture));
			if (FAILED(hr))
			{
				LOG_DEBUG_ERROR("DX12: Failed to create Texture2D.\n");
				return;
			}
			else
			{
				LOG_DEBUG_INFO("DX12: Created Texture2D.\n");
				
				m_Desc = desc;
			}
		}
	}


	void DX12Texture::InitFromResource(ID3D12Resource* pResource)
	{
		m_Texture.Reset();
		m_Texture = pResource;

		if (pResource)
		{
			//Get description
			D3D12_RESOURCE_DESC desc = pResource->GetDesc();
			m_Desc.Width = (uint32)desc.Width;
			m_Desc.Height = desc.Height;
			m_Desc.ArraySize = desc.DepthOrArraySize;
			m_Desc.Format = ConvertDXGIFormat(desc.Format);
			m_Desc.MipLevels = desc.MipLevels;
			m_Desc.SampleCount = desc.SampleDesc.Count;
			
			if (desc.Flags & D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET)
				m_Desc.Flags |= TEXTURE_FLAGS_RENDER_TARGET;
			if (desc.Flags & D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL)
				m_Desc.Flags |= TEXTURE_FLAGS_RENDER_TARGET;
			if (!(desc.Flags & D3D12_RESOURCE_FLAG_DENY_SHADER_RESOURCE))
				m_Desc.Flags |= TEXTURE_FLAGS_RENDER_TARGET;

			//Get heapproperties
			D3D12_HEAP_FLAGS heapFlags;
			D3D12_HEAP_PROPERTIES heapProp = {};
			pResource->GetHeapProperties(&heapProp, &heapFlags);

			if (heapProp.Type == D3D12_HEAP_TYPE_DEFAULT)
				m_Desc.Usage = RESOURCE_USAGE_DEFAULT;
			else if (heapProp.Type == D3D12_HEAP_TYPE_UPLOAD)
				m_Desc.Usage = RESOURCE_USAGE_DYNAMIC;
		}
	}


	void DX12Texture::SetResource(ID3D12Resource* pResource)
	{
		InitFromResource(pResource);
	}


	void DX12Texture::SetDescriptorHandle(const DX12DescriptorHandle& hDescriptor)
	{
		m_hDescriptor = hDescriptor;
	}
}
#endif
