#pragma once
#include "Graphics/Core/IPipelineState.h"
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

	//-------
	//VKNSlot
	//-------

	struct VKNSlot
	{
		ShaderVariableDesc Slot;
		union
		{
			VKNBuffer* pBuffer;
			VKNTexture* pTexture;
			VKNSamplerState* pSamplerState;
		};
		union
		{
			VkDescriptorBufferInfo	BufferInfo;
			VkDescriptorImageInfo	ImageInfo;
		};
	};

	//------------------------
	//VKNPipelineState
	//------------------------

    class VKNPipelineState final : public RefCountedObject<IPipelineState>
    {
    public:
        LAMBDA_NO_COPY(VKNPipelineState);
        
        VKNPipelineState(VKNDevice* pDevice, const PipelineStateDesc& desc);
        ~VKNPipelineState();

		virtual void SetTextures(ITexture** ppTextures, uint32 numTextures, uint32 startSlot) override final;
		virtual void SetSamplerStates(ISamplerState** ppSamplerStates, uint32 numSamplerStates, uint32 startSlot) override final;
		virtual void SetConstantBuffers(IBuffer** ppBuffers, uint32 numBuffers, uint32 startSlot) override final;

        virtual void SetName(const char* pName) override final;
        virtual void* GetNativeHandle() const override final;

		void CommitBindings();

		inline VkPipeline GetVkPipeline() const { return m_Pipeline; }
		inline VkPipelineLayout GetVkPipelineLayout() const { return m_PipelineLayout; }
		inline const uint32* GetDynamicOffsets() const { return m_DynamicOffsets.data(); }
		inline uint32 GetDynamicOffsetCount() const { return uint32(m_DynamicOffsets.size()); };
		inline VkDescriptorSet GetVkDescriptorSet() const { return m_DescriptorSet; };
    private:
        void Init(const PipelineStateDesc& desc);
    private:
		VKNDevice*	m_pDevice;
		std::map<uint32, VKNSlot> m_ResourceBindings;
		std::vector<VkWriteDescriptorSet>   m_DescriptorWrites;
		std::vector<uint32>				    m_DynamicOffsets;
		std::vector<VKNBuffer*>				m_DynamicBuffers;
		VKNDescriptorSetAllocator* m_pAllocator;
        VkPipeline			  m_Pipeline;
		VkPipelineLayout	  m_PipelineLayout;
		VkDescriptorSetLayout m_DescriptorSetLayout;
		VkDescriptorSet		  m_DescriptorSet;
		VkDescriptorPool	  m_DescriptorPool;
		PipelineStateDesc	  m_Desc;
		bool m_IsDirty;
    };
}
