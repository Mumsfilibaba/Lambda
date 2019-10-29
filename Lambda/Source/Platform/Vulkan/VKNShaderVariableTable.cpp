#include "LambdaPch.h"
#include "VKNShaderVariableTable.h"
#include "VKNDevice.h"
#include "VKNDeviceContext.h"
#include "VKNConversions.inl"

namespace Lambda
{
	//----------------------
	//VKNShaderVariableTable
	//----------------------

	VKNShaderVariableTable::VKNShaderVariableTable(VKNDevice* pDevice, VKNPipelineState* pPipelineState, const ShaderVariableTableDesc& desc)
		: TShaderVariableTable(pDevice, pPipelineState),
		m_DescriptorSet(VK_NULL_HANDLE),
		m_pDescriptors(nullptr),
		m_NumDescriptors(0)
	{
		this->AddRef();	
		Init(desc);
	}
	

	VKNShaderVariableTable::~VKNShaderVariableTable()
	{
		SafeDeleteArr(m_pDynamicOffsets);
		SafeDeleteArr(m_pDescriptors);

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
		m_DescriptorSet = m_PipelineState->AllocateDescriptorSet();;

		//Dynamic offsets for dynamic constantbuffers
		m_pDynamicOffsets = DBG_NEW uint32[m_DynamicVars.size()];

		//Set size of descriptorwrites
		m_pDescriptors = DBG_NEW VkWriteDescriptorSet[desc.NumVariables];
		m_NumDescriptors = desc.NumVariables;
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
        bool writeDescriptors = false;
		size_t index = 0;
		uint32 dynamicOffsetIndex = 0;
        for (auto& pVar : m_ShaderVariables)
        {
            //If validation failes we need to write the descriptors
			if (!pVar->Validate())
			{
				//Setup write info
				m_pDescriptors[index] = pVar->GetVkWriteDescriptorSet();
                writeDescriptors = true;
			}
            
			//Update dynamic offsets
			if (pVar->GetDesc().Type == RESOURCE_TYPE_CONSTANT_BUFFER && pVar->GetDesc().Usage == RESOURCE_USAGE_DYNAMIC)
				m_pDynamicOffsets[dynamicOffsetIndex++] = pVar->GetDynamicOffset();

			//Transition variable
			pVar->Transition(pContext);
			
			index++;
        }
        
        
		//Write descriptor set
		if (writeDescriptors)
		{
            //Allocate descriptorset
            m_DescriptorSet = m_PipelineState->AllocateDescriptorSet();
            
			//Setup the current set for writing
			for (uint32 i = 0; i < m_NumDescriptors; i++)
				m_pDescriptors[i].dstSet = m_DescriptorSet;

			//Write all descriptors
			vkUpdateDescriptorSets(m_pDevice->GetVkDevice(), m_NumDescriptors, m_pDescriptors, 0, nullptr);
		}
	}
}
