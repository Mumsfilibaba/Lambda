#pragma once
#include <Graphics/IGraphicsContext.h>
#include <System/IWindow.h>

#if defined(LAMBDA_PLAT_WINDOWS)
	#include <wrl/client.h>
	#include <dxgi1_6.h>
	#include <d3d12.h>
	#include <vector>

namespace Lambda
{
	class DX12GraphicsContext final : public IGraphicsContext
	{
	public:
		LAMBDA_NO_COPY(DX12GraphicsContext);

		DX12GraphicsContext(IWindow* pWindow, GraphicsContextFlags flags);
		~DX12GraphicsContext();

		virtual void* GetNativeHandle() const override final;
		virtual uint32 Release() override final;
		virtual uint32 AddRef() override final;

	private:
		void Init(IWindow* pWindow, GraphicsContextFlags flags);
		void ReleaseBackBuffers();
		
		bool CreateFactory(GraphicsContextFlags flags);
		bool QueryAdaper(GraphicsContextFlags flags);
		bool CreateDeviceAndCommandQueue(GraphicsContextFlags flags);
		bool CreateSwapChain(IWindow* pWindow);
		bool InitBackBuffers();

		virtual bool InternalOnEvent(const Event& event) override final;

	private:
		Microsoft::WRL::ComPtr<IDXGIFactory7> m_Factory;
		Microsoft::WRL::ComPtr<IDXGIAdapter4> m_Adapter;
		Microsoft::WRL::ComPtr<IDXGISwapChain4> m_SwapChain;
		Microsoft::WRL::ComPtr<ID3D12Device5> m_Device;
		Microsoft::WRL::ComPtr<ID3D12Debug3> m_Debug;
		Microsoft::WRL::ComPtr<ID3D12DebugDevice2> m_DebugDevice;
		Microsoft::WRL::ComPtr<ID3D12Fence> m_Fence;
		Microsoft::WRL::ComPtr<ID3D12CommandQueue> m_Queue;
		std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> m_BackBuffers;
		uint32 m_NumBackbuffers;
		uint32 m_References;
	};
}
#endif