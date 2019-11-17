#pragma once
#include "Graphics/Core/IDevice.h"
#include "Core/IWindow.h"
#if defined(LAMBDA_PLAT_WINDOWS)
	#include "DX12DeviceContext.h"
	#include "DX12CommandQueue.h"
	#include "DX12DescriptorAllocator.h"
	#include <dxgi1_6.h>
	#include <dxgidebug.h>

namespace Lambda
{
	class DX12Texture;

	//----------
	//DX12Device
	//----------

	class DX12Device final : public RefCountedObject<IDevice>
	{
	public:
		LAMBDA_NO_COPY(DX12Device);

		DX12Device(const DeviceDesc& desc);
		~DX12Device();

		virtual void CreateDefferedContext(IDeviceContext** ppDefferedContext) override final;
		virtual void CreateBuffer(IBuffer** ppBuffer, const ResourceData* pInitalData, const BufferDesc& desc) override final;
		virtual void CreateTexture(ITexture** ppTexture, const ResourceData* pInitalData, const TextureDesc& desc) override final;
		virtual void CreateShader(IShader** ppShader, const ShaderDesc& desc) override final;
		virtual void CreateSamplerState(ISamplerState** ppSamplerState, const SamplerStateDesc& desc) override final;
		virtual void CreatePipelineState(IPipelineState** ppPipelineState, const PipelineStateDesc& desc) override final;
		virtual void CreateQuery(IQuery** ppQuery, const QueryDesc& desc) override final;

		virtual void ExecuteCommandList(IDeviceContext* const * ppLists, uint32 numLists) const;

		virtual const DeviceProperties& GetProperties() const override final;
		virtual const DeviceDesc& GetDesc() const override final;
		virtual void* GetNativeHandle() const override final;
		
		void GPUWaitForFrame() const;
		void WaitForGPU() const;
		
		ID3D12Device* GetDevice() const;
	private:
		void Init(const DeviceDesc& desc);
		bool InitBackBuffers();

		void ReleaseBackBuffers();
		
		bool QueryAdaper(uint32 flags);
		bool IsDXRSupported(ID3D12Device* pDevice);
	private:
		Microsoft::WRL::ComPtr<IDXGIFactory5>	m_Factory;
		Microsoft::WRL::ComPtr<IDXGIAdapter3>	m_Adapter;
		Microsoft::WRL::ComPtr<ID3D12Device>	m_Device;
		Microsoft::WRL::ComPtr<ID3D12Device5>	m_DXRDevice;
		Microsoft::WRL::ComPtr<ID3D12Debug>		m_Debug;
		Microsoft::WRL::ComPtr<IDXGISwapChain3>	m_SwapChain;
		DX12DeviceContext*	m_pImmediateContext;
		DX12CommandQueue	m_DirectQueue;
		DX12CommandQueue	m_ComputeQueue;
		DX12CommandQueue	m_CopyQueue;
		mutable DX12DescriptorAllocator	m_RTAllocator;
		mutable DX12DescriptorAllocator	m_DSAllocator;
		mutable DX12DescriptorAllocator	m_ResourceAllocator;
		mutable DX12DescriptorAllocator	m_SamplerAllocator;
		DX12DescriptorHandle	m_NullSampler;
		DX12DescriptorHandle	m_NullSRV;
		DX12DescriptorHandle	m_NullCBV;
		DX12DescriptorHandle	m_NullUAV;
		std::vector<DX12Texture*>	m_BackBuffers;
		mutable std::vector<uint64>	m_FenceValues;
		DXGI_FORMAT		m_BackBufferFormat;
		uint32			m_BackBufferFlags;
		uint32			m_BackBufferHeight;
		uint32			m_BackBufferWidth;
		mutable uint32	m_CurrentBackBuffer;
		uint32			m_NumBackbuffers;
		bool			m_DXRSupported;
		DeviceDesc		m_Desc;
	private:
		static DX12Device* s_pInstance;
	public:
		static DX12Device& Get();
	};


	inline ID3D12Device* DX12Device::GetDevice() const
	{
		return m_Device.Get();
	}
}
#endif