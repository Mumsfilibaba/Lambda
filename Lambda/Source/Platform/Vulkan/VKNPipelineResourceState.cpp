#include "LambdaPch.h"
#include "VKNPipelineResourceState.h"
#include "VKNDevice.h"
#include "VKNAllocator.h"
#include "VKNBuffer.h"
#include "VKNTexture.h"
#include "VKNSamplerState.h"

namespace Lambda
{
	//------------------------
	//VKNPipelineResourceState
	//------------------------

	VKNPipelineResourceState::VKNPipelineResourceState(const PipelineResourceStateDesc& desc)
		: m_pAllocator(nullptr),
		m_PipelineLayout(VK_NULL_HANDLE),
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


	void VKNPipelineResourceState::Init(const PipelineResourceStateDesc& desc)
	{
		//Copy the resourceslots
		for (uint32 i = 0; i < desc.NumResourceSlots; i++)
		{
			auto& slot = m_ResourceBindings[desc.pResourceSlots[i].Slot];
			slot.Slot				= desc.pResourceSlots[i];
			slot.ImageInfo.sampler	= VK_NULL_HANDLE;
			slot.pBuffer			= nullptr;
		}
		
		//Number of each type
		uint32 uniformBufferCount			= 0;
		uint32 dynamicUniformBufferCount	= 0;
		uint32 samplerCount					= 0;
		uint32 sampledImageCount			= 0;
		uint32 combinedImageSamplerCount	= 0;

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
				if (desc.pResourceSlots[i].pSamplerState == nullptr)
				{
					layoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
					sampledImageCount++;
				}
				else
				{
					layoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
					combinedImageSamplerCount++;

					//Bind sampler to slot permanently
					VKNSamplerState* pSampler = reinterpret_cast<VKNSamplerState*>(desc.pResourceSlots[i].pSamplerState);
					auto& slot = m_ResourceBindings[desc.pResourceSlots[i].Slot];
					slot.ImageInfo.sampler = reinterpret_cast<VkSampler>(pSampler->GetNativeHandle());

					layoutBinding.pImmutableSamplers = &slot.ImageInfo.sampler;
				}
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
		descriptorLayoutInfo.sType          = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		descriptorLayoutInfo.pNext          = nullptr;
		descriptorLayoutInfo.flags          = 0;
		descriptorLayoutInfo.bindingCount   = uint32(layoutBindings.size());
		descriptorLayoutInfo.pBindings      = layoutBindings.data();

		VKNDevice& device = VKNDevice::Get();
		if (vkCreateDescriptorSetLayout(device.GetDevice(), &descriptorLayoutInfo, nullptr, &m_DescriptorSetLayout) != VK_SUCCESS)
		{
			LOG_DEBUG_ERROR("Vulkan: Failed to create DescriptorSetLayout\n");
			return;
		}
		else
		{
			LOG_DEBUG_INFO("Vulkan: Created DescriptorSetLayout\n");

			//Set to dirty when we create so that we will always allocate a descriptorset
			m_IsDirty = true;
		}

		//Create constantranges
		std::vector<VkPushConstantRange> constantRanges;
		for (uint32 i = 0; i < desc.NumConstants; i++)
		{
			VkPushConstantRange range = {};
			range.size			= desc.pConstantSlots[i].SizeInBytes;
			range.offset		= 0;
			range.stageFlags	= 0;
			if (desc.pConstantSlots[i].Stage == SHADER_STAGE_VERTEX)
				range.stageFlags |= VK_SHADER_STAGE_VERTEX_BIT;
			else if (desc.pConstantSlots[i].Stage == SHADER_STAGE_HULL)
				range.stageFlags |= VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
			else if (desc.pConstantSlots[i].Stage == SHADER_STAGE_DOMAIN)
				range.stageFlags |= VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
			else if (desc.pConstantSlots[i].Stage == SHADER_STAGE_GEOMETRY)
				range.stageFlags |= VK_SHADER_STAGE_GEOMETRY_BIT;
			else if (desc.pConstantSlots[i].Stage == SHADER_STAGE_PIXEL)
				range.stageFlags |= VK_SHADER_STAGE_FRAGMENT_BIT;
			else if (desc.pConstantSlots[i].Stage == SHADER_STAGE_COMPUTE)
				range.stageFlags |= VK_SHADER_STAGE_COMPUTE_BIT;

			constantRanges.push_back(range);
		}


		//Create pipelinelayout
		VkPipelineLayoutCreateInfo layoutInfo = {};
		layoutInfo.sType					= VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		layoutInfo.flags					= 0;
		layoutInfo.pNext					= nullptr;
		layoutInfo.setLayoutCount			= 1;
		layoutInfo.pSetLayouts				= &m_DescriptorSetLayout;
		layoutInfo.pushConstantRangeCount	= uint32(constantRanges.size());
		layoutInfo.pPushConstantRanges		= constantRanges.data();

		if (vkCreatePipelineLayout(device.GetDevice(), &layoutInfo, nullptr, &m_PipelineLayout) != VK_SUCCESS)
		{
			LOG_DEBUG_ERROR("Vulkan: Failed to create PipelineLayout\n");
			return;
		}
		else
		{
			LOG_DEBUG_INFO("Vulkan: Created PipelineLayout\n");

			m_pAllocator = DBG_NEW VKNDescriptorSetAllocator(uniformBufferCount, dynamicUniformBufferCount, samplerCount, sampledImageCount, combinedImageSamplerCount, 8);
		}
	}


