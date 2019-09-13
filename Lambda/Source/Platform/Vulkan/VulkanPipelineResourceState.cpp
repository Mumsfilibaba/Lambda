#include "LambdaPch.h"
#include "VulkanPipelineResourceState.h"
#include "VulkanGraphicsDevice.h"
#include "VulkanBuffer.h"
#include "VulkanTexture.h"
#include "VulkanSamplerState.h"

namespace Lambda
{
	//---------------------------
	//VulkanPipelineResourceState
	//---------------------------

	VulkanPipelineResourceState::VulkanPipelineResourceState(const PipelineResourceStateDesc& desc)
		: m_PipelineLayout(VK_NULL_HANDLE),
		m_DescriptorSetLayout(VK_NULL_HANDLE),
		m_DescriptorPool(VK_NULL_HANDLE),
		m_DescriptorSet(VK_NULL_HANDLE),
		m_ResourceBindings(),
		m_DescriptorWrites(),
		m_DynamicOffsets(),
		m_DynamicBuffers(),
		m_IsDirty(false)
	{
		Init(desc);
	}


	void VulkanPipelineResourceState::Init(const PipelineResourceStateDesc& desc)
	{
		//Copy the resourceslots
		for (uint32 i = 0; i < desc.NumResourceSlots; i++)
		{
			auto& slot		= m_ResourceBindings[desc.pResourceSlots[i].Slot];
			slot.Slot		= desc.pResourceSlots[i];
			slot.pBuffer	= nullptr;
		}
		

		//Number of each type
		uint32 uniformBufferCount			= 0;
		uint32 dynamicUniformBufferCount	= 0;
		uint32 samplerCount					= 0;
		uint32 sampledImageCount			= 0;

		//Create descriptor bindings for each resourceslot
		std::vector<VkDescriptorSetLayoutBinding> layoutBindings;
		for (uint32 i = 0; i < desc.NumResourceSlots; i++)
		{
			VkDescriptorSetLayoutBinding layoutBinding	= {};
			layoutBinding.descriptorCount				= 1;
			layoutBinding.binding						= desc.pResourceSlots[i].Slot;
			layoutBinding.pImmutableSamplers			= nullptr;
			
			//Set type
			layoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_MAX_ENUM;
			if (desc.pResourceSlots[i].Type == RESOURCE_TYPE_CONSTANT_BUFFER)
			{
				if (desc.pResourceSlots[i].Usage == RESOURCE_USAGE_DYNAMIC)
				{
					layoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
					dynamicUniformBufferCount++;
				}
				else
				{
					layoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
					uniformBufferCount++;
				}
			}
			else if (desc.pResourceSlots[i].Type == RESOURCE_TYPE_TEXTURE)
			{
				layoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
				sampledImageCount++;
			}
			else if (desc.pResourceSlots[i].Type == RESOURCE_TYPE_SAMPLER_STATE)
			{
				layoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER;
				samplerCount++;
			}

			//Set stage
			layoutBinding.stageFlags = 0;
			if (desc.pResourceSlots[i].Stage == SHADER_STAGE_VERTEX)
				layoutBinding.stageFlags |= VK_SHADER_STAGE_VERTEX_BIT;
			else if (desc.pResourceSlots[i].Stage == SHADER_STAGE_HULL)
				layoutBinding.stageFlags |= VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
			else if (desc.pResourceSlots[i].Stage == SHADER_STAGE_DOMAIN)
				layoutBinding.stageFlags |= VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
			else if (desc.pResourceSlots[i].Stage == SHADER_STAGE_GEOMETRY)
				layoutBinding.stageFlags |= VK_SHADER_STAGE_GEOMETRY_BIT;
			else if (desc.pResourceSlots[i].Stage == SHADER_STAGE_PIXEL)
				layoutBinding.stageFlags |= VK_SHADER_STAGE_FRAGMENT_BIT;
			else if (desc.pResourceSlots[i].Stage == SHADER_STAGE_COMPUTE)
				layoutBinding.stageFlags |= VK_SHADER_STAGE_COMPUTE_BIT;

			layoutBindings.push_back(layoutBinding);
		}

		//Create descriptorsetlayout
		VkDescriptorSetLayoutCreateInfo descriptorLayoutInfo = {};
		descriptorLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		descriptorLayoutInfo.pNext = nullptr;
		descriptorLayoutInfo.flags = 0;
		descriptorLayoutInfo.bindingCount = uint32(layoutBindings.size());
		descriptorLayoutInfo.pBindings = layoutBindings.data();

		VulkanGraphicsDevice& device = VulkanGraphicsDevice::GetInstance();
		if (vkCreateDescriptorSetLayout(device.GetDevice(), &descriptorLayoutInfo, nullptr, &m_DescriptorSetLayout) != VK_SUCCESS)
		{
			LOG_DEBUG_ERROR("Vulkan: Failed to create DescriptorSetLayout\n");
			return;
		}
		else
		{
			LOG_DEBUG_INFO("Vulkan: Created DescriptorSetLayout\n");
		}


		//Create pipelinelayout
		VkPipelineLayoutCreateInfo layoutInfo = {};
		layoutInfo.sType					= VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		layoutInfo.flags					= 0;
		layoutInfo.pNext					= nullptr;
		layoutInfo.setLayoutCount			= 1;
		layoutInfo.pSetLayouts				= &m_DescriptorSetLayout;
		layoutInfo.pushConstantRangeCount	= 0;
		layoutInfo.pPushConstantRanges		= nullptr;

		if (vkCreatePipelineLayout(device.GetDevice(), &layoutInfo, nullptr, &m_PipelineLayout) != VK_SUCCESS)
		{
			LOG_DEBUG_ERROR("Vulkan: Failed to create default PipelineLayout\n");
			return;
		}
		else
		{
			LOG_DEBUG_INFO("Vulkan: Created default PipelineLayout\n");
		}


		//Describe how many descriptors we want to create
		constexpr uint32 maxSets = 64;
		constexpr uint32 poolCount = 4;
		VkDescriptorPoolSize poolSizes[poolCount] = {};
		poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		poolSizes[0].descriptorCount = uniformBufferCount * maxSets;
		poolSizes[1].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
		poolSizes[1].descriptorCount = uniformBufferCount * maxSets;
		poolSizes[2].type = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
		poolSizes[2].descriptorCount = sampledImageCount * maxSets;
		poolSizes[3].type = VK_DESCRIPTOR_TYPE_SAMPLER;
		poolSizes[3].descriptorCount = samplerCount * maxSets;


		//Create descriptorpool
		VkDescriptorPoolCreateInfo descriptorPoolInfo = {};
		descriptorPoolInfo.sType			= VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		descriptorPoolInfo.flags			= 0;
		descriptorPoolInfo.pNext			= nullptr;
		descriptorPoolInfo.poolSizeCount	= poolCount;
		descriptorPoolInfo.pPoolSizes		= poolSizes;
		descriptorPoolInfo.maxSets			= maxSets;

		if (vkCreateDescriptorPool(device.GetDevice(), &descriptorPoolInfo, nullptr, &m_DescriptorPool) != VK_SUCCESS)
		{
			LOG_DEBUG_ERROR("Vulkan: Failed to create DescriptorPool\n");
			return;
		}
		else
		{
			LOG_DEBUG_INFO("Vulkan: Created DescriptorPool\n");
		}
	}


