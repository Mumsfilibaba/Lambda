#pragma once
#include "Graphics/Core/PipelineStateBase.h"
#include <map>
#include <vector>
#include <unordered_map>
#include "Memory/VKNDescriptorAllocator.h"
#include "Vulkan.h"

namespace Lambda
{
	class IBuffer;
	class ITexture;
	class VKNBuffer;
	class VKNDevice;
	class VKNTexture;
	class ISamplerState;
	class VKNSamplerState;

	//----------------
	//VKNPipelineState
	//----------------

    class VKNPipelineState final : public PipelineStateBase<VKNDevice>
    {
		using TPipelineState = PipelineStateBase<VKNDevice>;

    public:
        LAMBDA_NO_COPY(VKNPipelineState);
        
        VKNPipelineState(VKNDevice* pDevice, const PipelineStateDesc& desc);
        ~VKNPipelineState();

		virtual void CreateShaderVariableTable(IShaderVariableTable** ppVariableTable) override final;
        virtual void SetName(const char* pName) override final;
        virtual void* GetNativeHandle() const override final;

		
		_forceinline VkDescriptorSet AllocateDescriptorSet() const			
		{ 
			return m_pAllocator->Allocate(m_DescriptorSetLayout); 
		};


		_forceinline VkPipeline GetVkPipeline() const							
		{ 
			return m_Pipeline; 
		}
		
		
		_forceinline VkPipelineLayout GetVkPipelineLayout() const				
		{ 
			return m_PipelineLayout; 
		}
		
		
		_forceinline VkDescriptorSetLayout GetVkDescriptorSetLayout() const	
		{ 
			return m_DescriptorSetLayout; 
		}


		_forceinline VkSampler GetStaticVkSampler(const std::string& key) const
		{
			auto samplerState = m_StaticSamplerStates.find(key);
			if (samplerState != m_StaticSamplerStates.end())
				return samplerState->second;

			return VK_NULL_HANDLE;
		}
    private:
        void Init(const PipelineStateDesc& desc);
    private:
        VKNDescriptorSetAllocator* m_pAllocator;
        VkPipeline            m_Pipeline;
        VkPipelineLayout      m_PipelineLayout;
        VkDescriptorSetLayout m_DescriptorSetLayout;
        std::vector<ShaderVariableDesc> m_ShaderVariableDescs;
        std::vector<ConstantBlockDesc>  m_ConstantBlockDescs;
		std::unordered_map<std::string, VkSampler> m_StaticSamplerStates;
    };
}
