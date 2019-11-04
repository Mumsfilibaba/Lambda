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

	VKNShaderVariableTable::VKNShaderVariableTable(VKNDevice* pVkDevice, VKNPipelineState* pVkPipelineState, const ShaderVariableTableDesc& desc)
		: TShaderVariableTable(pVkDevice, pVkPipelineState),
		m_VkDescriptorSet(VK_NULL_HANDLE),
		m_pVkDescriptors(nullptr),
		m_NumDescriptors(0)
	{
		this->AddRef();	
		Init(desc);
	}
	

	VKNShaderVariableTable::~VKNShaderVariableTable()
	{
		SafeDeleteArr(m_pDynamicOffsets);
		SafeDeleteArr(m_pVkDescriptors);

		LOG_DEBUG_INFO("[Vulkan] Destroyed ShaderVariableTable\n");
	}


	void VKNShaderVariableTable::Init(const ShaderVariableTableDesc& desc)
	{
		//Set size of descriptorwrites
		m_pVkDescriptors		= DBG_NEW VkWriteDescriptorSet[desc.NumVariables];
		m_NumDescriptors	= desc.NumVariables;

		//Create variables
		for (uint32 i = 0; i < desc.NumVariables; i++)
		{
			//Create new variable
			VKNShaderVariable* pVariable = DBG_NEW VKNShaderVariable(m_pDevice, this, desc.pVariables[i]);
			m_ShaderVariables.emplace_back(pVariable);

			//Save the variable in a map so we can find it easy by name
			m_NameTable[std::string(desc.pVariables[i].pName)] = pVariable;

			//Get dynamic buffers
			if (desc.pVariables[i].Usage == USAGE_DYNAMIC && desc.pVariables[i].Type == RESOURCE_TYPE_CONSTANT_BUFFER)
                m_DynamicVars.emplace_back(pVariable);
		}

		//Allocate descriptorset
		m_VkDescriptorSet = m_PipelineState->AllocateDescriptorSet();;

		//Dynamic offsets for dynamic constantbuffers
		m_pDynamicOffsets = DBG_NEW uint32[m_DynamicVars.size()];
	}


	IShaderVariable* VKNShaderVariableTable::GetVariableByName(ShaderStage shader, const char* pName)
	{
		auto var = m_NameTable.find(std::string(pName));
		if (var == m_NameTable.end())
		{
			LOG_DEBUG_ERROR("[Vulkan] Invalid name on ShaderVariable\n");
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
	

	void VKNShaderVariableTable::CommitResources()
	{
		//Varify all variables (Have the resources changed)
		size_t index = 0;
        bool writeDescriptors = false;
        for (auto& pVar : m_ShaderVariables)
        {
            //If validation failes we need to write the descriptors
			if (!pVar->Validate())
			{
				//Setup write info
				m_pVkDescriptors[index] = pVar->GetVkWriteDescriptorSet();
                writeDescriptors = true;
			}
            	
			index++;
        }


        //Get dynamic resources offsets
		uint32 dynamicOffsetIndex = 0;
		for (auto& pDynamicVar : m_DynamicVars)
		{
			//Update dynamic offsets
			m_pDynamicOffsets[dynamicOffsetIndex++] = pDynamicVar->GetDynamicOffset();
		}

        
		//Write descriptor set
		if (writeDescriptors)
		{
            //Allocate descriptorset
            m_VkDescriptorSet = m_PipelineState->AllocateDescriptorSet();
            
			//Setup the current set for writing
			for (uint32 i = 0; i < m_NumDescriptors; i++)
				m_pVkDescriptors[i].dstSet = m_VkDescriptorSet;

			//Write all descriptors
			vkUpdateDescriptorSets(m_pDevice->GetVkDevice(), m_NumDescriptors, m_pVkDescriptors, 0, nullptr);
		}
	}
}