	void VulkanPipelineResourceState::AllocateDescriptorSet()
	{
		//Allocate descriptorsets
		VkDescriptorSetAllocateInfo descriptorAllocInfo = {};
		descriptorAllocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		descriptorAllocInfo.pNext = nullptr;
		descriptorAllocInfo.descriptorPool = m_DescriptorPool;
		descriptorAllocInfo.descriptorSetCount = 1;
		descriptorAllocInfo.pSetLayouts = &m_DescriptorSetLayout;

		VulkanGraphicsDevice& device = VulkanGraphicsDevice::GetInstance();
		if (vkAllocateDescriptorSets(device.GetDevice(), &descriptorAllocInfo, &m_DescriptorSet))
		{
			LOG_DEBUG_ERROR("Vulkan: Failed to allocate DescriptorSets\n");
		}
		else
		{
			LOG_DEBUG_INFO("Vulkan: Allocated DescriptorSets\n");
		}
	}


	void VulkanPipelineResourceState::SetTextures(ITexture** ppTextures, uint32 numTextures, uint32 startSlot)
	{
		for (uint32 i = 0; i < numTextures; i++)
		{
			auto& resourceBinding = m_ResourceBindings[startSlot + i];
			if (resourceBinding.Slot.Type == RESOURCE_TYPE_TEXTURE)
			{
				if (resourceBinding.pTexture != ppTextures[i])
				{
					resourceBinding.pTexture				= reinterpret_cast<VulkanTexture*>(ppTextures[i]);
					resourceBinding.ImageInfo.imageView		= resourceBinding.pTexture->GetImageView();
					resourceBinding.ImageInfo.imageLayout	= VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
					resourceBinding.ImageInfo.sampler		= VK_NULL_HANDLE;
					m_IsDirty = true;
				}
			}
			else
			{
				LOG_DEBUG_ERROR("Vulkan: Slot at '%u' is not set to bind a Texture\n", startSlot + i);
			}
		}
	}


