#pragma once
#include <Graphics/IRenderTarget.h>

#if defined(LAMBDA_PLAT_WINDOWS)
	#include <wrl/client.h>
	#include <d3d12.h>

namespace Lambda
{
	class DX12RenderTarget : public IRenderTarget
	{
		friend class DX12CommandList;
		friend class DX12GraphicsDevice;

	public:
		LAMBDA_NO_COPY(DX12RenderTarget);

		DX12RenderTarget();
		~DX12RenderTarget();

		virtual uint32 Release() override final;
		virtual uint32 AddRef() override final;

	private:
		void Init();

		void SetResource(ID3D12Resource* pResource);
		void SetDescriptorHandle(D3D12_CPU_DESCRIPTOR_HANDLE descriptor);
		void SetResourceState(D3D12_RESOURCE_STATES state);
		void SetDescriptorIndex(uint32 index);

		D3D12_CPU_DESCRIPTOR_HANDLE GetDescriptorHandle() const;
		D3D12_RESOURCE_STATES GetResourceState() const;
		ID3D12Resource* GetResource() const;
		uint32 GetDescriptorIndex() const;

	private:
		Microsoft::WRL::ComPtr<ID3D12Resource> m_Texture;
		D3D12_CPU_DESCRIPTOR_HANDLE m_Descriptor;
		D3D12_RESOURCE_STATES m_ResourceState;
		uint32 m_DescriptorIndex;
		uint32 m_References;
	};

	inline ID3D12Resource* DX12RenderTarget::GetResource() const
	{
		return m_Texture.Get();
	}

	inline uint32 DX12RenderTarget::GetDescriptorIndex() const
	{
		return m_DescriptorIndex;
	}

	inline D3D12_CPU_DESCRIPTOR_HANDLE DX12RenderTarget::GetDescriptorHandle() const
	{
		return m_Descriptor;
	}

	inline D3D12_RESOURCE_STATES DX12RenderTarget::GetResourceState() const
	{
		return m_ResourceState;
	}
}
#endif