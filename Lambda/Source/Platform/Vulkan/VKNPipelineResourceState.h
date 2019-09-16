#pragma once
#include "Graphics/IPipelineResourceState.h"
#include <map>
#include <vulkan/vulkan.h>

namespace Lambda
{
	class VKNBuffer;
	class VKNTexture;
	class VKNSamplerState;

	//-------
	//VKNSlot
	//-------

	struct VKNSlot
	{
		ResourceSlot			Slot;
		union
		{
			VKNBuffer*			pBuffer;
			VKNTexture*		pTexture;
			VKNSamplerState* pSamplerState;
		};
		union 
		{
			VkDescriptorBufferInfo	BufferInfo;
			VkDescriptorImageInfo	ImageInfo;
		};
	};

	//------------------------
	//VKNPipelineResourceState
	//------------------------

	class VKNPipelineResourceState final : public IPipelineResourceState
	{
	public:
		LAMBDA_NO_COPY(VKNPipelineResourceState);

		VKNPipelineResourceState(const PipelineResourceStateDesc& desc);
		~VKNPipelineResourceState() = default;

		virtual void SetTextures(ITexture** ppTextures, uint32 numTextures, uint32 startSlot) override final;
		virtual void SetSamplerStates(ISamplerState** ppSamplerStates, uint32 numSamplerStates, uint32 startSlot) override final;
		virtual void SetConstantBuffers(IBuffer** ppBuffers, uint32 numBuffers, uint32 startSlot) override final;

		virtual void* GetNativeHandle() const override final;
		
		const uint32*		GetDynamicOffsets() const;
		uint32				GetDynamicOffsetCount() const;
		VkDescriptorSet		GetDescriptorSet() const;
		VkPipelineLayout	GetPipelineLayout() const;

		void CommitBindings();
		void Destroy(VkDevice device);

	private:
		void Init(const PipelineResourceStateDesc& desc);
		void AllocateDescriptorSet();

	private:
		VkPipelineLayout				    m_PipelineLayout;
		VkDescriptorSetLayout			    m_DescriptorSetLayout;
		VkDescriptorSet					    m_DescriptorSet;
		VkDescriptorPool				    m_DescriptorPool;
		std::map<uint32, VKNSlot>			m_ResourceBindings;
		std::vector<VkWriteDescriptorSet>   m_DescriptorWrites;
		std::vector<uint32>				    m_DynamicOffsets;
		std::vector<VKNBuffer*>				m_DynamicBuffers;
		bool							    m_IsDirty;
	};


	inline VkPipelineLayout VKNPipelineResourceState::GetPipelineLayout() const
	{
		return m_PipelineLayout;
	}


	inline VkDescriptorSet VKNPipelineResourceState::GetDescriptorSet() const
	{
		return m_DescriptorSet;
	}


	inline const uint32* VKNPipelineResourceState::GetDynamicOffsets() const
	{
		return m_DynamicOffsets.data();
	}


	inline uint32 VKNPipelineResourceState::GetDynamicOffsetCount() const
	{
		return uint32(m_DynamicOffsets.size());
	}
}