	void VulkanPipelineResourceState::SetSamplerStates(ISamplerState** ppSamplerStates, uint32 numSamplerStates, uint32 startSlot)
	{
		for (uint32 i = 0; i < numSamplerStates; i++)
		{
			auto& resourceBinding = m_ResourceBindings[startSlot + i];
			if (resourceBinding.Slot.Type == RESOURCE_TYPE_SAMPLER_STATE)
			{
				if (resourceBinding.pSamplerState != ppSamplerStates[i])
				{
					resourceBinding.pSamplerState			= reinterpret_cast<VulkanSamplerState*>(ppSamplerStates[i]);
					resourceBinding.ImageInfo.imageView		= VK_NULL_HANDLE;
					resourceBinding.ImageInfo.imageLayout	= VK_IMAGE_LAYOUT_UNDEFINED;
					resourceBinding.ImageInfo.sampler		= reinterpret_cast<VkSampler>(ppSamplerStates[i]->GetNativeHandle());
					m_IsDirty = true;
				}
			}
			else
			{
				LOG_DEBUG_ERROR("Vulkan: Slot at '%u' is not set to bind a SamplerState\n", startSlot + i);
			}
		}
	}


	void VulkanPipelineResourceState::SetConstantBuffers(IBuffer** ppBuffers, uint32 numBuffers, uint32 startSlot)
	{
		for (uint32 i = 0; i < numBuffers; i++)
		{
			auto& resourceBinding = m_ResourceBindings[startSlot + i];
			if (resourceBinding.Slot.Type == RESOURCE_TYPE_CONSTANT_BUFFER)
			{
				if (resourceBinding.pBuffer != ppBuffers[i])
				{
					resourceBinding.pBuffer				= reinterpret_cast<VulkanBuffer*>(ppBuffers[i]);
					
					BufferDesc bufferDesc				= resourceBinding.pBuffer->GetDesc();
					resourceBinding.BufferInfo.buffer	= reinterpret_cast<VkBuffer>(resourceBinding.pBuffer->GetNativeHandle());
					resourceBinding.BufferInfo.offset	= 0;
					resourceBinding.BufferInfo.range	= bufferDesc.SizeInBytes;
					m_IsDirty = true;
				}
			}
			else
			{
				LOG_DEBUG_ERROR("Vulkan: Slot at '%u' is not set to bind a ConstantBuffer\n", startSlot + i);
			}
		}
	}

	
	void* VulkanPipelineResourceState::GetNativeHandle() const
	{
		return reinterpret_cast<void*>(m_PipelineLayout);
	}

	
	VkPipelineLayout VulkanPipelineResourceState::GetPipelineLayout() const
	{
		return m_PipelineLayout;
	}


	VkDescriptorSet VulkanPipelineResourceState::GetDescriptorSet() const
	{
		return m_DescriptorSet;
	}


