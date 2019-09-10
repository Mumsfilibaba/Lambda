#pragma once
#include "Graphics/IResourceState.h"
#include <vulkan/vulkan.h>

namespace Lambda
{
	class VulkanBuffer;
	class VulkanTexture;
	class VulkanSamplerState;


	class VulkanPipelineResourceState final : public IPipelineResourceState
	{
	public:
		union VulkanResourceBinding
		{
			VkDescriptorBufferInfo BufferBinding;
			VkDescriptorImageInfo ImageBinding;
		};

	public:
		LAMBDA_NO_COPY(VulkanPipelineResourceState);

		VulkanPipelineResourceState(VkDevice device, const PipelineResourceStateDesc& desc);
		~VulkanPipelineResourceState() = default;

		virtual void SetTextures(ITexture** ppTextures, uint32 numTextures, uint32 startSlot) override final;
		virtual void SetSamplerStates(ISamplerState** ppSamplerStates, uint32 numSamplerStates, uint32 startSlot) override final;
		virtual void SetConstantBuffers(IBuffer** ppBuffers, uint32 numBuffers, uint32 startSlot) override final;

		virtual void* GetNativeHandle() const override final;
		
		VkPipelineLayout GetPipelineLayout() const;
		VkDescriptorSet GetDescriptorSet() const;

		void CommitBindings(VkDevice device);
		void Destroy(VkDevice device);

	private:
		void Init(VkDevice device, const PipelineResourceStateDesc& desc);

	private:
		VkPipelineLayout m_PipelineLayout;
		VkDescriptorSetLayout m_DescriptorSetLayout;
		VkDescriptorSet m_DescriptorSet;
		VkDescriptorPool m_DescriptorPool;
		std::vector<ResourceSlot> m_ResourceSlots;
		std::vector<VkWriteDescriptorSet> m_DescriptorWrites;
		std::vector<VkDescriptorBufferInfo> m_BufferBindings;
		std::vector<VkDescriptorImageInfo> m_ImageBindings;
		std::vector<void*> m_CurrentBindings;
	};
}

