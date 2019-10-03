#include "LambdaPch.h"
#include "VKNShaderVariableTable.h"
#include "VKNDevice.h"
#include "VKNAllocator.h"
#include "VKNConversions.inl"

namespace Lambda
{
	//----------------------
	//VKNShaderVariableTable
	//----------------------

	VKNShaderVariableTable::VKNShaderVariableTable(VKNDevice* pDevice, VKNPipelineState* pPipelineState, const ShaderVariableTableDesc& desc)
		: DeviceObjectBase<VKNDevice, IShaderVariableTable>(pDevice),
		m_PipelineState(nullptr),
		m_DescriptorSet(VK_NULL_HANDLE),
		m_IsDirty(true)
	{
		this->AddRef();

		LAMBDA_ASSERT(pPipelineState != nullptr);
		
		pPipelineState->AddRef();
		m_PipelineState = pPipelineState;

		Init(desc);
	}
	

	VKNShaderVariableTable::~VKNShaderVariableTable()
	{
		SafeDeleteArr(m_pDescriptorWrites);
		SafeDeleteArr(m_pDynamicOffsets);

		LOG_DEBUG_INFO("Vulkan: Destroyed VKNShaderVariableTable\n");
	}


	void VKNShaderVariableTable::Init(const ShaderVariableTableDesc& desc)
	{
		//Create variables
		uint32 numDynamicBuffers = 0;
		//Allocate writes
		m_pDescriptorWrites = DBG_NEW VkWriteDescriptorSet[desc.NumVariables];
		for (uint32 i = 0; i < desc.NumVariables; i++)
		{
			//Create new variable
			VKNShaderVariable* pVariable = DBG_NEW VKNShaderVariable(m_pDevice, this, desc.pVariables[i]);
			m_ShaderVariables.emplace_back(pVariable);

			//Save the variable in a map so we can find it easy by name
			m_NameTable[std::string(desc.pVariables[i].pName)] = pVariable;

			//Setup writinginfos
			VkWriteDescriptorSet& writeInfo = m_pDescriptorWrites[i];
			writeInfo.sType				= VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			writeInfo.pNext				= nullptr;
			writeInfo.dstArrayElement	= 0;
			writeInfo.dstSet			= m_DescriptorSet;
			writeInfo.pTexelBufferView	= nullptr;
			writeInfo.descriptorCount	= 1;

			const ShaderVariableDesc& varDesc = desc.pVariables[i];
			writeInfo.dstBinding	 = varDesc.Slot;
			writeInfo.descriptorType = ConvertResourceToDescriptorType(varDesc.Type);
			if (writeInfo.descriptorType == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER && varDesc.Usage == RESOURCE_USAGE_DYNAMIC)
				writeInfo.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
			else if (writeInfo.descriptorType == VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE && varDesc.pSamplerState != nullptr)
				writeInfo.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;

			//Get the info about the resource
			if (varDesc.Type == RESOURCE_TYPE_TEXTURE || varDesc.Type == RESOURCE_TYPE_SAMPLER_STATE)
				writeInfo.pImageInfo = pVariable->GetVkDescriptorImageInfo();
			else
				writeInfo.pBufferInfo = pVariable->GetVkDescriptorBufferInfo();

			//Count dynamic buffers
			if (desc.pVariables[i].Usage == RESOURCE_USAGE_DYNAMIC && desc.pVariables[i].Type == RESOURCE_TYPE_CONSTANT_BUFFER)
				numDynamicBuffers++;
		}

		//Allocate descriptorset
		VkDescriptorSetLayout descriptorSetLayout = m_PipelineState->GetVkDescriptorSetLayout();
		m_DescriptorSet = m_PipelineState->GetAllocator()->Allocate(descriptorSetLayout);

		//Dynamic offsets for dynamic constantbuffers
		m_pDynamicOffsets = DBG_NEW uint32[numDynamicBuffers];
		m_NumDynamicOffsets = numDynamicBuffers;
	}


	IShaderVariable* VKNShaderVariableTable::GetVariableByName(ShaderStage shader, const char* pName)
	{
		auto var = m_NameTable.find(std::string(pName));
		return var != m_NameTable.end() ? var->second : nullptr;
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
	
	
	void VKNShaderVariableTable::CommitAndTransitionResources()
	{
		//Varify all variables (Have their bindings changed)
		uint32 index = 0;
		for (auto& var : m_ShaderVariables)
		{
			var->Verify();

			//Get dynamic offset
			const ShaderVariableDesc& varDesc = var->GetDesc();
			if (varDesc.Type == RESOURCE_TYPE_CONSTANT_BUFFER)
			{
				if (varDesc.Usage == RESOURCE_USAGE_DYNAMIC)
				{
					m_pDynamicOffsets[index++] = var->GetDynamicOffset();
				}
			}
		}

		//Write descriptor set
		if (m_IsDirty)
		{
			//Setup the current set for writing
			for (uint32 i = 0; i < uint32(m_ShaderVariables.size()); i++)
				m_pDescriptorWrites[i].dstSet = m_DescriptorSet;

			//Write all descriptors
			vkUpdateDescriptorSets(m_pDevice->GetVkDevice(), uint32(m_ShaderVariables.size()), m_pDescriptorWrites, 0, nullptr);

			//Do not update again
			m_IsDirty = false;
		}
	}
}