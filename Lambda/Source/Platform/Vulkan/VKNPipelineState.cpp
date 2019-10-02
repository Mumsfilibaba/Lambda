#include "LambdaPch.h"
#include "VKNPipelineState.h"
#include "VKNShader.h"
#include "VKNTexture.h"
#include "VKNBuffer.h"
#include "VKNSamplerState.h"
#include "VKNRenderPass.h"
#include "VKNSamplerState.h"
#include "VKNDevice.h"
#include "VKNUtilities.h"
#include "VKNConversions.inl"

namespace Lambda
{
	//----------------
	//VKNPipelineState
	//----------------

    VKNPipelineState::VKNPipelineState(VKNDevice* pDevice, const PipelineStateDesc& desc)
        : m_pDevice(pDevice),
		m_Pipeline(VK_NULL_HANDLE),
		m_IsDirty(true)
    {
		//Add a ref to the refcounter
		this->AddRef();

        Init(desc);
    }


	VKNPipelineState::~VKNPipelineState()
	{
		if (m_pAllocator)
		{
			m_pAllocator->Destroy(m_pDevice->GetVkDevice());
			m_pAllocator = nullptr;
		}
		if (m_DescriptorPool != VK_NULL_HANDLE)
		{
			vkDestroyDescriptorPool(m_pDevice->GetVkDevice(), m_DescriptorPool, nullptr);
			m_DescriptorPool = VK_NULL_HANDLE;
		}
		if (m_DescriptorSetLayout != VK_NULL_HANDLE)
		{
			vkDestroyDescriptorSetLayout(m_pDevice->GetVkDevice(), m_DescriptorSetLayout, nullptr);
			m_DescriptorSetLayout = VK_NULL_HANDLE;
		}
		if (m_PipelineLayout != VK_NULL_HANDLE)
		{
			vkDestroyPipelineLayout(m_pDevice->GetVkDevice(), m_PipelineLayout, nullptr);
			m_PipelineLayout = VK_NULL_HANDLE;
		}
		if (m_Pipeline != VK_NULL_HANDLE)
		{
			vkDestroyPipeline(m_pDevice->GetVkDevice(), m_Pipeline, nullptr);
			m_Pipeline = VK_NULL_HANDLE;
		}

		LOG_DEBUG_INFO("Vulkan: Destroyed PipelineState\n");
	}

    
    void VKNPipelineState::Init(const PipelineStateDesc& desc)
    {
		//Copy the resourceslots
		const ShaderVariableLayoutDesc& varibleLayout = desc.ShaderResources;
		for (uint32 i = 0; i < varibleLayout.NumResourceSlots; i++)
		{
			auto& variable	= m_ResourceBindings[varibleLayout.pResourceSlots[i].Slot];
			variable.Slot	= varibleLayout.pResourceSlots[i];
			variable.ImageInfo.sampler = VK_NULL_HANDLE;
			variable.pBuffer = nullptr;
		}

		//Number of each type
		uint32 uniformBufferCount = 0;
		uint32 dynamicUniformBufferCount = 0;
		uint32 samplerCount = 0;
		uint32 sampledImageCount = 0;
		uint32 combinedImageSamplerCount = 0;

		//Create descriptor bindings for each shadervariable
		std::vector<VkDescriptorSetLayoutBinding> layoutBindings;
		for (uint32 i = 0; i < varibleLayout.NumResourceSlots; i++)
		{
			const ShaderVariableDesc& variable = varibleLayout.pResourceSlots[i];

			VkDescriptorSetLayoutBinding layoutBinding = {};
			layoutBinding.descriptorCount = 1;
			layoutBinding.binding = variable.Slot;
			layoutBinding.pImmutableSamplers = nullptr;

			//Set type
			layoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_MAX_ENUM;
			if (varibleLayout.pResourceSlots[i].Type == RESOURCE_TYPE_CONSTANT_BUFFER)
			{
				if (varibleLayout.pResourceSlots[i].Usage == RESOURCE_USAGE_DYNAMIC)
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
			else if (variable.Type == RESOURCE_TYPE_TEXTURE)
			{
				if (variable.pSamplerState == nullptr)
				{
					layoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
					sampledImageCount++;
				}
				else
				{
					layoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
					combinedImageSamplerCount++;

					//Bind sampler to slot permanently
					VKNSamplerState* pSampler = reinterpret_cast<VKNSamplerState*>(variable.pSamplerState);
					auto& slot = m_ResourceBindings[variable.Slot];
					slot.ImageInfo.sampler = reinterpret_cast<VkSampler>(pSampler->GetNativeHandle());

					layoutBinding.pImmutableSamplers = &slot.ImageInfo.sampler;
				}
			}
			else if (variable.Type == RESOURCE_TYPE_SAMPLER_STATE)
			{
				layoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER;
				samplerCount++;
			}

			//Setup stages
			layoutBinding.stageFlags = 0;
			if (variable.Stage & SHADER_STAGE_VERTEX)
				layoutBinding.stageFlags |= VK_SHADER_STAGE_VERTEX_BIT;
			if (variable.Stage & SHADER_STAGE_HULL)
				layoutBinding.stageFlags |= VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
			if (variable.Stage & SHADER_STAGE_DOMAIN)
				layoutBinding.stageFlags |= VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
			if (variable.Stage & SHADER_STAGE_GEOMETRY)
				layoutBinding.stageFlags |= VK_SHADER_STAGE_GEOMETRY_BIT;
			if (variable.Stage & SHADER_STAGE_PIXEL)
				layoutBinding.stageFlags |= VK_SHADER_STAGE_FRAGMENT_BIT;
			if (variable.Stage & SHADER_STAGE_COMPUTE)
				layoutBinding.stageFlags |= VK_SHADER_STAGE_COMPUTE_BIT;

			layoutBindings.emplace_back(layoutBinding);
		}


		//Create descriptorsetlayout
		VkDescriptorSetLayoutCreateInfo descriptorLayoutInfo = {};
		descriptorLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		descriptorLayoutInfo.pNext = nullptr;
		descriptorLayoutInfo.flags = 0;
		descriptorLayoutInfo.bindingCount = uint32(layoutBindings.size());
		descriptorLayoutInfo.pBindings = layoutBindings.data();

		if (vkCreateDescriptorSetLayout(m_pDevice->GetVkDevice(), &descriptorLayoutInfo, nullptr, &m_DescriptorSetLayout) != VK_SUCCESS)
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
		for (uint32 i = 0; i < varibleLayout.NumConstantBlocks; i++)
		{
			const ConstantBlock& block = varibleLayout.pConstantBlocks[i];

			VkPushConstantRange range = {};
			range.size = block.SizeInBytes;
			range.offset = 0;
			range.stageFlags = 0;
			if (block.Stage == SHADER_STAGE_VERTEX)
				range.stageFlags |= VK_SHADER_STAGE_VERTEX_BIT;
			else if (block.Stage == SHADER_STAGE_HULL)
				range.stageFlags |= VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
			else if (block.Stage == SHADER_STAGE_DOMAIN)
				range.stageFlags |= VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
			else if (block.Stage == SHADER_STAGE_GEOMETRY)
				range.stageFlags |= VK_SHADER_STAGE_GEOMETRY_BIT;
			else if (block.Stage == SHADER_STAGE_PIXEL)
				range.stageFlags |= VK_SHADER_STAGE_FRAGMENT_BIT;
			else if (block.Stage == SHADER_STAGE_COMPUTE)
				range.stageFlags |= VK_SHADER_STAGE_COMPUTE_BIT;

			constantRanges.push_back(range);
		}


		//Create pipelinelayout
		VkPipelineLayoutCreateInfo layoutInfo = {};
		layoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		layoutInfo.flags = 0;
		layoutInfo.pNext = nullptr;
		layoutInfo.setLayoutCount = 1;
		layoutInfo.pSetLayouts = &m_DescriptorSetLayout;
		layoutInfo.pushConstantRangeCount = uint32(constantRanges.size());
		layoutInfo.pPushConstantRanges = constantRanges.data();

		if (vkCreatePipelineLayout(m_pDevice->GetVkDevice(), &layoutInfo, nullptr, &m_PipelineLayout) != VK_SUCCESS)
		{
			LOG_DEBUG_ERROR("Vulkan: Failed to create PipelineLayout\n");
			return;
		}
		else
		{
			LOG_DEBUG_INFO("Vulkan: Created PipelineLayout\n");
			m_pAllocator = DBG_NEW VKNDescriptorSetAllocator(uniformBufferCount, dynamicUniformBufferCount, samplerCount, sampledImageCount, combinedImageSamplerCount, 8);
		}


		//Create pipeline
		if (desc.Type == PIPELINE_TYPE_GRAPHICS)
		{
			//Describe shaderstages
			std::vector<VkPipelineShaderStageCreateInfo> shaderStages;
			
			//Common for all shaderstages
			VkPipelineShaderStageCreateInfo shaderStageInfo = {};
			shaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
			shaderStageInfo.pNext = nullptr;
			shaderStageInfo.flags = 0;
			shaderStageInfo.pSpecializationInfo = nullptr;
        
			//VertexShader
			if (desc.GraphicsPipeline.pVertexShader)
			{
				const ShaderDesc& vsDesc = desc.GraphicsPipeline.pVertexShader->GetDesc();
				shaderStageInfo.stage  = VK_SHADER_STAGE_VERTEX_BIT;
				shaderStageInfo.pName  = vsDesc.pEntryPoint;
				shaderStageInfo.module = reinterpret_cast<VkShaderModule>(desc.GraphicsPipeline.pVertexShader->GetNativeHandle());
				shaderStages.push_back(shaderStageInfo);
			}
			//HullShader
			if (desc.GraphicsPipeline.pHullShader)
			{
				const ShaderDesc& hsDesc = desc.GraphicsPipeline.pHullShader->GetDesc();
				shaderStageInfo.stage  = VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
				shaderStageInfo.pName  = hsDesc.pEntryPoint;
				shaderStageInfo.module = reinterpret_cast<VkShaderModule>(desc.GraphicsPipeline.pHullShader->GetNativeHandle());
				shaderStages.push_back(shaderStageInfo);
			}
			//DomainShader
			if (desc.GraphicsPipeline.pDomainShader)
			{
				const ShaderDesc& dsDesc = desc.GraphicsPipeline.pDomainShader->GetDesc();
				shaderStageInfo.stage  = VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
				shaderStageInfo.pName  = dsDesc.pEntryPoint;
				shaderStageInfo.module = reinterpret_cast<VkShaderModule>(desc.GraphicsPipeline.pDomainShader->GetNativeHandle());
				shaderStages.push_back(shaderStageInfo);
			}
			//GeometryShader
			if (desc.GraphicsPipeline.pGeometryShader)
			{
				const ShaderDesc& gsDesc = desc.GraphicsPipeline.pGeometryShader->GetDesc();
				shaderStageInfo.stage  = VK_SHADER_STAGE_GEOMETRY_BIT;
				shaderStageInfo.pName  = gsDesc.pEntryPoint;
				shaderStageInfo.module = reinterpret_cast<VkShaderModule>(desc.GraphicsPipeline.pGeometryShader->GetNativeHandle());
				shaderStages.push_back(shaderStageInfo);
			}
			//PixelShader
			if (desc.GraphicsPipeline.pPixelShader)
			{
				const ShaderDesc& psDesc = desc.GraphicsPipeline.pPixelShader->GetDesc();
				shaderStageInfo.stage  = VK_SHADER_STAGE_FRAGMENT_BIT;
				shaderStageInfo.pName  = psDesc.pEntryPoint;
				shaderStageInfo.module = reinterpret_cast<VkShaderModule>(desc.GraphicsPipeline.pPixelShader->GetNativeHandle());
				shaderStages.push_back(shaderStageInfo);
			}
        
			//InputLayout
			std::vector<VkVertexInputBindingDescription> bindingDescriptions;
			std::vector<VkVertexInputAttributeDescription> attributeDescriptions;

			const InputLayoutDesc& vertexInputDesc = desc.GraphicsPipeline.VertexInput;
			for (uint32 i = 0; i < vertexInputDesc.ElementCount; i++)
			{
				//Convert an input element to the corresponding AttributeDescription
				VkVertexInputAttributeDescription attributeDescription = {};
				attributeDescription.binding    = vertexInputDesc.pElements[i].BindingSlot;
				attributeDescription.location   = vertexInputDesc.pElements[i].InputSlot;
				attributeDescription.offset     = vertexInputDesc.pElements[i].StructureOffset;
				attributeDescription.format     = ConvertResourceFormat(vertexInputDesc.pElements[i].Format);
				attributeDescriptions.push_back(attributeDescription);
            
				//Check if binding is unique
				bool found = false;
				for (auto& bindDesc : bindingDescriptions)
				{
					if (bindDesc.binding == vertexInputDesc.pElements[i].BindingSlot)
					{
						found = true;
						break;
					}
				}
            
				//Add a new bindingdescription
				if (!found)
				{
					VkVertexInputBindingDescription bindingDescription = {};
					bindingDescription.binding      = vertexInputDesc.pElements[i].BindingSlot;
					bindingDescription.stride       = vertexInputDesc.pElements[i].Stride;
					bindingDescription.inputRate    = vertexInputDesc.pElements[i].IsInstanced ? VK_VERTEX_INPUT_RATE_INSTANCE : VK_VERTEX_INPUT_RATE_VERTEX;
					bindingDescriptions.push_back(bindingDescription);
				}
			}
        
			//Set inputlayout
			VkPipelineVertexInputStateCreateInfo inputLayout = {};
			inputLayout.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
			inputLayout.pNext = nullptr;
			inputLayout.flags = 0;
			inputLayout.vertexBindingDescriptionCount	= uint32(bindingDescriptions.size());
			inputLayout.pVertexBindingDescriptions		= bindingDescriptions.data();
			inputLayout.vertexAttributeDescriptionCount = uint32(attributeDescriptions.size());
			inputLayout.pVertexAttributeDescriptions	= attributeDescriptions.data();
        
			VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
			inputAssembly.sType						= VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
			inputAssembly.pNext						= nullptr;
			inputAssembly.flags						= 0;
			inputAssembly.topology					= ConvertPrimitiveTopology(desc.GraphicsPipeline.Topology);
			inputAssembly.primitiveRestartEnable	= VK_FALSE;
        
        
			//Setup dynamic states
			VkDynamicState dynamicStates[] =
			{
				VK_DYNAMIC_STATE_VIEWPORT,
				VK_DYNAMIC_STATE_SCISSOR
			};
        
			VkPipelineDynamicStateCreateInfo dynamicState = {};
			dynamicState.sType				= VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
			dynamicState.flags				= 0;
			dynamicState.pNext				= nullptr;
			dynamicState.pDynamicStates		= dynamicStates;
			dynamicState.dynamicStateCount	= 2;
        
        
			//Setup viewportstate
			VkPipelineViewportStateCreateInfo viewportState = {};
			viewportState.sType			= VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
			viewportState.flags			= 0;
			viewportState.pNext			= nullptr;
			viewportState.viewportCount	= 1;
			viewportState.pViewports	= nullptr;
			viewportState.scissorCount	= 1;
			viewportState.pScissors		= nullptr;
        
        
			//RasterizerState
			const RasterizerStateDesc& rsDesc = desc.GraphicsPipeline.RasterizerState;
			VkPipelineRasterizationStateCreateInfo rasterizerState = {};
			rasterizerState.sType                   = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
			rasterizerState.pNext                   = nullptr;
			rasterizerState.flags                   = 0;
			rasterizerState.depthClampEnable        = VK_FALSE;
			rasterizerState.rasterizerDiscardEnable = VK_FALSE;
			rasterizerState.polygonMode             = ConvertPolygonMode(rsDesc.FillMode);
			rasterizerState.lineWidth               = 1.0f;
			rasterizerState.frontFace               = rsDesc.FrontFaceCounterClockWise ? VK_FRONT_FACE_COUNTER_CLOCKWISE : VK_FRONT_FACE_CLOCKWISE;
			rasterizerState.depthBiasEnable         = VK_FALSE;
			rasterizerState.depthBiasConstantFactor = 0.0f;
			rasterizerState.depthBiasClamp          = 0.0f;
			rasterizerState.depthBiasSlopeFactor    = 0.0f;
        
			//Set cullmode
			if (rsDesc.Cull == CULL_MODE_BACK)
				rasterizerState.cullMode = VK_CULL_MODE_BACK_BIT;
			else if (rsDesc.Cull == CULL_MODE_FRONT)
				rasterizerState.cullMode = VK_CULL_MODE_FRONT_BIT;
			else if (rsDesc.Cull == CULL_MODE_NONE)
				rasterizerState.cullMode = VK_CULL_MODE_NONE;
        
        
			//Multisampling
			VkPipelineMultisampleStateCreateInfo multisampling = {};
			multisampling.sType                 = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
			multisampling.sampleShadingEnable   = VK_FALSE;
			multisampling.rasterizationSamples  = reinterpret_cast<VKNRenderPass*>(desc.GraphicsPipeline.pRenderPass)->GetSampleCount();
			multisampling.minSampleShading      = 1.0f;
			multisampling.pSampleMask           = nullptr;
			multisampling.alphaToCoverageEnable = VK_FALSE;
			multisampling.alphaToOneEnable      = VK_FALSE;
        
        
			//Blendstate
			const BlendStateDesc& blendDesc = desc.GraphicsPipeline.BlendState;
			VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
			colorBlendAttachment.colorWriteMask         = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
			colorBlendAttachment.blendEnable            = blendDesc.EnableBlending ? VK_TRUE : VK_FALSE;
			colorBlendAttachment.srcColorBlendFactor    = VK_BLEND_FACTOR_SRC_ALPHA;
			colorBlendAttachment.dstColorBlendFactor    = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
			colorBlendAttachment.colorBlendOp           = VK_BLEND_OP_ADD;
			colorBlendAttachment.srcAlphaBlendFactor    = VK_BLEND_FACTOR_ONE;
			colorBlendAttachment.dstAlphaBlendFactor    = VK_BLEND_FACTOR_ZERO;
			colorBlendAttachment.alphaBlendOp           = VK_BLEND_OP_ADD;
        
			VkPipelineColorBlendStateCreateInfo colorBlending = {};
			colorBlending.sType             = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
			colorBlending.logicOpEnable     = VK_FALSE;
			colorBlending.logicOp           = VK_LOGIC_OP_COPY;
			colorBlending.attachmentCount   = 1;
			colorBlending.pAttachments      = &colorBlendAttachment;
			colorBlending.blendConstants[0] = 0.0f;
			colorBlending.blendConstants[1] = 0.0f;
			colorBlending.blendConstants[2] = 0.0f;
			colorBlending.blendConstants[3] = 0.0f;
        
        
			//DepthStencilState
			const DepthStencilStateDesc& dsDesc = desc.GraphicsPipeline.DepthStencilState;
			VkPipelineDepthStencilStateCreateInfo depthStencilState = {};
			depthStencilState.sType                  = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
			depthStencilState.flags                  = 0;
			depthStencilState.pNext                  = nullptr;
			depthStencilState.depthTestEnable        = dsDesc.DepthTest ? VK_TRUE : VK_FALSE;
			depthStencilState.depthWriteEnable       = VK_TRUE;
			depthStencilState.depthCompareOp         = VK_COMPARE_OP_LESS;
			depthStencilState.depthBoundsTestEnable  = VK_FALSE;
			depthStencilState.minDepthBounds         = 0.0f;
			depthStencilState.maxDepthBounds         = 1.0f;
			depthStencilState.stencilTestEnable      = VK_FALSE;
			depthStencilState.front                  = {};
			depthStencilState.back                   = {};
        
        
			//Setup pipelinestate
			VkGraphicsPipelineCreateInfo pipelineInfo = {};
			pipelineInfo.sType                  = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
			pipelineInfo.flags                  = 0;
			pipelineInfo.pNext                  = nullptr;
			pipelineInfo.stageCount             = uint32(shaderStages.size());
			pipelineInfo.pStages                = shaderStages.data();
			pipelineInfo.pVertexInputState      = &inputLayout;
			pipelineInfo.pInputAssemblyState    = &inputAssembly;
			pipelineInfo.pViewportState         = &viewportState;
			pipelineInfo.pRasterizationState    = &rasterizerState;
			pipelineInfo.pMultisampleState      = &multisampling;
			pipelineInfo.pDepthStencilState     = &depthStencilState;
			pipelineInfo.pColorBlendState       = &colorBlending;
			pipelineInfo.pDynamicState          = &dynamicState;
			pipelineInfo.layout                 = m_PipelineLayout;
			pipelineInfo.renderPass             = reinterpret_cast<VkRenderPass>(desc.GraphicsPipeline.pRenderPass->GetNativeHandle());
			pipelineInfo.subpass                = 0;
			pipelineInfo.basePipelineHandle     = VK_NULL_HANDLE;
			pipelineInfo.basePipelineIndex      = -1;
			if (vkCreateGraphicsPipelines(m_pDevice->GetVkDevice(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_Pipeline) != VK_SUCCESS)
			{
				LOG_DEBUG_ERROR("Vulkan: Failed to create GraphicsPipelineState\n");
				return;
			}
			else
			{
				LOG_DEBUG_INFO("Vulkan: Created GraphicsPipelineState\n");
				SetName(desc.pName);
			}
		}
		else if (desc.Type == PIPELINE_TYPE_COMPUTE)
		{
			LOG_DEBUG_ERROR("Vulkan: Compute pipeline not implemented\n");
		}
		else
		{
			LOG_DEBUG_ERROR("Vulkan: Unknown PiplineType\n");
		}
    }


	void VKNPipelineState::SetTextures(ITexture** ppTextures, uint32 numTextures, uint32 startSlot)
	{
		for (uint32 i = 0; i < numTextures; i++)
		{
			if (ppTextures[i] == nullptr)
			{
				continue;
			}

			auto& resourceBinding = m_ResourceBindings[startSlot + i];
			if (resourceBinding.Slot.Type == RESOURCE_TYPE_TEXTURE)
			{
				if (resourceBinding.pTexture != ppTextures[i])
				{
					resourceBinding.pTexture = reinterpret_cast<VKNTexture*>(ppTextures[i]);
					resourceBinding.ImageInfo.imageView = resourceBinding.pTexture->GetVkImageView();
					resourceBinding.ImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
					if (resourceBinding.Slot.pSamplerState == nullptr)
					{
						resourceBinding.ImageInfo.sampler = VK_NULL_HANDLE;
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


	void VKNPipelineState::SetSamplerStates(ISamplerState** ppSamplerStates, uint32 numSamplerStates, uint32 startSlot)
	{
		for (uint32 i = 0; i < numSamplerStates; i++)
		{
			if (ppSamplerStates[i] == nullptr)
			{
				continue;
			}

			auto& resourceBinding = m_ResourceBindings[startSlot + i];
			if (resourceBinding.Slot.Type == RESOURCE_TYPE_SAMPLER_STATE)
			{
				if (resourceBinding.pSamplerState != ppSamplerStates[i])
				{
					resourceBinding.pSamplerState = reinterpret_cast<VKNSamplerState*>(ppSamplerStates[i]);
					resourceBinding.ImageInfo.imageView = VK_NULL_HANDLE;
					resourceBinding.ImageInfo.imageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
					resourceBinding.ImageInfo.sampler = reinterpret_cast<VkSampler>(ppSamplerStates[i]->GetNativeHandle());
					m_IsDirty = true;
				}
			}
			else
			{
				LOG_DEBUG_ERROR("Vulkan: Slot at '%u' is not set to bind a SamplerState\n", startSlot + i);
			}
		}
	}


	void VKNPipelineState::SetConstantBuffers(IBuffer** ppBuffers, uint32 numBuffers, uint32 startSlot)
	{
		for (uint32 i = 0; i < numBuffers; i++)
		{
			if (ppBuffers[i] == nullptr)
			{
				continue;
			}

			auto& resourceBinding = m_ResourceBindings[startSlot + i];
			if (resourceBinding.Slot.Type == RESOURCE_TYPE_CONSTANT_BUFFER)
			{
				if (resourceBinding.pBuffer != ppBuffers[i])
				{
					resourceBinding.pBuffer = reinterpret_cast<VKNBuffer*>(ppBuffers[i]);

					const BufferDesc& bufferDesc = resourceBinding.pBuffer->GetDesc();
					resourceBinding.BufferInfo.buffer = reinterpret_cast<VkBuffer>(resourceBinding.pBuffer->GetNativeHandle());
					resourceBinding.BufferInfo.offset = 0;
					resourceBinding.BufferInfo.range = bufferDesc.SizeInBytes;
					m_IsDirty = true;
				}
			}
			else
			{
				LOG_DEBUG_ERROR("Vulkan: Slot at '%u' is not set to bind a ConstantBuffer\n", startSlot + i);
			}
		}
	}


	void VKNPipelineState::CommitBindings()
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
			writeInfo.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			writeInfo.pNext = nullptr;
			writeInfo.dstArrayElement = 0;
			writeInfo.dstSet = m_DescriptorSet;
			writeInfo.pImageInfo = nullptr;
			writeInfo.pTexelBufferView = nullptr;

			for (auto& resourceBinding : m_ResourceBindings)
			{
				VKNSlot& binding = resourceBinding.second;
				writeInfo.descriptorCount = 1;
				writeInfo.dstBinding = binding.Slot.Slot;

				//Write constantbuffer
				if (binding.Slot.Type == RESOURCE_TYPE_CONSTANT_BUFFER && binding.pBuffer != nullptr)
				{
					writeInfo.pBufferInfo = &binding.BufferInfo;
					if (binding.Slot.Usage == RESOURCE_USAGE_DYNAMIC)
					{
						const BufferDesc& bufferDesc = binding.pBuffer->GetDesc();
						binding.BufferInfo.buffer = reinterpret_cast<VkBuffer>(binding.pBuffer->GetNativeHandle());
						binding.BufferInfo.offset = 0;
						binding.BufferInfo.range = bufferDesc.SizeInBytes;

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
					writeInfo.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER;
					writeInfo.pImageInfo = &binding.ImageInfo;

					//Add to descriptor writes
					m_DescriptorWrites.emplace_back(writeInfo);
				}
			}

			//Write all descriptors
			if (m_DescriptorWrites.size() > 0)
			{
				VKNDevice& device = VKNDevice::Get();
				vkUpdateDescriptorSets(device.GetVkDevice(), uint32(m_DescriptorWrites.size()), m_DescriptorWrites.data(), 0, nullptr);
				m_DescriptorWrites.clear();
			}

			m_DynamicOffsets.resize(m_DynamicBuffers.size());
			m_IsDirty = false;
		}


		//Update bufferoffsets
		for (size_t i = 0; i < m_DynamicBuffers.size(); i++)
			m_DynamicOffsets[i] = uint32(m_DynamicBuffers[i]->GetDynamicOffset());
	}
    
    
    void* VKNPipelineState::GetNativeHandle() const
    {
        return reinterpret_cast<void*>(m_Pipeline);
    }


	void VKNPipelineState::SetName(const char* pName)
    {
		if (pName != nullptr)
		{
			std::string name(pName);
			m_pDevice->SetVulkanObjectName(VK_OBJECT_TYPE_PIPELINE, (uint64)m_Pipeline, name);
		}
    }
}
