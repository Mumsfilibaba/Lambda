#include "LambdaPch.h"
#if defined(LAMBDA_PLAT_WINDOWS)
	#include "DX12Device.h"
	#include "DX12SamplerState.h"

namespace Lambda
{
	DX12SamplerState::DX12SamplerState(DX12Device* pDevice, DX12DescriptorHandle hDescriptor, const SSamplerStateDesc& desc)
		: CDeviceObjectBase<DX12Device, ISamplerState>(pDevice),
		m_Desc(),
		m_Descriptor()
	{
		Init(hDescriptor, desc);
	}


	const SSamplerStateDesc& DX12SamplerState::GetDesc() const
	{
		return m_Desc;
	}


	void* DX12SamplerState::GetNativeHandle() const
	{
		return nullptr;
	}


	void DX12SamplerState::Init(DX12DescriptorHandle hDescriptor, const SSamplerStateDesc& desc)
	{
		//Create sampler
		D3D12_SAMPLER_DESC sDesc = {};
		sDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
		sDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
		sDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
		sDesc.BorderColor[0] = 0.0f;
		sDesc.BorderColor[1] = 0.0f;
		sDesc.BorderColor[2] = 0.0f;
		sDesc.BorderColor[3] = 0.0f;
		sDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;
		sDesc.Filter = D3D12_FILTER_MIN_LINEAR_MAG_MIP_POINT;
		sDesc.MaxAnisotropy = 16;
		sDesc.MinLOD = 0;
		sDesc.MaxLOD = 1;
		sDesc.MipLODBias = 0;

		m_pDevice->GetDevice()->CreateSampler(&sDesc, hDescriptor.CPU);

		//Set descriptor
		m_Descriptor = hDescriptor;
	}
}
#endif
