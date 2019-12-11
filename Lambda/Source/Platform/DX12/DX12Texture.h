#pragma once
#include "Graphics/Core/ITexture.h"
#if defined(LAMBDA_PLAT_WINDOWS)
	#include <wrl/client.h>
	#include "Graphics/Core/CDeviceObjectBase.h"
	#include "DX12DescriptorHandle.h"
	#include "DX12Conversions.inl"

namespace Lambda
{
	class DX12Texture final : public CDeviceObjectBase<DX12Device, ITexture>
	{
		friend class DX12Device;
		friend class DX12DeviceContext;
	public:
		LAMBDA_NO_COPY(DX12Texture);

		DX12Texture(DX12Device* pDevice, ID3D12Resource* pResource);
		DX12Texture(DX12Device* pDevice, const STextureDesc& desc);
		~DX12Texture() = default;

		virtual void* GetNativeHandle() const override final;
		virtual const STextureDesc& GetDesc() const override final;
	private:
		void Init(const STextureDesc& desc);
		void InitFromResource(ID3D12Resource* pResource);

		void SetResource(ID3D12Resource* pResource);
		void SetDescriptorHandle(const DX12DescriptorHandle& hDescriptor);

		ID3D12Resource* GetResource() const;
		DX12DescriptorHandle GetDescriptorHandle() const;
	private:
		Microsoft::WRL::ComPtr<ID3D12Resource> m_Texture;
		DX12DescriptorHandle m_hDescriptor;
		STextureDesc m_Desc;
	};


	inline ID3D12Resource* Lambda::DX12Texture::GetResource() const
	{
		return m_Texture.Get();
	}
	

	inline DX12DescriptorHandle DX12Texture::GetDescriptorHandle() const
	{
		return m_hDescriptor;
	}
}
#endif
