#pragma once
#include "Defines.h"
#include "Types.h"
#include <vector>
#include <vulkan/vulkan.h>

namespace Lambda
{
	class VulkanGraphicsDevice;


	struct VulkanMemory
	{
		int32			ID = 0;
		uint64			Size = 0;
		uint64			Offset = 0;
		VkDeviceMemory	Memory = VK_NULL_HANDLE;
		uint8*			pMemory = nullptr;
	};


	class IVulkanAllocator
	{
	public:
		LAMBDA_INTERFACE(IVulkanAllocator);

		IVulkanAllocator() = default;
		~IVulkanAllocator() = default;

		virtual VulkanMemory Allocate(const VkMemoryRequirements& memoryRequirements, ResourceUsage usage) = 0;
		virtual void Deallocate(const VulkanMemory& allocation) = 0;
		virtual void Destroy(VkDevice device) = 0;
	};


	class VulkanMemoryChunk
	{
	public:
		LAMBDA_NO_COPY(VulkanMemoryChunk);

		VulkanMemoryChunk(VkDevice device, uint32 id, uint64 sizeInBytes, uint64 memoryType, ResourceUsage usage);
		~VulkanMemoryChunk() = default;

		bool CanAllocate(uint64 sizeInBytes, uint64 alignment);

		VulkanMemory Allocate(uint64 sizeInBytes, uint64 alignment);
		void Deallocate(const VulkanMemory& allocation);
		void Destroy(VkDevice device);

		uint32 GetMemoryType() const;

	private:
		void Init(VkDevice device);
		void Map(VkDevice device);
		void Unmap(VkDevice device);

	private:
		uint8* m_pMemory;
		VkDeviceMemory m_Memory;
		const ResourceUsage m_Usage;
		const uint32 m_ID;
		const uint32 m_MemoryType;
		const uint64 m_SizeInBytes;
		std::vector<VulkanMemory> m_Blocks;
		bool m_IsMapped;
	};


	class VulkanDeviceAllocator final : public IVulkanAllocator
	{
	public:
		LAMBDA_NO_COPY(VulkanDeviceAllocator);

		VulkanDeviceAllocator(VkDevice device, VkPhysicalDevice physicalDevice);
		~VulkanDeviceAllocator() = default;

		virtual VulkanMemory Allocate(const VkMemoryRequirements& memoryRequirements, ResourceUsage usage) override final;
		virtual void Deallocate(const VulkanMemory& allocation) override final;
		virtual void Destroy(VkDevice device) override final;

	private:
		VkDevice m_Device;
		VkPhysicalDevice m_PhysicalDevice;
		uint64 m_MaxAllocations;
		std::vector<VulkanMemoryChunk*> m_Chunks;
	};
}