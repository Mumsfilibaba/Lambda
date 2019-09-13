#pragma once
#include "Graphics/IPipelineResourceState.h"
#include <unordered_map>
#include <vulkan/vulkan.h>

namespace Lambda
{
	class VulkanBuffer;
	class VulkanTexture;
	class VulkanSamplerState;

	//----------
	//VulkanSlot
	//----------

	struct VulkanSlot
	{
		ResourceSlot			Slot;
		union
		{
			VulkanBuffer*		pBuffer;
			VulkanTexture*		pTexture;
			VulkanSamplerState* pSamplerState;
		};
		union 
		{
			VkDescriptorBufferInfo	BufferInfo;
			VkDescriptorImageInfo	ImageInfo;
		};
	};

	//---------------------------
	//VulkanPipelineResourceState
	//---------------------------

	class VulkanPipelineResourceState final : public IPipelineResourceState
	{
	public:
		LAMBDA_NO_COPY(VulkanPipelineResourceState);

		VulkanPipelineResourceState(const PipelineResourceStateDesc& desc);
		~VulkanPipelineResourceState() = default;

		virtual void SetTextures(ITexture** ppTextures, uint32 numTextures, uint32 startSlot) override final;
		virtual void SetSamplerStates(ISamplerState** ppSamplerStates, uint32 numSamplerStates, uint32 startSlot) override final;
		virtual void SetConstantBuffers(IBuffer** ppBuffers, uint32 numBuffers, uint32 startSlot) override final;

		virtual void* GetNativeHandle() const override final;
		
		const uint32*		GetDynamicOffsets() const;
		uint32				GetDynamicOffsetCount() const;
		VkDescriptorSet		GetDescriptorSet() const;
		VkPipelineLayout	GetPipelineLayout() const;

		void CommitBindings(VkDevice device);
		void Destroy(VkDevice device);

	private:
		void Init(const PipelineResourceStateDesc& desc);
		void AllocateDescriptorSet();

	private:
		VkPipelineLayout						m_PipelineLayout;
		VkDescriptorSetLayout					m_DescriptorSetLayout;
		VkDescriptorSet							m_DescriptorSet;
		VkDescriptorPool						m_DescriptorPool;
		std::unordered_map<uint32, VulkanSlot>	m_ResourceBindings;
		std::vector<VkWriteDescriptorSet>		m_DescriptorWrites;
		std::vector<uint32>						m_DynamicOffsets;
		std::vector<VulkanBuffer*>				m_DynamicBuffers;
		bool									m_IsDirty;
	};
}

