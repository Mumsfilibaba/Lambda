#include "LambdaPch.h"
#include "VulkanPipelineResourceState.h"
#include "VulkanBuffer.h"
#include "VulkanTexture.h"
#include "VulkanSamplerState.h"

namespace Lambda
{
	//---------------------------
	//VulkanPipelineResourceState
	//---------------------------

	VulkanPipelineResourceState::VulkanPipelineResourceState(VkDevice device, const PipelineResourceStateDesc& desc)
		: m_PipelineLayout(VK_NULL_HANDLE),
		m_DescriptorSetLayout(VK_NULL_HANDLE),
		m_DescriptorPool(VK_NULL_HANDLE),
		m_DescriptorSet(VK_NULL_HANDLE),
		m_ResourceSlots(),
		m_DescriptorWrites(),
		m_BufferBindings(),
		m_ImageBindings(),
		m_CurrentBindings()
	{
		LAMBDA_ASSERT(device != VK_NULL_HANDLE);

		m_ImageBindings.reserve(32);
		m_BufferBindings.reserve(32);

		Init(device, desc);
	}


	void VulkanPipelineResourceState::SetTextures(ITexture** ppTextures, uint32 numTextures, uint32 startSlot)
	{
		VkWriteDescriptorSet writeInfo = {};
		writeInfo.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		writeInfo.pNext = nullptr;
		writeInfo.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
		writeInfo.dstArrayElement = 0;
		writeInfo.dstSet = m_DescriptorSet;
		writeInfo.pBufferInfo = nullptr;
		writeInfo.pTexelBufferView = nullptr;

		uint32 writeSlot = startSlot;
		uint32 numDescriptors = 0;
		uint32 numImageBindings = uint32(m_ImageBindings.size());
		ShaderStage writingStage = m_ResourceSlots[startSlot].Stage;
		for (uint32 i = 0; i < numTextures; i++)
		{
			//If the texture is already bound then do not bind again, flush and move on
			if (m_CurrentBindings[startSlot + i] == ppTextures[i])
			{
				if (numDescriptors > 0)
				{
					writeInfo.descriptorCount = numDescriptors;
					writeInfo.dstBinding = writeSlot;
					writeInfo.pImageInfo = m_ImageBindings.data() + numImageBindings;
					m_DescriptorWrites.emplace_back(writeInfo);

					//Increase the next slot to write to
					writeSlot += numDescriptors;
					numDescriptors = 0;
					numImageBindings = uint32(m_ImageBindings.size());
				}

				continue;
			}

			//If the texture is not supposed to be in the same stage as the others, flush
			if (m_ResourceSlots[startSlot + i].Stage != writingStage)
			{
				if (numDescriptors > 0)
				{
					writeInfo.descriptorCount = numDescriptors;
					writeInfo.dstBinding = writeSlot;
					writeInfo.pImageInfo = m_ImageBindings.data() + numImageBindings;
					m_DescriptorWrites.emplace_back(writeInfo);

					//Increase the next slot to write to
					writeSlot += numDescriptors;
					numDescriptors = 0;

					//Update offset
					numImageBindings = uint32(m_ImageBindings.size());
				}
			}

			//Add this texture to the bindings
			VkDescriptorImageInfo imageInfo = {};
			imageInfo.imageView = reinterpret_cast<VulkanTexture*>(ppTextures[i])->GetImageView();
			imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			imageInfo.sampler = VK_NULL_HANDLE;
			m_ImageBindings.emplace_back(imageInfo);

			numDescriptors++;
			m_CurrentBindings[startSlot + i] = ppTextures[i];
		}


		//Before returning, flush
		if (numDescriptors > 0)
		{
			writeInfo.descriptorCount = numDescriptors;
			writeInfo.dstBinding = writeSlot;
			writeInfo.pImageInfo = m_ImageBindings.data() + numImageBindings;
			m_DescriptorWrites.emplace_back(writeInfo);
		}
	}


