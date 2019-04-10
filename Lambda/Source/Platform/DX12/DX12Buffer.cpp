#include <LambdaPch.h>
#include "DX12Buffer.h"

#if defined(LAMBDA_PLAT_WINDOWS)
namespace Lambda
{
	DX12Buffer::DX12Buffer(ID3D12Device5* pDevice, const BufferDesc& desc)
		: m_Buffer(nullptr),
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
	
	DX12Buffer::~DX12Buffer()
	{
	}

	void DX12Buffer::Map(void** ppData)
	{
		if (FAILED(m_Buffer->Map(0, nullptr, ppData)))
		{
			LOG_DEBUG_ERROR("DX12: Failed to map buffer.\n");
			(*ppData) = nullptr;
		}
	}

	void DX12Buffer::Unmap()
	{
		m_Buffer->Unmap(0, nullptr);
	}

	uint32 DX12Buffer::Release()
	{
		IOBJECT_IMPLEMENT_RELEASE(m_References);
	}

	uint32 DX12Buffer::AddRef()
	{
		return ++m_References;
	}

	void DX12Buffer::Init(ID3D12Device5* pDevice, const BufferDesc& desc)
	{
		//Create buffer resource
		{
			CD3DX12_HEAP_PROPERTIES heapProp;
			D3D12_RESOURCE_STATES state = D3D12_RESOURCE_STATE_COMMON;

			//We want a default heap
			if (desc.Usage == RESOURCE_USAGE_DEFAULT)
			{
				heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
			}
			//We want a uploadheap
			else if (desc.Usage == RESOURCE_USAGE_DYNAMIC)
			{
				heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
				state = D3D12_RESOURCE_STATE_GENERIC_READ;
			}

			CD3DX12_RESOURCE_DESC rDesc = CD3DX12_RESOURCE_DESC::Buffer(desc.SizeInBytes);
			HRESULT hr = pDevice->CreateCommittedResource(&heapProp, D3D12_HEAP_FLAG_NONE, &rDesc, state, nullptr, IID_PPV_ARGS(&m_Buffer));
			if (FAILED(hr))
			{
				LOG_DEBUG_ERROR("DX12: Failed to create Buffer.\n");
				return;
			}
			else
			{
				LOG_DEBUG_INFO("DX12: Created Buffer.\n");
				m_Desc = desc;
			}
		}

		//Create views
		{
			m_Adress = m_Buffer->GetGPUVirtualAddress();
			
			//Vertexbuffer
			if (desc.Flags & BUFFER_FLAGS_VERTEX_BUFFER)
			{
				m_VBV.BufferLocation = m_Adress;
				m_VBV.SizeInBytes = desc.SizeInBytes;
				m_VBV.StrideInBytes = desc.StrideInBytes;
			}
			//Indexbuffer
			else if (desc.Flags & BUFFER_FLAGS_INDEX_BUFFER)
			{
				m_IBV.BufferLocation = m_Adress;
				m_IBV.SizeInBytes = desc.SizeInBytes;
				
				//TODO: Format?
			}
		}
	}

	void DX12Buffer::SetDescriporHandle(const DX12DescriptorHandle& hDescriptor)
	{
		m_hDescriptor = hDescriptor;
	}
}
#endif