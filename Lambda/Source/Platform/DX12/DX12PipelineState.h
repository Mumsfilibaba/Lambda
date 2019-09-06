#pragma once
#include "Graphics/IPipelineState.h"
#if defined(LAMBDA_PLAT_WINDOWS)
	#include <wrl/client.h>
	#include <d3d12.h>

namespace Lambda
{
	class DX12GraphicsPipelineState final : public IGraphicsPipelineState
	{
		friend class DX12CommandList;

	public:
		LAMBDA_NO_COPY(DX12GraphicsPipelineState);

		DX12GraphicsPipelineState(ID3D12Device* pDevice, const GraphicsPipelineStateDesc& desc);
		~DX12GraphicsPipelineState() = default;

        virtual void SetName(const char* pName) override final;
		virtual void* GetNativeHandle() const override final;

	private:
		void Init(ID3D12Device* pDevice, const GraphicsPipelineStateDesc& desc);
		ID3D12PipelineState* GetPipelineState() const;
		ID3D12RootSignature* GetRootSignature() const;

	private:
		Microsoft::WRL::ComPtr<ID3D12PipelineState> m_State;
		Microsoft::WRL::ComPtr<ID3D12RootSignature> m_RootSignature;
		std::string m_Name;
	};


	inline ID3D12PipelineState* DX12GraphicsPipelineState::GetPipelineState() const
	{
		return m_State.Get();
	}


	inline ID3D12RootSignature* DX12GraphicsPipelineState::GetRootSignature() const
	{
		return m_RootSignature.Get();
	}
}
#endif
