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
	public:
		VKNDynamicAllocation& operator=(const VKNDynamicAllocation& other)
		{
			memcpy(this, &other, sizeof(VKNDynamicAllocation));
			return *this;
		}

	public:
		VKNDynamicMemoryBlock* pBlock	= nullptr;
		uint8*			pHostMemory		= nullptr;
		VkBuffer		Buffer			= VK_NULL_HANDLE;
		VkDeviceSize    BufferOffset	= 0;
	};

	//---------------------
	//VKNDynamicMemoryBlock
	//---------------------

	struct VKNDynamicMemoryBlock
	{
		VKNDynamicMemoryPage*	pPage		= nullptr;
		VKNDynamicMemoryBlock*	pNext		= nullptr;
		VKNDynamicMemoryBlock*  pPrevious	= nullptr;
		VkDeviceSize SizeInBytes			= 0;
		VkDeviceSize BufferOffset			= 0;
#if defined(LAMBDA_DEBUG)
		uint32       ID						= 0;
#endif
		bool         IsFree					= true;
	};

	//---------------------
	//VKNDynamicMemoryPage
	//---------------------

	class VKNDynamicMemoryPage
	{
	private:

		//------------
		//VKNBlockPool
		//------------

		class VKNBlockPool
		{
		public:
			VKNBlockPool(uint32 numBlocks);
			~VKNBlockPool();

			VKNDynamicMemoryBlock* Get();
			void Return(VKNDynamicMemoryBlock* pBlock);
		private:
			VKNDynamicMemoryBlock* AllocateBlocks(uint32 numBlocks);
		private:
			std::vector<VKNDynamicMemoryBlock*> m_Chains;
			VKNDynamicMemoryBlock* m_pHead;
		};

	public:
		LAMBDA_NO_COPY(VKNDynamicMemoryPage);

		VKNDynamicMemoryPage(VKNDevice* pVkDevice, uint32 id, VkDeviceSize sizeInBytes);
		~VKNDynamicMemoryPage() = default;

		bool Allocate(VKNDynamicAllocation& allocation, VkDeviceSize sizeInBytes, VkDeviceSize alignment);
		void Deallocate(VKNDynamicAllocation& allocation);
		void Destroy(VKNDevice* pVkDevice);


		_forceinline bool IsEmpty() const
		{
			return m_pHead->pPrevious == nullptr && m_pHead->pNext == nullptr && m_pHead->IsFree;
		}


		_forceinline uint64 GetSize() const
		{
			return m_SizeInBytes;
		}
	private:
		void Init(VKNDevice* pVkDevice);
	private:
		VKNDynamicMemoryBlock* m_pHead;
		VKNDynamicMemoryBlock* m_pNextFree;
		VKNBlockPool m_VkBlockPool;
		VKNAllocation m_Memory;
		const uint64 m_SizeInBytes;
		VkBuffer m_VkBuffer;
		const uint32 m_ID;
		uint32 m_BlockCount;
	};

	//-------------------------
	//VKNDynamicMemoryAllocator
	//-------------------------

	class VKNDynamicMemoryAllocator final
	{
	public:
		LAMBDA_NO_COPY(VKNDynamicMemoryAllocator);

		VKNDynamicMemoryAllocator(VKNDevice* pVkDevice);
		~VKNDynamicMemoryAllocator();

		bool Allocate(VKNDynamicAllocation& allocation, uint64 sizeInBytes, uint64 alignment);
		void Deallocate(VKNDynamicAllocation& allocation);
		void EmptyGarbageMemory();


		_forceinline uint64 GetTotalReserved() const	
		{ 
			return m_TotalReserved; 
		}
		
		
		_forceinline uint64 GetTotalAllocated() const 
		{ 
			return m_TotalAllocated; 
		}
	private:
		std::vector<VKNDynamicMemoryPage*> m_Pages;
        std::vector<VKNDynamicMemoryPage*> m_EmptyPages;
		std::vector<std::vector<VKNDynamicAllocation>> m_MemoryToDeallocate;
		VKNDevice* m_pVkDevice;
		VKNDynamicMemoryPage* m_pCurrentPage;
		uint64 m_FrameIndex;
		uint64 m_TotalReserved;
		uint64 m_TotalAllocated;
	};
}
