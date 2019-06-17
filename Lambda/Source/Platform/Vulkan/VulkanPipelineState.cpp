#include "VulkanPipelineState.h"
#include "VulkanShader.h"
#include <vector>

namespace Lambda
{
    VulkanGraphicsPipelineState::VulkanGraphicsPipelineState(VkDevice device, const GraphicsPipelineStateDesc& desc)
    {
        Init(device, desc);
    }

    
    void VulkanGraphicsPipelineState::Init(VkDevice device, const GraphicsPipelineStateDesc& desc)
    {
        //describe shaderstages
        std::vector<VkPipelineShaderStageCreateInfo> shaderStages;
        
        //VertexShader
        VulkanShader* pVS = reinterpret_cast<VulkanShader*>(desc.pVertexShader);
        if (desc.pVertexShader)
        {
            VkPipelineShaderStageCreateInfo info = {};
            info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            info.pNext = nullptr;
            info.flags = 0;
            info.stage = VK_SHADER_STAGE_VERTEX_BIT;
            info.pName = pVS->GetEntryPoint();
            info.module = pVS->GetShaderModule();
            info.pSpecializationInfo = nullptr;
            
            shaderStages.push_back(info);
        }
        
        //PixelShader
        VulkanShader* pPS = reinterpret_cast<VulkanShader*>(desc.pPixelShader);
        if (desc.pVertexShader)
        {
            VkPipelineShaderStageCreateInfo info = {};
            info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            info.pNext = nullptr;
            info.flags = 0;
            info.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
            info.pName = pPS->GetEntryPoint();
            info.module = pPS->GetShaderModule();
            info.pSpecializationInfo = nullptr;
            
            shaderStages.push_back(info);
        }
        
        //InputLayout
        VkPipelineVertexInputStateCreateInfo inputLayout = {};
        inputLayout.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        inputLayout.pNext = nullptr;
        inputLayout.vertexBindingDescriptionCount = 0;
        inputLayout.pVertexBindingDescriptions = nullptr; // Optional
        inputLayout.vertexAttributeDescriptionCount = 0;
        inputLayout.pVertexAttributeDescriptions = nullptr; // Optional
        
        VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
        inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        inputAssembly.pNext = nullptr;
        inputAssembly.flags = 0;
        inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        inputAssembly.primitiveRestartEnable = VK_FALSE;
        
        //Viewport and Scissor - For now will be dynamic later
        VkViewport viewport = {};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = 1920.0f;
        viewport.height = 1080.0f;
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        
        VkRect2D scissor = {};
        scissor.offset = {0, 0};
        scissor.extent = { 1920, 1080 };
        
        VkPipelineViewportStateCreateInfo viewportState = {};
        viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewportState.viewportCount = 1;
        viewportState.pViewports = &viewport;
        viewportState.scissorCount = 1;
        viewportState.pScissors = &scissor;
        
        //RasterizerState
        VkPipelineRasterizationStateCreateInfo rasterizerState = {};
        rasterizerState.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterizerState.pNext = nullptr;
        rasterizerState.flags = 0;
        rasterizerState.depthClampEnable = VK_FALSE;
        rasterizerState.rasterizerDiscardEnable = VK_FALSE;
        rasterizerState.polygonMode = VK_POLYGON_MODE_FILL;
        rasterizerState.lineWidth = 1.0f;
        rasterizerState.cullMode = VK_CULL_MODE_BACK_BIT;
        rasterizerState.frontFace = VK_FRONT_FACE_CLOCKWISE;
        rasterizerState.depthBiasEnable = VK_FALSE;
        rasterizerState.depthBiasConstantFactor = 0.0f; // Optional
        rasterizerState.depthBiasClamp = 0.0f; // Optional
        rasterizerState.depthBiasSlopeFactor = 0.0f; // Optional
        
        
    }
    
    
    void VulkanGraphicsPipelineState::Destroy(VkDevice device)
    {
        
    }
}