	const uint32* VulkanPipelineResourceState::GetDynamicOffsets() const
	{
		return m_DynamicOffsets.data();
	}


	uint32 VulkanPipelineResourceState::GetDynamicOffsetCount() const
	{
		return uint64(m_DynamicOffsets.size());
	}

	
	void VulkanPipelineResourceState::CommitBindings(VkDevice device)
	{
		//Update bufferoffsets
		for (auto pBuffer : m_DynamicBuffers)
		{
			if (pBuffer->IsDirty())
			{
				m_IsDirty = true;
				pBuffer->SetIsClean();
				break;
			}
		}


		//Update descriptorset
		if (m_IsDirty)
		{
			AllocateDescriptorSet();

			m_DynamicBuffers.clear();
			m_DynamicOffsets.clear();

			VkWriteDescriptorSet writeInfo = {};
			writeInfo.sType					= VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			writeInfo.pNext					= nullptr;
			writeInfo.dstArrayElement		= 0;
			writeInfo.dstSet				= m_DescriptorSet;
			writeInfo.pImageInfo			= nullptr;
			writeInfo.pTexelBufferView		= nullptr;

			//Setup writing slots
			for (auto& resourceBinding : m_ResourceBindings)
			{
				VulkanSlot& binding			= resourceBinding.second;
				writeInfo.descriptorCount	= 1;
				writeInfo.dstBinding		= binding.Slot.Slot;

				if (binding.Slot.Type == RESOURCE_TYPE_CONSTANT_BUFFER)
				{
					writeInfo.pBufferInfo = &binding.BufferInfo;
					if (binding.Slot.Usage == RESOURCE_USAGE_DYNAMIC)
					{
						BufferDesc bufferDesc = binding.pBuffer->GetDesc();
						binding.BufferInfo.buffer	= reinterpret_cast<VkBuffer>(binding.pBuffer->GetNativeHandle());
						binding.BufferInfo.offset	= 0;
						binding.BufferInfo.range	= bufferDesc.SizeInBytes;

						writeInfo.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
						m_DynamicBuffers.push_back(binding.pBuffer);
					}
					else
					{
						writeInfo.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
					}
				}
				else if (binding.Slot.Type == RESOURCE_TYPE_TEXTURE)
				{
					writeInfo.descriptorType	= VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
					writeInfo.pImageInfo		= &binding.ImageInfo;
				}
				else if (binding.Slot.Type == RESOURCE_TYPE_SAMPLER_STATE)
				{
					writeInfo.descriptorType	= VK_DESCRIPTOR_TYPE_SAMPLER;
					writeInfo.pImageInfo		= &binding.ImageInfo;
				}

				m_DescriptorWrites.emplace_back(writeInfo);
			}

			//Write all descriptors
			if (m_DescriptorWrites.size() > 0)
			{
				vkUpdateDescriptorSets(device, uint32(m_DescriptorWrites.size()), m_DescriptorWrites.data(), 0, nullptr);
				m_DescriptorWrites.clear();
			}

			m_DynamicOffsets.resize(m_DynamicBuffers.size());
			m_IsDirty = false;
		}


		//Update bufferoffsets
		for (size_t i = 0; i < m_DynamicBuffers.size(); i++)
		{
			m_DynamicOffsets[i] = m_DynamicBuffers[i]->GetDynamicOffset();
		}
	}


	void VulkanPipelineResourceState::Destroy(VkDevice device)
	{
		if (m_DescriptorPool != VK_NULL_HANDLE)
		{
			vkDestroyDescriptorPool(device, m_DescriptorPool, nullptr);
			m_DescriptorPool = VK_NULL_HANDLE;
		}
		if (m_DescriptorSetLayout != VK_NULL_HANDLE)
		{
			vkDestroyDescriptorSetLayout(device, m_DescriptorSetLayout, nullptr);
			m_DescriptorSetLayout = VK_NULL_HANDLE;
		}
		if (m_PipelineLayout != VK_NULL_HANDLE)
		{
			vkDestroyPipelineLayout(device, m_PipelineLayout, nullptr);
			m_PipelineLayout = VK_NULL_HANDLE;
		}

		delete this;
	}
}