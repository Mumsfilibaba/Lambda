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
	class DX12Texture2D;

	class DX12GraphicsDevice final : public IGraphicsDevice
	{
	public:
		LAMBDA_NO_COPY(DX12GraphicsDevice);

		DX12GraphicsDevice(IWindow* pWindow, const GraphicsDeviceDesc& desc);
		~DX12GraphicsDevice();

		virtual void CreateCommandList(ICommandList** ppList, CommandListType type) const override final;
		virtual void CreateBuffer(IBuffer** ppBuffer, const ResourceData* pInitalData, const BufferDesc& desc) const override final;
		virtual void CreateTexture2D(ITexture2D** ppTexture, const ResourceData* pInitalData, const Texture2DDesc& desc) const override final;
		virtual void CreateShader(IShader** ppShader, const ShaderDesc& desc) const override final;
		virtual void CreateSamplerState(ISamplerState** ppSamplerState, const SamplerDesc& desc) const override final;
		virtual void CreateGraphicsPipelineState(IGraphicsPipelineState** ppPipelineState, const GraphicsPipelineStateDesc& desc) const override final;
		virtual void CreateRenderPass(IRenderPass** ppRenderPass, const RenderPassDesc& desc) const override final;

		virtual void DestroyCommandList(ICommandList** ppList) const override final;
		virtual void DestroyBuffer(IBuffer** ppBuffer) const override final;
		virtual void DestroyTexture2D(ITexture2D** ppTexture) const override final;
		virtual void DestroyShader(IShader** ppShader) const override final;
		virtual void DestroySamplerState(ISamplerState** ppSamplerState) const override final;
		virtual void DestroyGraphicsPipelineState(IGraphicsPipelineState** ppPipelineState) const override final;
		virtual void DestroyRenderPass(IRenderPass** ppRenderPass) const override final;
		virtual void Destroy() const override final;

		virtual void ExecuteCommandList(ICommandList* const * ppLists, uint32 numLists) const;
		virtual void ExecuteCommandListAndPresent(ICommandList* const* ppLists, uint32 numLists) const;
		virtual void Present() const override final;
		virtual void GPUWaitForFrame() const override final;
		virtual void WaitForGPU() const override final;

		virtual void* GetNativeHandle() const override final;
		virtual ITexture2D* GetDepthStencil() const override final;
		virtual ITexture2D* GetCurrentRenderTarget() const override final;
		virtual ResourceFormat GetBackBufferFormat() const override final;
		virtual uint32 GetCurrentBackBufferIndex() const override final;
		virtual uint32 GetSwapChainWidth() const override final;
		virtual uint32 GetSwapChainHeight() const override final;

	private:
		void Init(IWindow* pWindow, const GraphicsDeviceDesc& desc);
		void ReleaseBackBuffers();
		
		bool QueryAdaper(uint32 flags);
		bool CreateFactory(uint32 flags);
		bool CreateDeviceAndCommandQueues(uint32 flags);
		bool CreateCommandList();
		bool CreateSwapChain(IWindow* pWindow);
		bool CreateDescriptorHeaps();
		bool InitBackBuffers();

		bool IsDXRSupported(ID3D12Device* pDevice);

		virtual bool InternalOnEvent(const Event& event) override final;

	private:
		Microsoft::WRL::ComPtr<IDXGIFactory5> m_Factory;
		Microsoft::WRL::ComPtr<IDXGIAdapter3> m_Adapter;
		Microsoft::WRL::ComPtr<IDXGISwapChain3> m_SwapChain;
		Microsoft::WRL::ComPtr<ID3D12Device> m_Device;
		Microsoft::WRL::ComPtr<ID3D12Device5> m_DXRDevice;
		Microsoft::WRL::ComPtr<ID3D12Debug> m_Debug;

		DX12CommandList* m_pCommandList;

		DX12CommandQueue m_DirectQueue;
		DX12CommandQueue m_ComputeQueue;
		DX12CommandQueue m_CopyQueue;

		mutable DX12DescriptorAllocator m_RTAllocator;
		mutable DX12DescriptorAllocator m_DSAllocator;
		mutable DX12DescriptorAllocator m_ResourceAllocator;
		mutable DX12DescriptorAllocator m_SamplerAllocator;
		DX12DescriptorHandle m_NullSampler;
		DX12DescriptorHandle m_NullSRV;
		DX12DescriptorHandle m_NullCBV;
		DX12DescriptorHandle m_NullUAV;
		std::vector<DX12Texture2D*> m_BackBuffers;
		mutable std::vector<uint64> m_FenceValues;

		DXGI_FORMAT m_BackBufferFormat;
		uint32 m_BackBufferFlags;
		uint32 m_BackBufferHeight;
		uint32 m_BackBufferWidth;
		mutable uint32 m_CurrentBackBuffer;
		uint32 m_NumBackbuffers;
		bool m_DXRSupported;
	};
}
#endif