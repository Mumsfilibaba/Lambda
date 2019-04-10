#pragma once
#include <Graphics/ITexture2D.h>
#if defined(LAMBDA_PLAT_WINDOWS)
	#include <wrl/client.h>
	#include "DX12DescriptorHandle.h"
	#include "DX12Conversions.inl"

namespace Lambda
{
	class DX12Texture2D final : public ITexture2D
	{
		friend class DX12CommandList;
		friend class DX12GraphicsDevice;

	public:
		LAMBDA_NO_COPY(DX12Texture2D);

		DX12Texture2D(ID3D12Resource* pResource);
		DX12Texture2D(ID3D12Device5* pDevice, const Texture2DDesc& desc);
		~DX12Texture2D();

		virtual uint32 Release() override final;
		virtual uint32 AddRef() override final;

	private:
		void Init(ID3D12Device5* pDevice, const Texture2DDesc& desc);
		void InitFromResource(ID3D12Resource* pResource);

		void SetResource(ID3D12Resource* pResource);
		void SetDescriptorHandle(const DX12DescriptorHandle& hDescriptor);

		ID3D12Resource* GetResource() const;
		DX12DescriptorHandle GetDescriptorHandle() const;
		D3D12_GPU_VIRTUAL_ADDRESS GetGPUVirtualAdress() const;

	private:
		Microsoft::WRL::ComPtr<ID3D12Resource> m_Texture;
		DX12DescriptorHandle m_hDescriptor;
		D3D12_GPU_VIRTUAL_ADDRESS m_Adress;

		Texture2DDesc m_Desc;
		uint32 m_References;
	};

	inline ID3D12Resource* Lambda::DX12Texture2D::GetResource() const
	{
		return m_Texture.Get();
	}
	
	inline DX12DescriptorHandle DX12Texture2D::GetDescriptorHandle() const
	{
		return m_hDescriptor;
	}

	inline D3D12_GPU_VIRTUAL_ADDRESS DX12Texture2D::GetGPUVirtualAdress() const
	{
		return m_Adress;
	}
}
#endif