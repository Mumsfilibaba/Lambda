#include "LambdaPch.h"
#include "VKNPipelineState.h"
#include "VKNShader.h"
#include "VKNRenderPass.h"
#include "VKNPipelineResourceState.h"
#include "VKNDevice.h"
#include "VKNUtilities.h"
#include "VKNConversions.inl"

namespace Lambda
{
	//------------------------
	//VKNGraphicsPipelineState
	//------------------------

    VKNGraphicsPipelineState::VKNGraphicsPipelineState(const GraphicsPipelineStateDesc& desc)
        : m_Pipeline(VK_NULL_HANDLE)
    {
        Init(desc);
    }

    
    void VKNGraphicsPipelineState::Init(const GraphicsPipelineStateDesc& desc)
    {       
        //Describe shaderstages
        std::vector<VkPipelineShaderStageCreateInfo> shaderStages;
        
        //VertexShader
        if (desc.pVertexShader)
        {
			ShaderDesc vsDesc = desc.pVertexShader->GetDesc();
            VkPipelineShaderStageCreateInfo info = {};
            info.sType                  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            info.pNext                  = nullptr;
            info.flags                  = 0;
            info.stage                  = VK_SHADER_STAGE_VERTEX_BIT;
            info.pName                  = vsDesc.pEntryPoint;
            info.module                 = reinterpret_cast<VkShaderModule>(desc.pVertexShader->GetNativeHandle());
            info.pSpecializationInfo    = nullptr;
            
            shaderStages.push_back(info);
        }
        
        //PixelShader
        if (desc.pPixelShader)
        {
			ShaderDesc psDesc = desc.pPixelShader->GetDesc();
            VkPipelineShaderStageCreateInfo info = {};
            info.sType                  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            info.pNext                  = nullptr;
            info.flags                  = 0;
            info.stage                  = VK_SHADER_STAGE_FRAGMENT_BIT;
            info.pName                  = psDesc.pEntryPoint;
            info.module                 = reinterpret_cast<VkShaderModule>(desc.pPixelShader->GetNativeHandle());
            info.pSpecializationInfo    = nullptr;
            
            shaderStages.push_back(info);
        }
        
        
        //InputLayout
        std::vector<VkVertexInputBindingDescription> bindingDescriptions;
        std::vector<VkVertexInputAttributeDescription> attributeDescriptions;
        for (uint32 i = 0; i < desc.InputElementCount; i++)
        {
            //Convert an input element to the corresponding AttributeDescription
            VkVertexInputAttributeDescription attributeDescription = {};
            attributeDescription.binding    = desc.pInputElements[i].BindingSlot;
            attributeDescription.location   = desc.pInputElements[i].InputSlot;
            attributeDescription.offset     = desc.pInputElements[i].StructureOffset;
            attributeDescription.format     = ConvertResourceFormat(desc.pInputElements[i].Format);
            attributeDescriptions.push_back(attributeDescription);
            
            //Check if binding is unique
            bool found = false;
            for (auto& bindDesc : bindingDescriptions)
            {
                if (bindDesc.binding == desc.pInputElements[i].BindingSlot)
                {
                    found = true;
                    break;
                }
            }
            
            //Add a new bindingdescription
            if (!found)
            {
                VkVertexInputBindingDescription bindingDescription = {};
                bindingDescription.binding      = desc.pInputElements[i].BindingSlot;
                bindingDescription.stride       = desc.pInputElements[i].Stride;
                bindingDescription.inputRate    = desc.pInputElements[i].IsInstanced ? VK_VERTEX_INPUT_RATE_INSTANCE : VK_VERTEX_INPUT_RATE_VERTEX;
                bindingDescriptions.push_back(bindingDescription);
            }
        }
        
        //Set inputlayout
        VkPipelineVertexInputStateCreateInfo inputLayout = {};
        inputLayout.sType							= VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        inputLayout.pNext							= nullptr;
        inputLayout.flags							= 0;
        inputLayout.vertexBindingDescriptionCount	= uint32(bindingDescriptions.size());
        inputLayout.pVertexBindingDescriptions		= bindingDescriptions.data();
        inputLayout.vertexAttributeDescriptionCount = uint32(attributeDescriptions.size());
        inputLayout.pVertexAttributeDescriptions	= attributeDescriptions.data();
        
        VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
        inputAssembly.sType						= VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        inputAssembly.pNext						= nullptr;
        inputAssembly.flags						= 0;
        inputAssembly.topology					= ConvertPrimitiveTopology(desc.Topology);
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
        VkPipelineRasterizationStateCreateInfo rasterizerState = {};
        rasterizerState.sType                   = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterizerState.pNext                   = nullptr;
        rasterizerState.flags                   = 0;
        rasterizerState.depthClampEnable        = VK_FALSE;
        rasterizerState.rasterizerDiscardEnable = VK_FALSE;
        rasterizerState.polygonMode             = ConvertPolygonMode(desc.Mode);
        rasterizerState.lineWidth               = 1.0f;
        rasterizerState.frontFace               = VK_FRONT_FACE_CLOCKWISE;
        rasterizerState.depthBiasEnable         = VK_FALSE;
        rasterizerState.depthBiasConstantFactor = 0.0f;
        rasterizerState.depthBiasClamp          = 0.0f;
        rasterizerState.depthBiasSlopeFactor    = 0.0f;
        
        //Set cullmode
        if (desc.Cull == CULL_MODE_BACK)
            rasterizerState.cullMode                = VK_CULL_MODE_BACK_BIT;
        else if (desc.Cull == CULL_MODE_FRONT)
            rasterizerState.cullMode                = VK_CULL_MODE_FRONT_BIT;
        else if (desc.Cull == CULL_MODE_NONE)
            rasterizerState.cullMode                = VK_CULL_MODE_NONE;
        
        
        //Multisampling
        VkPipelineMultisampleStateCreateInfo multisampling = {};
        multisampling.sType                 = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisampling.sampleShadingEnable   = VK_FALSE;
        multisampling.rasterizationSamples  = reinterpret_cast<VKNRenderPass*>(desc.pRenderPass)->GetSampleCount();
        multisampling.minSampleShading      = 1.0f;
        multisampling.pSampleMask           = nullptr;
        multisampling.alphaToCoverageEnable = VK_FALSE;
        multisampling.alphaToOneEnable      = VK_FALSE;
        
        
        //Blendstate
        VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
        colorBlendAttachment.colorWriteMask         = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        colorBlendAttachment.blendEnable            = VK_FALSE;
        colorBlendAttachment.srcColorBlendFactor    = VK_BLEND_FACTOR_ONE;
        colorBlendAttachment.dstColorBlendFactor    = VK_BLEND_FACTOR_ZERO;
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
        VkPipelineDepthStencilStateCreateInfo depthStencilState = {};
        depthStencilState.sType                  = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
        depthStencilState.flags                  = 0;
        depthStencilState.pNext                  = nullptr;
        depthStencilState.depthTestEnable        = desc.DepthTest ? VK_TRUE : VK_FALSE;
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
        pipelineInfo.layout                 = reinterpret_cast<VkPipelineLayout>(desc.pResourceState->GetNativeHandle());
        pipelineInfo.renderPass             = reinterpret_cast<VkRenderPass>(desc.pRenderPass->GetNativeHandle());
        pipelineInfo.subpass                = 0;
        pipelineInfo.basePipelineHandle     = VK_NULL_HANDLE;
        pipelineInfo.basePipelineIndex      = -1;
        
		VKNDevice& device = VKNDevice::GetInstance();
        if (vkCreateGraphicsPipelines(device.GetDevice(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_Pipeline) != VK_SUCCESS)
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
    
    
    void* VKNGraphicsPipelineState::GetNativeHandle() const
    {
        return reinterpret_cast<void*>(m_Pipeline);
    }
    
    
    void VKNGraphicsPipelineState::Destroy(VkDevice device)
    {
		LAMBDA_ASSERT(device != VK_NULL_HANDLE);

        if (m_Pipeline != VK_NULL_HANDLE)
        {
            vkDestroyPipeline(device, m_Pipeline, nullptr);
            m_Pipeline = VK_NULL_HANDLE;
        }

        delete this;
    }
    
    
    void VKNGraphicsPipelineState::SetName(const char* pName)
    {
		if (pName != nullptr)
		{
			std::string name(pName);

			VKNDevice& device = VKNDevice::GetInstance();
			device.SetVulkanObjectName(VK_OBJECT_TYPE_PIPELINE, (uint64)m_Pipeline, name);
		}
    }
}