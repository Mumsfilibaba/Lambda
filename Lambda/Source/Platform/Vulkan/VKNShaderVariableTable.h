#pragma once
#include "Graphics/Core/ShaderVariableTableBase.h"
#include "VKNPipelineState.h"
#include "VKNShaderVariable.h"

namespace Lambda
{
	class VKNDevice;
	class VKNDeviceContext;

	//----------------------
	//VKNShaderVariableTable
	//----------------------

	class VKNShaderVariableTable : public ShaderVariableTableBase<VKNDevice, VKNPipelineState>
	{
		using TShaderVariableTable = ShaderVariableTableBase<VKNDevice, VKNPipelineState>;

	public:
		LAMBDA_NO_COPY(VKNShaderVariableTable);

		VKNShaderVariableTable(VKNDevice* pVkDevice, VKNPipelineState* pVkPipelineState, const ShaderVariableTableDesc& desc);
		~VKNShaderVariableTable();

		virtual IShaderVariable* GetVariableByName(ShaderStage shader, const char* pName) override final;
		virtual IShaderVariable* GetVariableByIndex(ShaderStage shader, uint32 index) override final;
		virtual uint32 GetVariableCount() const override final;
		void CommitResources();
        

        _forceinline uint32* GetDynamicOffsets() const            
		{ 
			return m_pDynamicOffsets; 
		}
		
		
		_forceinline uint32 GetDynamicOffsetCount() const         
		{ 
			return uint32(m_DynamicVars.size()); 
		}
		
		
		_forceinline VkDescriptorSet GetVkDescriptorSet() const   
		{ 
			return m_VkDescriptorSet; 
		}


		_forceinline VkSampler GetStaticVkSampler(const std::string& key) const
		{
			return m_PipelineState->GetStaticVkSampler(key);
		}
	private:
		void Init(const ShaderVariableTableDesc& desc);
	private:
		std::unordered_map<std::string, VKNShaderVariable*> m_NameTable;
        std::vector<AutoRef<VKNShaderVariable>> m_ShaderVariables;
        std::vector<VKNShaderVariable*> m_DynamicVars;
		VkWriteDescriptorSet* m_pVkDescriptors;
		uint32* m_pDynamicOffsets;
		VkDescriptorSet	m_VkDescriptorSet;
		uint32 m_NumDescriptors;
	};
}
