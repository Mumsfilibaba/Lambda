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
        
        VKNPipelineState(VKNDevice* pVkDevice, const PipelineStateDesc& desc);
        ~VKNPipelineState();

		virtual void CreateShaderVariableTable(IShaderVariableTable** ppVariableTable) override final;
        virtual void SetName(const char* pName) override final;
        virtual void* GetNativeHandle() const override final;

		
		_forceinline VkDescriptorSet AllocateDescriptorSet() const			
		{ 
			return m_pAllocator->Allocate(m_VkDescriptorSetLayout); 
		};


		_forceinline VkPipeline GetVkPipeline() const							
		{ 
			return m_VkPipeline; 
		}
		
		
		_forceinline VkPipelineLayout GetVkPipelineLayout() const				
		{ 
			return m_VkPipelineLayout; 
		}
		
		
		_forceinline VkDescriptorSetLayout GetVkDescriptorSetLayout() const	
		{ 
			return m_VkDescriptorSetLayout; 
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
        std::vector<ShaderVariableDesc> m_ShaderVariableDescs;
        std::vector<ConstantBlockDesc>  m_ConstantBlockDescs;
		std::unordered_map<std::string, VkSampler> m_StaticSamplerStates;
        VKNDescriptorSetAllocator* m_pAllocator;
        VkPipeline m_VkPipeline;
        VkPipelineLayout m_VkPipelineLayout;
        VkDescriptorSetLayout m_VkDescriptorSetLayout;
    };
}
