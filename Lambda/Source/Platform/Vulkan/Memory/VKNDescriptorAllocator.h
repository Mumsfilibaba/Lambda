#pragma once
#include "Defines.h"
#include "Types.h"
#include <vector>
#include "../Vulkan.h"

namespace Lambda
{
	class VKNDevice;

	//-------------------------
	//VKNDescriptorSetAllocator
	//-------------------------

	class VKNDescriptorSetAllocator
	{
	public:
		LAMBDA_NO_COPY(VKNDescriptorSetAllocator);

		VKNDescriptorSetAllocator(VKNDevice* m_pDevice, uint32 uniformBufferCount, uint32 dynamicUniformBufferCount, uint32 samplerCount, uint32 sampledImageCount, uint32 combinedImageSamplerCount, uint32 numSets);
		~VKNDescriptorSetAllocator() = default;

		VkDescriptorSet Allocate(VkDescriptorSetLayout descriptorSetLayout);
		void Destroy(VkDevice device);
	private:
		void Init();
	private:
		VKNDevice* m_pDevice;
		VkDescriptorPool m_Pool;
		uint32 m_NumSets;
		uint32 m_UniformBufferCount;
		uint32 m_DynamicUniformBufferCount;
		uint32 m_SamplerCount;
		uint32 m_SampledImageCount;
		uint32 m_CombinedImageSamplerCount;
		uint32 m_SetCount;
	};
}