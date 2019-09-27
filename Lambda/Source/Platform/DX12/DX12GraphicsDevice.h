#pragma once
#include "Graphics/IGraphicsDevice.h"
#include "System/IWindow.h"
#if defined(LAMBDA_PLAT_WINDOWS)
	#include "DX12CommandList.h"
	#include "DX12CommandQueue.h"
	#include "DX12DescriptorAllocator.h"
	#include <dxgi1_6.h>
	#include <dxgidebug.h>

namespace Lambda
{
	class DX12Texture;

	//------------------
	//DX12GraphicsDevice
	//------------------

	class DX12GraphicsDevice final : public IGraphicsDevice
	{
	public:
		LAMBDA_NO_COPY(DX12GraphicsDevice);

		DX12GraphicsDevice(const GraphicsDeviceDesc& desc);
		~DX12GraphicsDevice();

		virtual void CreateCommandList(ICommandList** ppList, CommandListType type) override final;
		virtual void CreateBuffer(IBuffer** ppBuffer, const ResourceData* pInitalData, const BufferDesc& desc) override final;
		virtual void CreateTexture(ITexture** ppTexture, const ResourceData* pInitalData, const TextureDesc& desc) override final;
		virtual void CreateShader(IShader** ppShader, const ShaderDesc& desc) override final;
		virtual void CreateSamplerState(ISamplerState** ppSamplerState, const SamplerStateDesc& desc) override final;
		virtual void CreateGraphicsPipelineState(IGraphicsPipelineState** ppPipelineState, const GraphicsPipelineStateDesc& desc) override final;
		virtual void CreateRenderPass(IRenderPass** ppRenderPass, const RenderPassDesc& desc) override final;
		virtual void CreatePipelineResourceState(IPipelineResourceState** ppResourceState, const PipelineResourceStateDesc& desc) override final;
		virtual void CreateQuery(IQuery** ppQuery, const QueryDesc& desc) override final;

		virtual void DestroyCommandList(ICommandList** ppList) override final;
		virtual void DestroyBuffer(IBuffer** ppBuffer) override final;
		virtual void DestroyTexture(ITexture** ppTexture) override final;
		virtual void DestroyShader(IShader** ppShader) override final;
		virtual void DestroySamplerState(ISamplerState** ppSamplerState) override final;
		virtual void DestroyGraphicsPipelineState(IGraphicsPipelineState** ppPipelineState) override final;
		virtual void DestroyRenderPass(IRenderPass** ppRenderPass) override final;
		virtual void DestroyResourceState(IPipelineResourceState** ppResourceState) override final;
		virtual void DestroyQuery(IQuery** ppQuery) override final;
		virtual void Destroy() const override final;

		virtual void ExecuteCommandList(ICommandList* const * ppLists, uint32 numLists) const;
		
		virtual void PresentBegin() const override final;
		virtual void PresentEnd(ICommandList* const* ppLists, uint32 numLists) const override final;

		virtual void GPUWaitForFrame() const override final;
		virtual void WaitForGPU() const override final;

		virtual DeviceProperties GetProperties() const override final;
		virtual GraphicsDeviceDesc GetDesc() const override final;
		virtual void* GetNativeHandle() const override final;
		virtual ITexture* GetDepthStencil() const override final;
		virtual ITexture* GetRenderTarget() const override final;
		virtual ResourceFormat GetBackBufferFormat() const override final;
		virtual uint32 GetBackBufferIndex() const override final;
		virtual uint32 GetSwapChainWidth() const override final;
		virtual uint32 GetSwapChainHeight() const override final;
		ID3D12Device* GetDevice() const;
	private:
		void Init(const GraphicsDeviceDesc& desc);
		bool InitBackBuffers();

		void ReleaseBackBuffers();
		
		bool QueryAdaper(uint32 flags);
		bool IsDXRSupported(ID3D12Device* pDevice);

		virtual bool OnResize(const WindowResizeEvent& event) override final;
	private:
		Microsoft::WRL::ComPtr<IDXGIFactory5>	m_Factory;
		Microsoft::WRL::ComPtr<IDXGIAdapter3>	m_Adapter;
		Microsoft::WRL::ComPtr<ID3D12Device>	m_Device;
		Microsoft::WRL::ComPtr<ID3D12Device5>	m_DXRDevice;
		Microsoft::WRL::ComPtr<ID3D12Debug>		m_Debug;
		Microsoft::WRL::ComPtr<IDXGISwapChain3>	m_SwapChain;
		DX12CommandList*						m_pCommandList;
		DX12CommandQueue						m_DirectQueue;
		DX12CommandQueue						m_ComputeQueue;
		DX12CommandQueue						m_CopyQueue;
		mutable DX12DescriptorAllocator			m_RTAllocator;
		mutable DX12DescriptorAllocator			m_DSAllocator;
		mutable DX12DescriptorAllocator			m_ResourceAllocator;
		mutable DX12DescriptorAllocator			m_SamplerAllocator;
		DX12DescriptorHandle					m_NullSampler;
		DX12DescriptorHandle					m_NullSRV;
		DX12DescriptorHandle					m_NullCBV;
		DX12DescriptorHandle					m_NullUAV;
		std::vector<DX12Texture*>				m_BackBuffers;
		mutable std::vector<uint64>				m_FenceValues;
		DXGI_FORMAT								m_BackBufferFormat;
		uint32									m_BackBufferFlags;
		uint32									m_BackBufferHeight;
		uint32									m_BackBufferWidth;
		mutable uint32							m_CurrentBackBuffer;
		uint32									m_NumBackbuffers;
		bool									m_DXRSupported;
		GraphicsDeviceDesc						m_Desc;
	public:
		static DX12GraphicsDevice& Get();
	};


	inline ID3D12Device* DX12GraphicsDevice::GetDevice() const
	{
		return m_Device.Get();
	}
}
#endif