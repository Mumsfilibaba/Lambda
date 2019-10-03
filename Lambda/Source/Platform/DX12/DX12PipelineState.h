#pragma once
#include "Graphics/Core/IPipelineState.h"
#if defined(LAMBDA_PLAT_WINDOWS)
	#include "Graphics/Core/DeviceObjectBase.h"
	#include <wrl/client.h>
	#include <d3d12.h>

namespace Lambda
{
	class DX12Device;

	//-----------------
	//DX12PipelineState
	//-----------------

	class DX12PipelineState final : public DeviceObjectBase<DX12Device, IPipelineState>
	{
		friend class DX12DeviceContext;

	public:
		LAMBDA_NO_COPY(DX12PipelineState);

		DX12PipelineState(DX12Device* pDevice, const PipelineStateDesc& desc);
		~DX12PipelineState() = default;

		virtual void CreateShaderVariableTable(IShaderVariableTable** ppVariableTable) override final;
        virtual void SetName(const char* pName) override final;
		virtual void* GetNativeHandle() const override final;
		virtual const PipelineStateDesc& GetDesc() const override final;
	private:
		void Init(const PipelineStateDesc& desc);
		inline ID3D12PipelineState* GetPipelineState() const { return m_State.Get(); }
		inline ID3D12RootSignature* GetRootSignature() const { return m_RootSignature.Get(); }
	private:
		Microsoft::WRL::ComPtr<ID3D12PipelineState> m_State;
		Microsoft::WRL::ComPtr<ID3D12RootSignature> m_RootSignature;
		std::string m_Name;
	};
}
#endif
