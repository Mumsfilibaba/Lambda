#include "LambdaPch.h"
#include "VKNPipelineState.h"
#include "VKNShader.h"
#include "VKNBuffer.h"
#include "VKNDevice.h"
#include "VKNTexture.h"
#include "VKNUtilities.h"
#include "VKNSamplerState.h"
#include "VKNSamplerState.h"
#include "VKNRenderPassCache.h"
#include "VKNShaderVariableTable.h"
#include "VKNConversions.inl"

namespace Lambda
{
	//----------------
	//VKNPipelineState
	//----------------

    VKNPipelineState::VKNPipelineState(VKNDevice* pVkDevice, const PipelineStateDesc& desc)
        : PipelineStateBase<VKNDevice>(pVkDevice, desc),
		m_pAllocator(nullptr),
        m_VkPipeline(VK_NULL_HANDLE),
        m_VkPipelineLayout(VK_NULL_HANDLE),
        m_VkDescriptorSetLayout(VK_NULL_HANDLE),
        m_ShaderVariableDescs(),
        m_ConstantBlockDescs()
    {
		//Add a ref to the refcounter
		this->AddRef();
        Init(desc);
    }


	VKNPipelineState::~VKNPipelineState()
	{
		//Delete static samplers
		for (auto sampler : m_StaticSamplerStates)
		{
			if (sampler.second != VK_NULL_HANDLE)
				m_pDevice->SafeReleaseVkResource<VkSampler>(sampler.second);
		}

		//Delete DesctiptorAllocator
		if (m_pAllocator)
		{
			m_pAllocator->Destroy(m_pDevice->GetVkDevice());
			m_pAllocator = nullptr;
		}

		//Delete pipelineresources
		if (m_VkPipeline != VK_NULL_HANDLE)
			m_pDevice->SafeReleaseVkResource<VkPipeline>(m_VkPipeline);
		if (m_VkDescriptorSetLayout != VK_NULL_HANDLE)
			m_pDevice->SafeReleaseVkResource<VkDescriptorSetLayout>(m_VkDescriptorSetLayout);
		if (m_VkPipelineLayout != VK_NULL_HANDLE)
			m_pDevice->SafeReleaseVkResource<VkPipelineLayout>(m_VkPipelineLayout);

		LOG_DEBUG_INFO("[Vulkan] Destroyed PipelineState\n");
	}

	
	void VKNPipelineState::CreateShaderVariableTable(IShaderVariableTable** ppVariableTable)
	{
		LAMBDA_ASSERT(ppVariableTable != nullptr);

		ShaderVariableTableDesc desc = {};
		desc.NumVariables		= uint32(m_ShaderVariableDescs.size());
		desc.pVariables			= m_ShaderVariableDescs.data();
		desc.NumConstantBlocks	= uint32(m_ConstantBlockDescs.size());
		desc.pConstantBlocks	= m_ConstantBlockDescs.data();

		(*ppVariableTable) = DBG_NEW VKNShaderVariableTable(m_pDevice, this, desc);
	}

    
    void VKNPipelineState::Init(const PipelineStateDesc& desc)
    {
		//Get renderpass        
		VKNRenderPassCache& cache = VKNRenderPassCache::Get();
		VKNRenderPassCacheKey key = {};
		key.SampleCount			= desc.GraphicsPipeline.SampleCount;
		key.DepthStencilFormat	= desc.GraphicsPipeline.DepthStencilFormat;
		key.NumRenderTargets	= desc.GraphicsPipeline.NumRenderTargets;
		for (uint32 i = 0; i < key.NumRenderTargets; i++)
			key.RenderTargetFormats[i] = desc.GraphicsPipeline.RenderTargetFormats[i];

		VkRenderPass renderPass = cache.GetRenderPass(key);

		//Copy the resourceslots
		const ShaderVariableTableDesc& varibleLayout = desc.ShaderVariableTable;
		for (uint32 i = 0; i < varibleLayout.NumStaticSamplerStates; i++)
		{
			//Get desc
			const StaticSamplerStateDesc& staticSamplerStateDesc = varibleLayout.pStaticSamplerStates[i];

			//Static samplers must have a name
			if (staticSamplerStateDesc.pName == nullptr)
			{
				LOG_DEBUG_ERROR("[Vulkan] Static SamplerStates must have a name\n");
			}

			//Get adress mode
			VkSamplerAddressMode adressMode = ConvertSamplerAdressMode(staticSamplerStateDesc.AdressMode);

			//Create sampler
			VkSamplerCreateInfo info = {};
			info.sType					 = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
			info.pNext					 = nullptr;
			info.flags					 = 0;
			info.magFilter				 = VK_FILTER_LINEAR;
			info.minFilter				 = VK_FILTER_LINEAR;
			info.addressModeU			 = adressMode;
			info.addressModeV			 = adressMode;
			info.addressModeW			 = adressMode;
			info.anisotropyEnable		 = VK_TRUE;
			info.maxAnisotropy			 = staticSamplerStateDesc.Anisotropy;
			info.borderColor			 = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
			info.unnormalizedCoordinates = VK_FALSE;
			info.compareEnable			 = VK_FALSE;
			info.compareOp				 = VK_COMPARE_OP_ALWAYS;
			info.mipmapMode				 = VK_SAMPLER_MIPMAP_MODE_LINEAR;
			info.minLod					 = staticSamplerStateDesc.MinMipLOD;
			info.maxLod					 = staticSamplerStateDesc.MaxMipLOD;
			info.mipLodBias				 = staticSamplerStateDesc.MipLODBias;

			VkSampler staticSampler = VK_NULL_HANDLE;
			if (vkCreateSampler(m_pDevice->GetVkDevice(), &info, nullptr, &staticSampler) != VK_SUCCESS)
			{
				LOG_DEBUG_ERROR("[Vulkan] Failed to create create Static SamplerState\n");
			}
			else
			{
				LOG_DEBUG_INFO("[Vulkan] Created Static SamplerState\n");

				auto sampler = m_StaticSamplerStates.find(staticSamplerStateDesc.pName);
				if (sampler != m_StaticSamplerStates.end())
				{
					LOG_DEBUG_WARNING("[Vulkan] Multiple Static SamplerState with the same name '%s'. Only the first one is created\n", staticSamplerStateDesc.pName);
					m_pDevice->SafeReleaseVkResource<VkSampler>(staticSampler);
				}
				else
				{
					m_StaticSamplerStates.insert(std::pair<std::string, VkSampler>(staticSamplerStateDesc.pName, staticSampler));
					
					if (desc.pName)
						m_pDevice->SetVulkanObjectName(VK_OBJECT_TYPE_SAMPLER, (uint64)staticSampler, std::string(desc.pName) + " - Static Sampler [" + std::string(staticSamplerStateDesc.pName) + "]");
				}
			}
		}


		//Create descriptor bindings for each shadervariable
		std::vector<VkDescriptorSetLayoutBinding> layoutBindings;
		for (uint32 i = 0; i < varibleLayout.NumVariables; i++)
		{
			const ShaderVariableDesc& variable = varibleLayout.pVariables[i];
			m_ShaderVariableDescs.emplace_back(variable);

			VkDescriptorSetLayoutBinding layoutBinding = {};
			layoutBinding.descriptorCount		= 1;
			layoutBinding.binding				= variable.Slot;
			layoutBinding.pImmutableSamplers	= nullptr;
			layoutBinding.stageFlags			= ConvertShaderStages(variable.Stage);
			layoutBinding.descriptorType		= ConvertResourceToDescriptorType(variable.Type, variable.Usage);
            if (layoutBinding.descriptorType == VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE && variable.pStaticSamplerName != nullptr)
			{
				//With static samplers we use the combined image sampler
				layoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;

				//Bind sampler to slot permanently
				const auto& sampler = m_StaticSamplerStates.find(std::string(variable.pStaticSamplerName));
				if (sampler != m_StaticSamplerStates.end())
				{
					VkSampler& vkSampler = sampler->second;
					layoutBinding.pImmutableSamplers = &vkSampler;
				}
				else
				{
					LOG_DEBUG_ERROR("[Vulkan] No Static SamplerState with the name '%s'\n", variable.pStaticSamplerName);
				}
			}

			layoutBindings.emplace_back(layoutBinding);
		}

		//Create constantranges
		std::vector<VkPushConstantRange> constantRanges;
		for (uint32 i = 0; i < varibleLayout.NumConstantBlocks; i++)
		{
			const ConstantBlockDesc& block = varibleLayout.pConstantBlocks[i];
			m_ConstantBlockDescs.emplace_back(block);

			VkPushConstantRange range = {};
			range.size		 = block.SizeInBytes;
			range.offset	 = 0;
			range.stageFlags = ConvertShaderStages(block.Stage);
			constantRanges.push_back(range);
		}


		//Create pipelineLayout
		VkDescriptorSetLayoutCreateInfo descriptorLayoutInfo = {};
		descriptorLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		descriptorLayoutInfo.pNext = nullptr;
		descriptorLayoutInfo.flags = 0;
		descriptorLayoutInfo.bindingCount	= uint32(layoutBindings.size());
		descriptorLayoutInfo.pBindings		= layoutBindings.data();
		if (vkCreateDescriptorSetLayout(m_pDevice->GetVkDevice(), &descriptorLayoutInfo, nullptr, &m_VkDescriptorSetLayout) != VK_SUCCESS)
		{
			LOG_DEBUG_ERROR("[Vulkan] Failed to create DescriptorSetLayout\n");
			return;
		}
		else
		{
			LOG_DEBUG_INFO("[Vulkan] Created DescriptorSetLayout\n");
		}

		//Create pipelinelayout
		VkPipelineLayoutCreateInfo layoutInfo = {};
		layoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		layoutInfo.flags = 0;
		layoutInfo.pNext = nullptr;
		layoutInfo.setLayoutCount			= 1;
		layoutInfo.pSetLayouts				= &m_VkDescriptorSetLayout;
		layoutInfo.pushConstantRangeCount	= uint32(constantRanges.size());
		layoutInfo.pPushConstantRanges		= constantRanges.data();
		if (vkCreatePipelineLayout(m_pDevice->GetVkDevice(), &layoutInfo, nullptr, &m_VkPipelineLayout) != VK_SUCCESS)
		{
			LOG_DEBUG_ERROR("[Vulkan] Failed to create PipelineLayout\n");
			return;
		}
		else
		{
			LOG_DEBUG_INFO("[Vulkan] Created PipelineLayout\n");

			//Create allocator
			m_pAllocator = DBG_NEW VKNDescriptorSetAllocator(m_pDevice, 64, 64, 64, 64, 64, 64);
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
				attributeDescription.format     = ConvertFormat(vertexInputDesc.pElements[i].Format);
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
			multisampling.rasterizationSamples  = ConvertSampleCount(desc.GraphicsPipeline.SampleCount);
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
			pipelineInfo.layout                 = m_VkPipelineLayout;
			pipelineInfo.renderPass             = renderPass;
			pipelineInfo.subpass                = 0;
			pipelineInfo.basePipelineHandle     = VK_NULL_HANDLE;
			pipelineInfo.basePipelineIndex      = -1;
			if (vkCreateGraphicsPipelines(m_pDevice->GetVkDevice(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_VkPipeline) != VK_SUCCESS)
			{
				LOG_DEBUG_ERROR("[Vulkan] Failed to create GraphicsPipelineState\n");
				return;
			}
			else
			{
				LOG_DEBUG_INFO("[Vulkan] Created GraphicsPipelineState\n");
				SetName(desc.pName);
			}
		}
		else if (desc.Type == PIPELINE_TYPE_COMPUTE)
		{
			LOG_DEBUG_ERROR("[Vulkan] Compute pipeline not implemented\n");
		}
		else
		{
			LOG_DEBUG_ERROR("[Vulkan] Unknown PiplineType\n");
		}
    }

    
    void* VKNPipelineState::GetNativeHandle() const
    {
        return reinterpret_cast<void*>(m_VkPipeline);
    }


	void VKNPipelineState::SetName(const char* pName)
    {
		TPipelineState::SetName(pName);
		if (pName)
		{
			m_pDevice->SetVulkanObjectName(VK_OBJECT_TYPE_PIPELINE, (uint64)m_VkPipeline, m_Name);
			m_Desc.pName = m_Name.c_str();
		}
    }
}
