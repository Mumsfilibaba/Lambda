#pragma once
#include <Graphics/ISamplerState.h>
#if defined(LAMBDA_PLAT_WINDOWS)
	#include <wrl/client.h>
	#include "DX12DescriptorHandle.h"

namespace Lambda
{
	class DX12SamplerState final : public ISamplerState
	{
		friend class DX12GraphicsDevice;
		friend class DX12CommandList;

	public:
		LAMBDA_NO_COPY(DX12SamplerState);

		DX12SamplerState(ID3D12Device* pDevice, D3D12_CPU_DESCRIPTOR_HANDLE hDescriptor, const SamplerDesc& desc);
		~DX12SamplerState();

		virtual uint32 Release() override final;
		virtual uint32 AddRef() override final;

		virtual SamplerDesc GetDesc() const override final;

	private:
		void Init(ID3D12Device* pDevice, D3D12_CPU_DESCRIPTOR_HANDLE hDescriptor, const SamplerDesc& desc);

		void SetDescriptorHandle(D3D12_CPU_DESCRIPTOR_HANDLE hDescriptor);
		D3D12_CPU_DESCRIPTOR_HANDLE GetDescriptorHandle() const;

	private:
		D3D12_CPU_DESCRIPTOR_HANDLE m_Descriptor;
		SamplerDesc m_Desc;
		uint32 m_References;
	};

	inline void DX12SamplerState::SetDescriptorHandle(D3D12_CPU_DESCRIPTOR_HANDLE hDescriptor)
	{
		m_Descriptor = hDescriptor;
	}


	inline D3D12_CPU_DESCRIPTOR_HANDLE DX12SamplerState::GetDescriptorHandle() const
	{
		return m_Descriptor;
	}
}
#endif