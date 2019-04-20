#include <LambdaPch.h>
#include <Graphics/ITexture2D.h>
#include "DX12Texture2D.h"

#if defined(LAMBDA_PLAT_WINDOWS)
namespace Lambda
{
	DX12Texture2D::DX12Texture2D(ID3D12Resource* pResource)
		: m_Texture(nullptr),
		m_References(0)
	{
		//Init Desc
		memset(&m_Desc, 0, sizeof(m_Desc));

		//Init
		AddRef();
		InitFromResource(pResource);
	}


	DX12Texture2D::DX12Texture2D(ID3D12Device* pDevice, const Texture2DDesc& desc)
		: m_Texture(nullptr),
		m_References(0)
	{
		assert(pDevice != nullptr);

		//Init Desc
		memset(&m_Desc, 0, sizeof(m_Desc));

		//Init
		AddRef();
		Init(pDevice, desc);
	}
	

	DX12Texture2D::~DX12Texture2D()
	{
	}


	Texture2DDesc DX12Texture2D::GetDesc() const
	{
		return m_Desc;
	}


	uint32 DX12Texture2D::Release()
	{
		IOBJECT_IMPLEMENT_RELEASE(m_References);
	}


	uint32 DX12Texture2D::AddRef()
	{
		return ++m_References;
	}


	void DX12Texture2D::Init(ID3D12Device* pDevice, const Texture2DDesc& desc)
	{
		//Create texture2d resource
		{
			//Choose heap
			CD3DX12_HEAP_PROPERTIES heapProp;
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
			clearValue.Color[0] = desc.ClearColor[0];
			clearValue.Color[1] = desc.ClearColor[1];
			clearValue.Color[2] = desc.ClearColor[2];
			clearValue.Color[3] = desc.ClearColor[3];
			clearValue.DepthStencil.Depth = desc.ClearDepth;
			clearValue.DepthStencil.Stencil = desc.ClearStencil;

			//only if we support rendertargets or depthstencil
			if (flags & D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET || flags & D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL)
			{
				pClearValue = &clearValue;
			}


			//Create resource
			CD3DX12_RESOURCE_DESC rDesc = CD3DX12_RESOURCE_DESC::Tex2D(format, desc.Width, desc.Height, (UINT16)desc.ArraySize, (UINT16)desc.MipLevels, (UINT16)desc.SampleCount, 0, flags);
			HRESULT hr = pDevice->CreateCommittedResource(&heapProp, D3D12_HEAP_FLAG_NONE, &rDesc, state, pClearValue, IID_PPV_ARGS(&m_Texture));
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


	void DX12Texture2D::InitFromResource(ID3D12Resource* pResource)
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


	void DX12Texture2D::SetResource(ID3D12Resource* pResource)
	{
		InitFromResource(pResource);
	}


	void DX12Texture2D::SetDescriptorHandle(const DX12DescriptorHandle& hDescriptor)
	{
		m_hDescriptor = hDescriptor;
	}
}
#endif