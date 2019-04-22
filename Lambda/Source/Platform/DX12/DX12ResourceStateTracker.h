#pragma once
#include <Defines.h>
#include <Types.h>
#if defined(LAMBDA_PLAT_WINDOWS)
	#include <vector>
	#include <mutex>
	#include <unordered_map>
	#include <wrl/client.h>
	#include <d3d12.h>

namespace Lambda
{
	struct DX12ResourceState
	{
		D3D12_RESOURCE_STATES State = D3D12_RESOURCE_STATE_COMMON;
		std::vector<D3D12_RESOURCE_STATES> SubResourceStates;
	};


	class DX12ResourceStateTracker final
	{
		friend class DX12GraphicsDevice;

	public:
		using ResourceMap = std::unordered_map<ID3D12Resource*, DX12ResourceState>;

		LAMBDA_NO_COPY(DX12ResourceStateTracker);

		DX12ResourceStateTracker();
		~DX12ResourceStateTracker();

		void TransitionResource(ID3D12Resource* pResource, uint32 subresource, D3D12_RESOURCE_STATES toState);
		void FlushBarriers(ID3D12GraphicsCommandList* pCommandList);

		D3D12_RESOURCE_STATES GetState(ID3D12Resource* pResource, uint32 subresource) const;

	private:
		void SyncGlobalStates();
	
	private:
		ResourceMap m_ResourceStates;
		std::vector<D3D12_RESOURCE_BARRIER> m_DefferedBarriers;

	private:
		static DX12ResourceState& GetGlobalState(ID3D12Resource* pResource);
		static bool HasGlobalState(ID3D12Resource* pResource);
		static void AddGlobalState(ID3D12Resource* pResource, const DX12ResourceState& state);
		static void RemoveGlobalState(ID3D12Resource* pResource);

	private:
		static std::mutex s_GlobalMutex;
		static ResourceMap s_GlobalResourceStates;
	};
}
#endif