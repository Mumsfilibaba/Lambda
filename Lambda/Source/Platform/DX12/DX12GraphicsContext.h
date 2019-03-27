#pragma once
#include <Graphics/IGraphicsContext.h>

#if defined(LAMBDA_PLAT_WINDOWS)
namespace Lambda
{
	class DX12GraphicsContext final : public IGraphicsContext
	{
	public:
		LAMBDA_NO_COPY(DX12GraphicsContext);

		DX12GraphicsContext();
		~DX12GraphicsContext();

		virtual uint32 Release() override final;
		virtual uint32 AddRef() override final;

	private:
		void Init();

	private:
		Microsoft::WRL::ComPtr<IDXGIFactory7> m_Factory;
		Microsoft::WRL::ComPtr<IDXGIAdapter4> m_Adapter;
		Microsoft::WRL::ComPtr<ID3D12Device5> m_Device;
		Microsoft::WRL::ComPtr<ID3D12Debug3> m_Debug;
		Microsoft::WRL::ComPtr<ID3D12DebugDevice2> m_DebugDevice;
		uint32 m_References;
	};
}
#endif