	void VulkanPipelineResourceState::SetSamplerStates(ISamplerState** ppSamplerStates, uint32 numSamplerStates, uint32 startSlot)
	{
		VkWriteDescriptorSet writeInfo = {};
		writeInfo.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		writeInfo.pNext = nullptr;
		writeInfo.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER;
		writeInfo.dstArrayElement = 0;
		writeInfo.dstSet = m_DescriptorSet;
		writeInfo.pBufferInfo = nullptr;
		writeInfo.pTexelBufferView = nullptr;

		uint32 writeSlot = startSlot;
		uint32 numDescriptors = 0;
		uint32 numImageBindings = uint32(m_ImageBindings.size());
		ShaderStage writingStage = m_ResourceSlots[startSlot].Stage;
		for (uint32 i = 0; i < numSamplerStates; i++)
		{
			//If the samplerstate is already bound then do not bind again, flush and move on
			if (m_CurrentBindings[startSlot + i] == ppSamplerStates[i])
			{
				if (numDescriptors > 0)
				{
					writeInfo.descriptorCount = numDescriptors;
					writeInfo.dstBinding = writeSlot;
					writeInfo.pImageInfo = m_ImageBindings.data() + numImageBindings;
					m_DescriptorWrites.emplace_back(writeInfo);

					//Increase the next slot to write to
					writeSlot += numDescriptors;
					numDescriptors = 0;
					numImageBindings = uint32(m_ImageBindings.size());
				}

				continue;
			}

			//If the samplerstate is not supposed to be in the same stage as the others, flush
			if (m_ResourceSlots[startSlot + i].Stage != writingStage)
			{
				if (numDescriptors > 0)
				{
					writeInfo.descriptorCount = numDescriptors;
					writeInfo.dstBinding = writeSlot;
					writeInfo.pImageInfo = m_ImageBindings.data() + numImageBindings;
					m_DescriptorWrites.emplace_back(writeInfo);

					//Increase the next slot to write to
					writeSlot += numDescriptors;
					numDescriptors = 0;

					//Update offset
					numImageBindings = uint32(m_ImageBindings.size());
				}
			}

			//Add this samplerstate to the bindings
			VkDescriptorImageInfo imageInfo = {};
			imageInfo.imageView = VK_NULL_HANDLE;
			imageInfo.imageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			imageInfo.sampler = reinterpret_cast<VkSampler>(ppSamplerStates[i]->GetNativeHandle());
			m_ImageBindings.emplace_back(imageInfo);

			numDescriptors++;
			m_CurrentBindings[startSlot + i] = ppSamplerStates[i];
		}


		//Before returning, flush
		if (numDescriptors > 0)
		{
			writeInfo.descriptorCount = numDescriptors;
			writeInfo.dstBinding = writeSlot;
			writeInfo.pImageInfo = m_ImageBindings.data() + numImageBindings;
			m_DescriptorWrites.emplace_back(writeInfo);
		}
	}


