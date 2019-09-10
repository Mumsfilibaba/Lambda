#include "LambdaPch.h"
#include "DX12ResourceStateTracker.h"
#if defined(LAMBDA_PLAT_WINDOWS)

namespace Lambda
{
	std::mutex DX12ResourceStateTracker::s_GlobalMutex = std::mutex();
	DX12ResourceStateTracker::ResourceMap DX12ResourceStateTracker::s_GlobalResourceStates;

	DX12ResourceStateTracker::DX12ResourceStateTracker()
		: m_ResourceStates(),
		m_DefferedBarriers()
	{
	}


	void DX12ResourceStateTracker::TransitionResource(ID3D12Resource* pResource, uint32 subresource, D3D12_RESOURCE_STATES toState)
	{
		LAMBDA_ASSERT(pResource != nullptr);

		D3D12_RESOURCE_BARRIER barrier = {};
		barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		barrier.Transition.pResource = pResource;
		barrier.Transition.StateAfter = toState;
		barrier.Transition.Subresource = subresource;

		auto& state = m_ResourceStates[pResource];
		if (subresource == D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES)
		{
			if (state.SubResourceStates.empty())
			{
				//Avoid unnecceassry barriers
				if (state.State != toState)
				{
					barrier.Transition.StateBefore = state.State;
					state.State = toState;
					m_DefferedBarriers.push_back(barrier);
				}
			}
			else
			{
				//Set all subresource states to the correct one
				for (uint32 i = 0; i < state.SubResourceStates.size(); i++)
				{
					//If state is not correct add a barrier
					if (state.SubResourceStates[i] != toState)
					{
						barrier.Transition.StateBefore = state.SubResourceStates[i];
						barrier.Transition.Subresource = i;
						m_DefferedBarriers.push_back(barrier);
					}
				}

				//Set state for all resources since all have the same state
				state.State = toState;
				state.SubResourceStates.clear();
			}
		}
		else
		{
			if (state.SubResourceStates.empty() || state.SubResourceStates.size() <= subresource)
			{
				//Allocate and set subresource states
				uint32 i = (uint32)state.SubResourceStates.size();

				state.SubResourceStates.resize(subresource + 1);
				for (; i < state.SubResourceStates.size(); i++)
					state.SubResourceStates[i] = state.State;
			}

			//Avoid unnecceassry barriers
			if (state.SubResourceStates[subresource] != toState)
			{
				barrier.Transition.StateBefore = state.SubResourceStates[subresource];
				m_DefferedBarriers.push_back(barrier);
			}
		}
	}
	

	void DX12ResourceStateTracker::FlushBarriers(ID3D12GraphicsCommandList* pCommandList)
	{
		LAMBDA_ASSERT(pCommandList != nullptr);
		std::lock_guard<std::mutex> lock(s_GlobalMutex);

		//Make sure the states are still valid
		for (int32 i = 0; i < m_DefferedBarriers.size(); i++)
		{
			//Only check transitions
			auto& barrier = m_DefferedBarriers[i];
			if (barrier.Type == D3D12_RESOURCE_BARRIER_TYPE_TRANSITION)
			{
				//Check with global state
				auto globalState = s_GlobalResourceStates.find(barrier.Transition.pResource);
				if (globalState != s_GlobalResourceStates.end())
				{
					if (barrier.Transition.Subresource == D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES)
					{
						if (globalState->second.State != barrier.Transition.StateBefore)
							barrier.Transition.StateBefore = globalState->second.State;
					}
					else
					{
						if (globalState->second.SubResourceStates[barrier.Transition.Subresource] != barrier.Transition.StateBefore)
							barrier.Transition.StateBefore = globalState->second.State;
					}

					if (barrier.Transition.StateAfter == barrier.Transition.StateBefore)
					{
						barrier = m_DefferedBarriers.back();
						m_DefferedBarriers.pop_back();
					}
				}
			}
		}
		
		//Dont do if no barriers
		if (!m_DefferedBarriers.empty())
		{
			pCommandList->ResourceBarrier((uint32)m_DefferedBarriers.size(), m_DefferedBarriers.data());
			m_DefferedBarriers.clear();
			SyncGlobalStates();
		}
	}


	void DX12ResourceStateTracker::SyncGlobalStates()
	{
		for (auto& iter : m_ResourceStates)
		{
			auto& globalState = s_GlobalResourceStates[iter.first];
			globalState = iter.second;
		}
	}


	D3D12_RESOURCE_STATES DX12ResourceStateTracker::GetState(ID3D12Resource* pResource, uint32 subresource) const
	{
		LAMBDA_ASSERT(pResource != nullptr);

		//Get state
		auto pair = m_ResourceStates.find(pResource);
		if (pair != m_ResourceStates.end())
		{
			auto& state = pair->second;
			//if there are no subresources
			if (state.SubResourceStates.empty())
				return state.State;
			else if (subresource < state.SubResourceStates.size())
				return state.SubResourceStates[subresource];
		}

		return D3D12_RESOURCE_STATES(-1);
	}


	DX12ResourceState& DX12ResourceStateTracker::GetGlobalState(ID3D12Resource* pResource)
	{
		LAMBDA_ASSERT(pResource != nullptr);

		//Return the state
		auto resourceState = s_GlobalResourceStates.find(pResource);
		return resourceState->second;
	}


	bool DX12ResourceStateTracker::HasGlobalState(ID3D12Resource* pResource)
	{
		LAMBDA_ASSERT(pResource != nullptr);

		//Does the resource have a state
		auto resourceState = s_GlobalResourceStates.find(pResource);
		return resourceState != s_GlobalResourceStates.end();
	}


	void DX12ResourceStateTracker::AddGlobalState(ID3D12Resource* pResource, const DX12ResourceState& state)
	{
		LAMBDA_ASSERT(pResource != nullptr);

		//Add a new globalstate
		auto resourceState = s_GlobalResourceStates.find(pResource);
		if (resourceState == s_GlobalResourceStates.end())
			resourceState->second = state;
	}


	void DX12ResourceStateTracker::RemoveGlobalState(ID3D12Resource* pResource)
	{
		LAMBDA_ASSERT(pResource != nullptr);

		std::lock_guard<std::mutex> lock(s_GlobalMutex);
		s_GlobalResourceStates.erase(pResource);
	}
}
#endif
