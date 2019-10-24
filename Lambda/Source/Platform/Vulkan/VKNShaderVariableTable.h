#pragma once
#include "Graphics/Core/IShaderVariableTable.h"
#include "Graphics/Core/DeviceObjectBase.h"
#include "VKNPipelineState.h"
#include "VKNShaderVariable.h"

namespace Lambda
{
	class VKNDevice;
	class VKNDeviceContext;

	//----------------------
	//VKNShaderVariableTable
	//----------------------

	class VKNShaderVariableTable : public DeviceObjectBase<VKNDevice, IShaderVariableTable>
	{
	public:
		LAMBDA_NO_COPY(VKNShaderVariableTable);

		VKNShaderVariableTable(VKNDevice* pDevice, VKNPipelineState* pPipelineState, const ShaderVariableTableDesc& desc);
		~VKNShaderVariableTable();

		virtual IShaderVariable* GetVariableByName(ShaderStage shader, const char* pName) override final;
		virtual IShaderVariable* GetVariableByIndex(ShaderStage shader, uint32 index) override final;
		virtual IPipelineState* GetPipelineState() const override final;
		virtual uint32 GetVariableCount() const override final;
		void CommitAndTransitionResources(VKNDeviceContext* pContext);
        
        inline uint32* GetDynamicOffsets() const            { return m_pDynamicOffsets; }
        inline uint32 GetDynamicOffsetCount() const         { return uint32(m_DynamicVars.size()); }
        inline VkDescriptorSet GetVkDescriptorSet() const   { return m_DescriptorSet; }
	private:
		void Init(const ShaderVariableTableDesc& desc);
	private:
		AutoRef<VKNPipelineState>   m_PipelineState;
		VkDescriptorSet	            m_DescriptorSet;
		uint32* m_pDynamicOffsets;
        std::vector<VKNShaderVariable*> m_DynamicVars;
        std::vector<VkWriteDescriptorSet> m_DescriptorWrites;
        std::vector<AutoRef<VKNShaderVariable>> m_ShaderVariables;
		std::unordered_map<std::string, VKNShaderVariable*> m_NameTable;
	};
}
