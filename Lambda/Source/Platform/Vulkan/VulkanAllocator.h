#pragma once
#include "Defines.h"
#include "Types.h"
#include <vector>
#include <vulkan/vulkan.h>

namespace Lambda
{
	//------------
	//VulkanMemory
	//------------

	struct VulkanMemory
	{
		int32			ID = 0;
		uint64			Size = 0;
		uint64			Offset = 0;
		VkDeviceMemory	Memory = VK_NULL_HANDLE;
		uint8*			pMemory = nullptr;
	};

	//----------------
	//IVulkanAllocator
	//----------------

	class IVulkanAllocator
	{
	public:
		LAMBDA_INTERFACE(IVulkanAllocator);

		IVulkanAllocator() = default;
		~IVulkanAllocator() = default;

		virtual VulkanMemory Allocate(const VkMemoryRequirements& memoryRequirements, ResourceUsage usage) = 0;
		virtual void Deallocate(const VulkanMemory& allocation) = 0;
		virtual void DefferedDeallocate(const VulkanMemory& allocation, uint64 frameCount) = 0;
		virtual void CleanGarbageMemory(uint64 frameCount) = 0;
		virtual void Destroy(VkDevice device) = 0;
	};

	//-----------------
	//VulkanMemoryChunk
	//-----------------

	class VulkanMemoryChunk
	{
	public:
		LAMBDA_NO_COPY(VulkanMemoryChunk);

		VulkanMemoryChunk(uint32 id, uint64 sizeInBytes, uint64 memoryType, ResourceUsage usage);
		~VulkanMemoryChunk() = default;

		bool		 CanAllocate(uint64 sizeInBytes, uint64 alignment);
		VulkanMemory Allocate(uint64 sizeInBytes, uint64 alignment);
		void		 Deallocate(const VulkanMemory& allocation);
		void		 Destroy(VkDevice device);
		uint32		 GetMemoryType() const;

	private:
		void Init();
		void Map();
		void Unmap();

	private:
		uint8*						m_pMemory;
		VkDeviceMemory				m_Memory;
		const ResourceUsage			m_Usage;
		const uint32				m_ID;
		const uint32				m_MemoryType;
		const uint64				m_SizeInBytes;
		std::vector<VulkanMemory>	m_Blocks;
		bool						m_IsMapped;
	};

	//---------------------
	//VulkanDeviceAllocator
	//---------------------

	class VulkanDeviceAllocator final : public IVulkanAllocator
	{
	public:
		LAMBDA_NO_COPY(VulkanDeviceAllocator);

		VulkanDeviceAllocator();
		~VulkanDeviceAllocator() = default;

		virtual VulkanMemory Allocate(const VkMemoryRequirements& memoryRequirements, ResourceUsage usage) override final;
		virtual void Deallocate(const VulkanMemory& allocation) override final;
		virtual void DefferedDeallocate(const VulkanMemory& allocation, uint64 frameCount) override final;
		virtual void CleanGarbageMemory(uint64 frameCount) override final;
		virtual void Destroy(VkDevice device) override final;

	private:
		uint64									m_MaxAllocations;
		std::vector<VulkanMemoryChunk*>			m_Chunks;
		std::vector<std::vector<VulkanMemory>>	m_MemoryToDeallocate;
	};
}