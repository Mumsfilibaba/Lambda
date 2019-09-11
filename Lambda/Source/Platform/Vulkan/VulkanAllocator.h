#pragma once
#include "Defines.h"
#include "Types.h"
#include <vector>
#include <vulkan/vulkan.h>

namespace Lambda
{
	class VulkanGraphicsDevice;


	struct VulkanAllocation
	{
		int32 ID = 0;
		uint64 Size = 0;
		uint64 Offset = 0;
		VkDeviceMemory Memory = VK_NULL_HANDLE;
	};


	class IVulkanAllocator
	{
	public:
		LAMBDA_INTERFACE(IVulkanAllocator);

		IVulkanAllocator() = default;
		~IVulkanAllocator() = default;

		virtual VulkanAllocation Allocate(uint64 sizeInBytes, uint64 alignment, uint64 memoryType) = 0;
		virtual void Map(const VulkanAllocation& allocation, void** ppMem) = 0;
		virtual void Unmap(const VulkanAllocation& allocation) = 0;
		virtual void Deallocate(const VulkanAllocation& allocation) = 0;
		virtual void Destroy(VkDevice device) = 0;
	};


	class VulkanMemoryChunk
	{
	public:
		LAMBDA_NO_COPY(VulkanMemoryChunk);

		VulkanMemoryChunk(VkDevice device, uint32 id, uint64 sizeInBytes, uint64 memoryType);
		~VulkanMemoryChunk() = default;

		bool CanAllocate(uint64 sizeInBytes, uint64 alignment);

		VulkanAllocation Allocate(uint64 sizeInBytes, uint64 alignment);
		void Map(VkDevice device, const VulkanAllocation& allocation, void** ppMem);
		void Deallocate(const VulkanAllocation& allocation);
		void Destroy(VkDevice device);

		uint32 GetMemoryType() const;

	private:
		void Init(VkDevice device);

	private:
		uint8* m_pMemory;
		VkDeviceMemory m_Memory;
		const uint32 m_ID;
		const uint32 m_MemoryType;
		const uint64 m_SizeInBytes;
		std::vector<VulkanAllocation> m_Blocks;
		bool m_IsMapped;
	};


	class VulkanDeviceAllocator final : public IVulkanAllocator
	{
	public:
		LAMBDA_NO_COPY(VulkanDeviceAllocator);

		VulkanDeviceAllocator(VkDevice device, VkPhysicalDevice physicalDevice);
		~VulkanDeviceAllocator() = default;

		virtual VulkanAllocation Allocate(uint64 sizeInBytes, uint64 alignment, uint64 memoryType) override final;
		virtual void Map(const VulkanAllocation& allocation, void** ppMem) override final;
		virtual void Unmap(const VulkanAllocation& allocation) override final;
		virtual void Deallocate(const VulkanAllocation& allocation) override final;
		virtual void Destroy(VkDevice device) override final;

	private:
		VkDevice m_Device;
		uint64 m_MaxAllocations;
		std::vector<VulkanMemoryChunk*> m_Chunks;
	};
}