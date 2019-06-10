#pragma once
#include "Graphics/ISamplerState.h"
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

		DX12SamplerState(ID3D12Device* pDevice, DX12DescriptorHandle hDescriptor, const SamplerDesc& desc);
		~DX12SamplerState();

		virtual SamplerDesc GetDesc() const override final;

	private:
		void Init(ID3D12Device* pDevice, DX12DescriptorHandle hDescriptor, const SamplerDesc& desc);

		void SetDescriptorHandle(DX12DescriptorHandle hDescriptor);
		DX12DescriptorHandle GetDescriptorHandle() const;

	private:
		DX12DescriptorHandle m_Descriptor;
		SamplerDesc m_Desc;
	};

	inline void DX12SamplerState::SetDescriptorHandle(DX12DescriptorHandle hDescriptor)
	{
		m_Descriptor = hDescriptor;
	}


	inline DX12DescriptorHandle DX12SamplerState::GetDescriptorHandle() const
	{
		return m_Descriptor;
	}
}
#endif