	void VKNPipelineResourceState::SetTextures(ITexture** ppTextures, uint32 numTextures, uint32 startSlot)
	{
		for (uint32 i = 0; i < numTextures; i++)
		{
			auto& resourceBinding = m_ResourceBindings[startSlot + i];
			if (resourceBinding.Slot.Type == RESOURCE_TYPE_TEXTURE)
			{
				if (resourceBinding.pTexture != ppTextures[i])
				{
					resourceBinding.pTexture				= reinterpret_cast<VKNTexture*>(ppTextures[i]);
					resourceBinding.ImageInfo.imageView		= resourceBinding.pTexture->GetImageView();
					resourceBinding.ImageInfo.imageLayout	= VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
					if (resourceBinding.Slot.pSamplerState == nullptr)
					{
						resourceBinding.ImageInfo.sampler		= VK_NULL_HANDLE;
					}
					m_IsDirty = true;
				}
			}
			else
			{
				LOG_DEBUG_ERROR("Vulkan: Slot at '%u' is not set to bind a Texture\n", startSlot + i);
			}
		}
	}


	void VKNPipelineResourceState::SetSamplerStates(ISamplerState** ppSamplerStates, uint32 numSamplerStates, uint32 startSlot)
	{
		for (uint32 i = 0; i < numSamplerStates; i++)
		{
			auto& resourceBinding = m_ResourceBindings[startSlot + i];
			if (resourceBinding.Slot.Type == RESOURCE_TYPE_SAMPLER_STATE)
			{
				if (resourceBinding.pSamplerState != ppSamplerStates[i])
				{
					resourceBinding.pSamplerState			= reinterpret_cast<VKNSamplerState*>(ppSamplerStates[i]);
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


	void VKNPipelineResourceState::SetConstantBuffers(IBuffer** ppBuffers, uint32 numBuffers, uint32 startSlot)
	{
		for (uint32 i = 0; i < numBuffers; i++)
		{
			auto& resourceBinding = m_ResourceBindings[startSlot + i];
			if (resourceBinding.Slot.Type == RESOURCE_TYPE_CONSTANT_BUFFER)
			{
				if (resourceBinding.pBuffer != ppBuffers[i])
				{
					resourceBinding.pBuffer				= reinterpret_cast<VKNBuffer*>(ppBuffers[i]);
					
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

	
	void* VKNPipelineResourceState::GetNativeHandle() const
	{
		return reinterpret_cast<void*>(m_PipelineLayout);
	}


	void VKNPipelineResourceState::CommitBindings()
	{
		//Update bufferoffsets
		for (auto pBuffer : m_DynamicBuffers)
		{
			if (pBuffer->IsDirty())
			{
				m_IsDirty = true;
                break;
			}
		}

		//Update descriptorset
		if (m_IsDirty)
		{
			//Allocate descriptorset
			m_DescriptorSet = m_pAllocator->Allocate(m_DescriptorSetLayout);

            //Clear dynamic buffers
			m_DynamicBuffers.clear();
			m_DynamicOffsets.clear();

            //Setup writinginfos
            VkWriteDescriptorSet writeInfo = {};
            writeInfo.sType                 = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            writeInfo.pNext                 = nullptr;
            writeInfo.dstArrayElement       = 0;
            writeInfo.dstSet                = m_DescriptorSet;
            writeInfo.pImageInfo            = nullptr;
            writeInfo.pTexelBufferView      = nullptr;

			for (auto& resourceBinding : m_ResourceBindings)
			{
				VKNSlot& binding			= resourceBinding.second;
				writeInfo.descriptorCount	= 1;
				writeInfo.dstBinding		= binding.Slot.Slot;

				//Write constantbuffer
				if (binding.Slot.Type == RESOURCE_TYPE_CONSTANT_BUFFER && binding.pBuffer != nullptr)
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
                        binding.pBuffer->SetIsClean();
					}
					else
					{
						writeInfo.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
					}

					//Add to descriptor writes
					m_DescriptorWrites.emplace_back(writeInfo);
				}
				//Bind texture
				else if (binding.Slot.Type == RESOURCE_TYPE_TEXTURE && binding.pTexture != nullptr)
				{
					writeInfo.pImageInfo = &binding.ImageInfo;
					if (binding.ImageInfo.sampler == VK_NULL_HANDLE)
					{
						writeInfo.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
					}
					else
					{
						writeInfo.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
					}

					//Add to descriptor writes
					m_DescriptorWrites.emplace_back(writeInfo);
				}
				//Bind samplerstate
				else if (binding.Slot.Type == RESOURCE_TYPE_SAMPLER_STATE && binding.pSamplerState != nullptr)
				{
					writeInfo.descriptorType	= VK_DESCRIPTOR_TYPE_SAMPLER;
					writeInfo.pImageInfo		= &binding.ImageInfo;
					
					//Add to descriptor writes
					m_DescriptorWrites.emplace_back(writeInfo);
				}
			}

			//Write all descriptors
			if (m_DescriptorWrites.size() > 0)
			{
				VKNDevice& device = VKNDevice::Get();
				vkUpdateDescriptorSets(device.GetDevice(), uint32(m_DescriptorWrites.size()), m_DescriptorWrites.data(), 0, nullptr);
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


	void VKNPipelineResourceState::Destroy(VkDevice device)
	{
		if (m_pAllocator)
		{
			m_pAllocator->Destroy(device);
			m_pAllocator = nullptr;
		}
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
