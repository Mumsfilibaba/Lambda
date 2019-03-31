#include <LambdaPch.h>
#include <Graphics/ITexture2D.h>
#include "DX12Texture2D.h"

#if defined(LAMBDA_PLAT_WINDOWS)
namespace Lambda
{
	DX12Texture2D::DX12Texture2D(ID3D12Device5* pDevice, const Texture2DDesc& desc)
		: m_Texture(nullptr),
		m_State(D3D12_RESOURCE_STATE_COMMON),
		m_Adress(0),
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
	
	uint32 DX12Texture2D::Release()
	{
		IOBJECT_IMPLEMENT_RELEASE(m_References);
	}

	uint32 DX12Texture2D::AddRef()
	{
		return ++m_References;
	}

	void DX12Texture2D::Init(ID3D12Device5* pDevice, const Texture2DDesc& desc)
	{
		//Create texture2d resource
		{
			//Chose heap
			CD3DX12_HEAP_PROPERTIES heapProp;

			//We want a default heap
			if (desc.Usage == RESOURCE_USAGE_DEFAULT)
			{
				heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
				m_State = D3D12_RESOURCE_STATE_COMMON;
			}
			//We want a uploadheap
			else if (desc.Usage == RESOURCE_USAGE_DYNAMIC)
			{
				heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
				m_State = D3D12_RESOURCE_STATE_GENERIC_READ;
			}

			//Set flags
			D3D12_RESOURCE_FLAGS flags = D3D12_RESOURCE_FLAGS::D3D12_RESOURCE_FLAG_NONE;
			if (desc.Flags & TEXTURE_FLAGS_RENDER_TARGET)
				flags |= D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
			if (desc.Flags & TEXTURE_FLAGS_DEPTH_STENCIL)
			{
				flags |= D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
				if (!(desc.Flags & TEXTURE_FLAGS_SHADER_RESOURCE))
					flags |= D3D12_RESOURCE_FLAG_DENY_SHADER_RESOURCE;
			}

			//Create resource
			CD3DX12_RESOURCE_DESC rDesc = CD3DX12_RESOURCE_DESC::Tex2D(ConvertFormat(desc.Format), desc.Width, desc.Height, (UINT16)desc.ArraySize, (UINT16)desc.MipMaps, (UINT16)desc.Samples, 0, flags);
			HRESULT hr = pDevice->CreateCommittedResource(&heapProp, D3D12_HEAP_FLAG_NONE, &rDesc, m_State, nullptr, IID_PPV_ARGS(&m_Texture));
			if (FAILED(hr))
			{
				LOG_DEBUG_ERROR("DX12: Failed to create Texture2D.\n");
				return;
			}
			else
			{
				LOG_DEBUG_INFO("DX12: Created Texture2D.\n");
				
				m_Desc = desc;
				if (desc.Flags & TEXTURE_FLAGS_SHADER_RESOURCE)
					m_Adress = m_Texture->GetGPUVirtualAddress();
			}
		}
	}

	void DX12Texture2D::SetResourceState(D3D12_RESOURCE_STATES state) const
	{
		m_State = state;
	}

	void DX12Texture2D::SetDescriptor(const DX12DescriptorHandle& hDescriptor)
	{
		m_hDescriptor = hDescriptor;
	}
}
#endif