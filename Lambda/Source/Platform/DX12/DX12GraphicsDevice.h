#pragma once
#include <Graphics/IGraphicsDevice.h>
#include <System/IWindow.h>

#if defined(LAMBDA_PLAT_WINDOWS)
	#include "DX12CommandList.h"
	#include "DX12DescriptorAllocator.h"
	#include <dxgi1_6.h>
	#include <vector>

namespace Lambda
{
	class DX12Texture2D;

	class DX12GraphicsDevice final : public IGraphicsDevice
	{
	public:
		LAMBDA_NO_COPY(DX12GraphicsDevice);

		DX12GraphicsDevice(IWindow* pWindow, GraphicsContextFlags flags);
		~DX12GraphicsDevice();

		virtual void CreateBuffer(IBuffer** ppBuffer, const ResourceData* pInitalData, const BufferDesc& desc) const override final;
		virtual void CreateTexture2D(ITexture2D** ppTexture, const ResourceData* pInitalData, const Texture2DDesc& desc) const override final;
		virtual void CreateShader(IShader** ppShader, const ShaderDesc& desc) const override final;
		virtual void CreateGraphicsPipelineState(IGraphicsPipelineState** ppPSO, const GraphicsPipelineStateDesc& desc) const override final;

		virtual void ExecuteCommandList(ICommandList* const * ppLists, uint32 numLists) const;
		virtual void Present(uint32 verticalSync) const override final;
		virtual void GPUWaitForFrame() const override final;
		virtual void WaitForGPU() const override final;

		virtual ITexture2D* GetCurrentRenderTarget() override final;
		virtual uint32 GetCurrentBackBufferIndex() const override final;
		virtual void* GetNativeHandle() const override final;

		virtual uint32 Release() override final;
		virtual uint32 AddRef() override final;

	private:
		void Init(IWindow* pWindow, GraphicsContextFlags flags);
		void ReleaseBackBuffers();
		
		bool CreateFactory(GraphicsContextFlags flags);
		bool QueryAdaper(GraphicsContextFlags flags);
		bool CreateDeviceAndCommandQueue(GraphicsContextFlags flags);
		bool CreateCommandList();
		bool CreateSwapChain(IWindow* pWindow);
		bool CreateDescriptorHeaps();
		bool InitBackBuffers();

		virtual bool InternalOnEvent(const Event& event) override final;

	private:
		Microsoft::WRL::ComPtr<IDXGIFactory7> m_Factory;
		Microsoft::WRL::ComPtr<IDXGIAdapter4> m_Adapter;
		Microsoft::WRL::ComPtr<IDXGISwapChain4> m_SwapChain;
		Microsoft::WRL::ComPtr<ID3D12Device5> m_Device;
		Microsoft::WRL::ComPtr<ID3D12Debug3> m_Debug;
		Microsoft::WRL::ComPtr<ID3D12DebugDevice1> m_DebugDevice;
		Microsoft::WRL::ComPtr<ID3D12Fence> m_Fence;
		Microsoft::WRL::ComPtr<ID3D12CommandQueue> m_Queue;
		mutable std::vector<ID3D12CommandList*> m_PendingLists;

		DX12CommandList* m_pCommandList;
		DX12DescriptorAllocator* m_pRTAllocator;
		DX12DescriptorAllocator* m_pDSAllocator;
		DX12DescriptorAllocator* m_pResourceAllocator;
		DX12DescriptorAllocator* m_pSamplerAllocator;
		DX12DescriptorHandle m_NullSampler;
		DX12DescriptorHandle m_NullSRV;
		DX12DescriptorHandle m_NullCBV;
		std::vector<DX12Texture2D*> m_BackBuffers;
		mutable std::vector<uint64> m_FenceValues;

		HANDLE m_GPUWaitEvent;
		DXGI_FORMAT m_BackBufferFormat;
		uint32 m_BackBufferFlags;
		mutable uint32 m_CurrentBackBuffer;
		uint32 m_NumBackbuffers;
		uint32 m_References;
	};
}
#endif