#include <LambdaPch.h>
#include "DX12RenderTarget.h"

#if defined(LAMBDA_PLAT_WINDOWS)
namespace Lambda
{
	DX12RenderTarget::DX12RenderTarget()
		: m_Texture(nullptr),
		m_ResourceState(D3D12_RESOURCE_STATE_COMMON),
		m_Descriptor(),
		m_References(0)
	{
		m_Descriptor.ptr = 0;
		
		AddRef();
		Init();
	}
	
	DX12RenderTarget::~DX12RenderTarget()
	{
	}

	uint32 DX12RenderTarget::Release()
	{
		IOBJECT_IMPLEMENT_RELEASE(m_References);
	}

	uint32 DX12RenderTarget::AddRef()
	{
		return ++m_References;
	}
	
	void DX12RenderTarget::Init()
	{
	}
	
	void DX12RenderTarget::SetResource(ID3D12Resource* pResource)
	{
		m_Texture.Reset();
		m_Texture = pResource;
	}

	void DX12RenderTarget::SetDescriptorHandle(D3D12_CPU_DESCRIPTOR_HANDLE descriptor)
	{
		m_Descriptor = descriptor;
	}

	void DX12RenderTarget::SetResourceState(D3D12_RESOURCE_STATES state)
	{
		m_ResourceState = state;
	}

	void DX12RenderTarget::SetDescriptorIndex(uint32 index)
	{
		m_DescriptorIndex = index;
	}
}
#endif