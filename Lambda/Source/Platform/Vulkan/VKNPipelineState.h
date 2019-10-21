#pragma once
#include "Graphics/Core/PipelineStateBase.h"
#include <map>
#include <vector>
#include <vulkan/vulkan.h>

namespace Lambda
{
	class IBuffer;
	class ITexture;
	class VKNBuffer;
	class VKNDevice;
	class VKNTexture;
	class ISamplerState;
	class VKNSamplerState;
	class VKNDescriptorSetAllocator;

	//----------------
	//VKNPipelineState
	//----------------

    class VKNPipelineState final : public PipelineStateBase<VKNDevice>
    {
    public:
        LAMBDA_NO_COPY(VKNPipelineState);
        
        VKNPipelineState(VKNDevice* pDevice, const PipelineStateDesc& desc);
        ~VKNPipelineState();

		virtual void CreateShaderVariableTable(IShaderVariableTable** ppVariableTable) override final;
        virtual void SetName(const char* pName) override final;
        virtual void* GetNativeHandle() const override final;

		inline VkPipeline GetVkPipeline() const							{ return m_Pipeline; }
		inline VkPipelineLayout GetVkPipelineLayout() const				{ return m_PipelineLayout; }
		inline VkDescriptorSetLayout GetVkDescriptorSetLayout() const	{ return m_DescriptorSetLayout; }
		inline VKNDescriptorSetAllocator* GetAllocator() const			{ return m_pAllocator; };
    private:
        void Init(const PipelineStateDesc& desc);
    private:
        VKNDescriptorSetAllocator* m_pAllocator;
        VkPipeline            m_Pipeline;
        VkPipelineLayout      m_PipelineLayout;
        VkDescriptorSetLayout m_DescriptorSetLayout;
        std::vector<ShaderVariableDesc> m_ShaderVariableDescs;
        std::vector<ConstantBlockDesc>  m_ConstantBlockDescs;
    };
}
