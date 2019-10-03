#pragma once
#include "Graphics/Core/IShaderVariableTable.h"
#include "Graphics/Core/DeviceObjectBase.h"
#include "VKNPipelineState.h"
#include "VKNShaderVariable.h"

namespace Lambda
{
	class VKNDevice;

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
		void CommitAndTransitionResources();

		inline void Invalidate() { m_IsDirty = true; }
		inline VkDescriptorSet GetVkDescriptorSet() const { return m_DescriptorSet; }
		inline uint32* GetDynamicOffsets() const { return m_pDynamicOffsets; }
		inline uint32 GetDynamicOffsetCount() const { return m_NumDynamicOffsets; }
	private:
		void Init(const ShaderVariableTableDesc& desc);
	private:
		AutoRef<VKNPipelineState> m_PipelineState;
		VkDescriptorSet	m_DescriptorSet;
		VkWriteDescriptorSet* m_pDescriptorWrites;
		uint32* m_pDynamicOffsets;
		uint32 m_NumDynamicOffsets;
		std::vector<AutoRef<VKNShaderVariable>> m_ShaderVariables;
		std::unordered_map<std::string, VKNShaderVariable*> m_NameTable;
		bool m_IsDirty;
	};
}
