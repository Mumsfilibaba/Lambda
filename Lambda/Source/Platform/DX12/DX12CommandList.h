#pragma once
#include <Graphics/ICommandList.h>

#if defined(LAMBDA_PLAT_WINDOWS)
	#include <wrl/client.h>
	#include <d3d12.h>

namespace Lambda
{
	class DX12CommandList final : public ICommandList
	{
	public:
		LAMBDA_NO_COPY(DX12CommandList);

		DX12CommandList(ID3D12Device5* pDevice, CommandListType type);
		~DX12CommandList();
	
		virtual void Close() override final;
		virtual void Reset() override final;
		
		virtual uint32 Release() override final;
		virtual uint32 AddRef() override final;

	private:
		void Init(ID3D12Device5* pDevice, CommandListType type);

	private:
		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> m_Allocator;
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> m_List;
		uint32 m_References;
	};
}
#endif