	void VulkanPipelineResourceState::SetConstantBuffers(IBuffer** ppBuffers, uint32 numBuffers, uint32 startSlot)
	{
		VkWriteDescriptorSet writeInfo = {};
		writeInfo.sType				= VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		writeInfo.pNext				= nullptr;
		writeInfo.descriptorType	= VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		writeInfo.dstArrayElement	= 0;
		writeInfo.dstSet			= m_DescriptorSet;
		writeInfo.pImageInfo		= nullptr;
		writeInfo.pTexelBufferView	= nullptr;

		uint32 writeSlot = startSlot;
		uint32 numDescriptors	= 0;
		uint32 numBufferBindings = uint32(m_BufferBindings.size());
		ShaderStage writingStage = m_ResourceSlots[startSlot].Stage;
		ResourceUsage usage = m_ResourceSlots[startSlot].Usage;
		for (uint32 i = 0; i < numBuffers; i++)
		{
			//If the buffer is already bound then do not bind again, flush and move on
			if (m_CurrentBindings[startSlot + i] == ppBuffers[i])
			{
				if (numDescriptors > 0)
				{
					writeInfo.descriptorCount = numDescriptors;
					writeInfo.dstBinding = writeSlot;
					writeInfo.pBufferInfo = m_BufferBindings.data() + numBufferBindings;
					m_DescriptorWrites.emplace_back(writeInfo);

					//Increase the next slot to write to
					writeSlot += numDescriptors;
					numDescriptors = 0;
					numBufferBindings = uint32(m_BufferBindings.size());
				}

				continue;
			}

			//If the buffer is not supposed to be in the same stage as the others or the usage is not the same, flush
			if (m_ResourceSlots[startSlot + i].Stage != writingStage || m_ResourceSlots[startSlot + i].Usage != usage)
			{
				if (numDescriptors > 0)
				{
					writeInfo.descriptorCount = numDescriptors;
					writeInfo.dstBinding = writeSlot;
					writeInfo.pBufferInfo = m_BufferBindings.data() + numBufferBindings;
					m_DescriptorWrites.emplace_back(writeInfo);

					//Increase the next slot to write to
					writeSlot += numDescriptors;
					numDescriptors = 0;

					//Update offset
					numBufferBindings = uint32(m_BufferBindings.size());
				}
			}

			//Add this buffer to the bindings
			BufferDesc bufferDesc = ppBuffers[i]->GetDesc();
			VkDescriptorBufferInfo bufferInfo = {};
			bufferInfo.buffer = reinterpret_cast<VkBuffer>(ppBuffers[i]->GetNativeHandle());
			bufferInfo.offset = 0;
			bufferInfo.range = bufferDesc.SizeInBytes;
			m_BufferBindings.emplace_back(bufferInfo);

			usage = m_ResourceSlots[startSlot + i].Usage;
			if (usage == RESOURCE_USAGE_DYNAMIC)
				writeInfo.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
			else
				writeInfo.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;

			numDescriptors++;
			m_CurrentBindings[startSlot + i] = ppBuffers[i];
		}


		//Before returning, flush
		if (numDescriptors > 0)
		{
			writeInfo.descriptorCount	= numDescriptors;
			writeInfo.dstBinding		= writeSlot;
			writeInfo.pBufferInfo		= m_BufferBindings.data() + numBufferBindings;
			m_DescriptorWrites.emplace_back(writeInfo);
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

	
	void VulkanPipelineResourceState::CommitBindings(VkDevice device)
	{
		if (m_DescriptorWrites.size() > 0)
		{
			//Write all descriptors
			vkUpdateDescriptorSets(device, uint32(m_DescriptorWrites.size()), m_DescriptorWrites.data(), 0, nullptr);

			//Clear bindings and writes
			m_BufferBindings.clear();
			m_ImageBindings.clear();
			m_DescriptorWrites.clear();
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


	void VulkanPipelineResourceState::Init(VkDevice device, const PipelineResourceStateDesc& desc)
	{
		//Number of resources that can be bound
		m_CurrentBindings.resize(desc.NumResourceSlots);

		//Copy the resourceslots
		m_ResourceSlots = std::vector<ResourceSlot>(desc.pResourceSlots, desc.pResourceSlots + desc.NumResourceSlots);
		
		//Number of each type
		uint32 uniformBufferCount			= 0;
		uint32 dynamicUniformBufferCount	= 0;
		uint32 samplerCount					= 0;
		uint32 sampledImageCount			= 0;

		//Create descriptor bindings for each resourceslot
		std::vector<VkDescriptorSetLayoutBinding> layoutBindings;
		for (uint32 i = 0; i < desc.NumResourceSlots; i++)
		{
			//Set current resources to nullptr
			m_CurrentBindings[i] = nullptr;

			VkDescriptorSetLayoutBinding layoutBinding = {};
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
			else if (desc.pResourceSlots[i].Type == RESOURCE_TYPE_SAMPLER)
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

			layoutBinding.descriptorCount		= 1;
			layoutBinding.binding				= desc.pResourceSlots[i].Slot;
			layoutBinding.pImmutableSamplers	= nullptr;
			layoutBindings.push_back(layoutBinding);
		}

		//Create descriptorsetlayout
		VkDescriptorSetLayoutCreateInfo descriptorLayoutInfo = {};
		descriptorLayoutInfo.sType			= VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		descriptorLayoutInfo.pNext			= nullptr;
		descriptorLayoutInfo.flags			= 0;
		descriptorLayoutInfo.bindingCount	= uint32(layoutBindings.size());
		descriptorLayoutInfo.pBindings		= layoutBindings.data();

		//Create layout for shaderstage
		if (vkCreateDescriptorSetLayout(device, &descriptorLayoutInfo, nullptr, &m_DescriptorSetLayout) != VK_SUCCESS)
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

		if (vkCreatePipelineLayout(device, &layoutInfo, nullptr, &m_PipelineLayout) != VK_SUCCESS)
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
		poolSizes[0].type				= VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		poolSizes[0].descriptorCount	= uniformBufferCount * maxSets;
		poolSizes[1].type				= VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
		poolSizes[1].descriptorCount	= uniformBufferCount * maxSets;
		poolSizes[2].type				= VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
		poolSizes[2].descriptorCount	= sampledImageCount * maxSets;
		poolSizes[3].type				= VK_DESCRIPTOR_TYPE_SAMPLER;
		poolSizes[3].descriptorCount	= samplerCount * maxSets;


		//Create descriptorpool
		VkDescriptorPoolCreateInfo descriptorPoolInfo = {};
		descriptorPoolInfo.sType			= VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		descriptorPoolInfo.flags			= 0;
		descriptorPoolInfo.pNext			= nullptr;
		descriptorPoolInfo.poolSizeCount	= poolCount;
		descriptorPoolInfo.pPoolSizes		= poolSizes;
		descriptorPoolInfo.maxSets			= maxSets;

		if (vkCreateDescriptorPool(device, &descriptorPoolInfo, nullptr, &m_DescriptorPool) != VK_SUCCESS)
		{
			LOG_DEBUG_ERROR("Vulkan: Failed to create DescriptorPool\n");
			return;
		}
		else
		{
			LOG_DEBUG_INFO("Vulkan: Created DescriptorPool\n");
		}

		//Allocate descriptorsets
		VkDescriptorSetAllocateInfo descriptorAllocInfo = {};
		descriptorAllocInfo.sType				= VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		descriptorAllocInfo.pNext				= nullptr;
		descriptorAllocInfo.descriptorPool		= m_DescriptorPool;
		descriptorAllocInfo.descriptorSetCount	= 1;
		descriptorAllocInfo.pSetLayouts			= &m_DescriptorSetLayout;

		if (vkAllocateDescriptorSets(device, &descriptorAllocInfo, &m_DescriptorSet))
		{
			LOG_DEBUG_ERROR("Vulkan: Failed to allocate DescriptorSets\n");
		}
		else
		{
			LOG_DEBUG_INFO("Vulkan: Allocated DescriptorSets\n");
		}
	}
}