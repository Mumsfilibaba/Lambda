#include "LambdaPch.h"
#include "VKNShaderVariableTable.h"
#include "VKNDevice.h"
#include "VKNDeviceContext.h"
#include "VKNDeviceAllocator.h"
#include "VKNConversions.inl"

namespace Lambda
{
	//----------------------
	//VKNShaderVariableTable
	//----------------------

	VKNShaderVariableTable::VKNShaderVariableTable(VKNDevice* pDevice, VKNPipelineState* pPipelineState, const ShaderVariableTableDesc& desc)
		: DeviceObjectBase<VKNDevice, IShaderVariableTable>(pDevice),
		m_PipelineState(nullptr),
		m_DescriptorSet(VK_NULL_HANDLE)
	{
		this->AddRef();

		LAMBDA_ASSERT(pPipelineState != nullptr);
		
		pPipelineState->AddRef();
		m_PipelineState = pPipelineState;

		Init(desc);
	}
	

	VKNShaderVariableTable::~VKNShaderVariableTable()
	{
		SafeDeleteArr(m_pDynamicOffsets);
		LOG_DEBUG_INFO("Vulkan: Destroyed ShaderVariableTable\n");
	}


	void VKNShaderVariableTable::Init(const ShaderVariableTableDesc& desc)
	{
		//Create variabless
		for (uint32 i = 0; i < desc.NumVariables; i++)
		{
			//Create new variable
			VKNShaderVariable* pVariable = DBG_NEW VKNShaderVariable(m_pDevice, this, desc.pVariables[i]);
			m_ShaderVariables.emplace_back(pVariable);

			//Save the variable in a map so we can find it easy by name
			m_NameTable[std::string(desc.pVariables[i].pName)] = pVariable;

			//Get dynamic buffers
			if (desc.pVariables[i].Usage == RESOURCE_USAGE_DYNAMIC && desc.pVariables[i].Type == RESOURCE_TYPE_CONSTANT_BUFFER)
                m_DynamicVars.emplace_back(pVariable);
		}

		//Allocate descriptorset
		VkDescriptorSetLayout descriptorSetLayout = m_PipelineState->GetVkDescriptorSetLayout();
		m_DescriptorSet = m_PipelineState->GetAllocator()->Allocate(descriptorSetLayout);

		//Dynamic offsets for dynamic constantbuffers
		m_pDynamicOffsets = DBG_NEW uint32[m_DynamicVars.size()];
	}


	IShaderVariable* VKNShaderVariableTable::GetVariableByName(ShaderStage shader, const char* pName)
	{
		auto var = m_NameTable.find(std::string(pName));
		if (var == m_NameTable.end())
		{
			LOG_DEBUG_ERROR("Vulkan: Invalid name on ShaderVariable\n");
			return nullptr;
		}

		return var->second;
	}


	IShaderVariable* VKNShaderVariableTable::GetVariableByIndex(ShaderStage shader, uint32 index)
	{
		LAMBDA_ASSERT(index < m_ShaderVariables.size());
		return m_ShaderVariables[index].Get();
	}


	IPipelineState* VKNShaderVariableTable::GetPipelineState() const
	{
		m_PipelineState->AddRef();
		return m_PipelineState.Get();
	}


	uint32 VKNShaderVariableTable::GetVariableCount() const
	{
		return uint32(m_ShaderVariables.size());
	}
	

	void VKNShaderVariableTable::CommitAndTransitionResources(VKNDeviceContext* pContext)
	{
		//If table is empty we return
		if (m_ShaderVariables.empty())
		{
			return;
		}

		//Varify all variables (Have the resources changed)
		m_DescriptorWrites.clear();
        bool writeDescriptors = false;
        for (auto& pVar : m_ShaderVariables)
        {
            //If validation failes we need to write the descriptors
            if (!pVar->Validate())
                writeDescriptors = true;
            
			//Transition variable
			pVar->Transition(pContext);

			//Setup write info
            const VkWriteDescriptorSet& writeInfo = pVar->GetVkWriteDescriptorSet();
            m_DescriptorWrites.emplace_back(writeInfo);
        }
        
        
        //Get dynamic offsets
        uint32 dynamicOffsetIndex = 0;
        for (auto pDynamicVar : m_DynamicVars)
        {
            //Get dynamic offset
			LAMBDA_ASSERT(pDynamicVar->GetDesc().Type == RESOURCE_TYPE_CONSTANT_BUFFER);
            m_pDynamicOffsets[dynamicOffsetIndex++] = pDynamicVar->GetDynamicOffset();
        }
        
        
		//Write descriptor set
		if (writeDescriptors)
		{
            //Allocate descriptorset
            VkDescriptorSetLayout descriptorSetLayout = m_PipelineState->GetVkDescriptorSetLayout();
            m_DescriptorSet = m_PipelineState->GetAllocator()->Allocate(descriptorSetLayout);
            
			//Setup the current set for writing
			for (auto& writeInfo : m_DescriptorWrites)
				writeInfo.dstSet = m_DescriptorSet;

			//Write all descriptors
			vkUpdateDescriptorSets(m_pDevice->GetVkDevice(), uint32(m_DescriptorWrites.size()), m_DescriptorWrites.data(), 0, nullptr);
		}
	}
}