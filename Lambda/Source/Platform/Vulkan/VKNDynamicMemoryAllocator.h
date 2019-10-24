#pragma once
#include "VKNDeviceAllocator.h"

namespace Lambda
{
	class VKNDynamicMemoryPage;
	struct VKNDynamicMemoryBlock;

	//--------------------
	//VKNDynamicAllocation
	//--------------------

	struct VKNDynamicAllocation
	{
		VKNDynamicMemoryBlock* pBlock	= nullptr;
		VkDeviceSize    BufferOffset	= 0;
		uint8*			pHostMemory		= nullptr;
		VkBuffer		Buffer			= VK_NULL_HANDLE;
	};

	//---------------------
	//VKNDynamicMemoryBlock
	//---------------------

	struct VKNDynamicMemoryBlock
	{
		VKNDynamicMemoryPage* pPage = nullptr;
		VKNDynamicMemoryBlock* pNext = nullptr;
		VKNDynamicMemoryBlock* pPrevious = nullptr;
		VkDeviceSize SizeInBytes = 0;
		VkDeviceSize BufferOffset = 0;
		bool         IsFree = true;
#if defined(LAMBDA_DEBUG)
		uint32       ID = 0;
#endif
	};

	//---------------------
	//VKNDynamicMemoryPage
	//---------------------

	class VKNDynamicMemoryPage
	{
	private:
		class BlockPool
		{
		public:
			BlockPool();
			~BlockPool();

			VKNDynamicMemoryBlock* Get();
			void Return(VKNDynamicMemoryBlock* pBlock);
		private:
			VKNDynamicMemoryBlock* AllocateChain();
		private:
			VKNDynamicMemoryBlock* m_pHead;
			std::vector<VKNDynamicMemoryBlock*> m_Chains;
		};

	public:
		LAMBDA_NO_COPY(VKNDynamicMemoryPage);

		VKNDynamicMemoryPage(VKNDevice* pDevice, uint32 id, VkDeviceSize sizeInBytes);
		~VKNDynamicMemoryPage() = default;

		bool Allocate(VKNDynamicAllocation& allocation, VkDeviceSize sizeInBytes, VkDeviceSize alignment);
		void Deallocate(VKNDynamicAllocation& allocation);
		void Destroy(VKNDevice* pDevice);
	private:
		void Init(VKNDevice* pDevice);
	private:
		BlockPool m_BlockPool;
		VKNDynamicMemoryBlock* m_pHead;
		VKNDynamicMemoryBlock* m_pNextFree;
		VkBuffer m_Buffer;
		VKNAllocation m_Memory;
		const uint32 m_ID;
		const uint64 m_SizeInBytes;
		uint32 m_BlockCount;
	};

	//-------------------------
	//VKNDynamicMemoryAllocator
	//-------------------------

	class VKNDynamicMemoryAllocator final
	{
	public:
		LAMBDA_NO_COPY(VKNDynamicMemoryAllocator);

		VKNDynamicMemoryAllocator(VKNDevice* pDevice);
		~VKNDynamicMemoryAllocator();

		bool Allocate(VKNDynamicAllocation& allocation, uint64 sizeInBytes, uint64 alignment);
		void Deallocate(VKNDynamicAllocation& allocation);
		void EmptyGarbageMemory();

		inline uint64 GetTotalReserved() const	{ return m_TotalReserved; }
		inline uint64 GetTotalAllocated() const { return m_TotalAllocated; }
	private:
		VKNDevice* m_pDevice;
		uint64 m_FrameIndex;
		VKNDynamicMemoryPage* m_pCurrentPage;
		std::vector<VKNDynamicMemoryPage*> m_Pages;
		std::vector<std::vector<VKNDynamicAllocation>> m_MemoryToDeallocate;
		uint64 m_TotalReserved;
		uint64 m_TotalAllocated;
	};
}