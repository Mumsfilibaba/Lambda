#include "LambdaPch.h"
#include "VKNUploadBuffer.h"
#include "VKNDevice.h"
#include "VKNBuffer.h"
#include "VKNUtilities.h"

namespace Lambda
{    
	//---------------
	//VKNUploadBuffer
	//---------------

    VKNUploadBuffer::VKNUploadBuffer(IVKNAllocator* pAllocator, uint64 sizeInBytes)
        : m_pAllocator(pAllocator),
        m_pCurrent(nullptr),
        m_Buffer(VK_NULL_HANDLE),
        m_Memory(),
        m_SizeInBytes(0),
		m_BytesLeft(0)
    {
		LAMBDA_ASSERT(pAllocator != nullptr);
        Init(sizeInBytes);
    }

    
    bool VKNUploadBuffer::Init(uint64 sizeInBytes)
    {
		//Create buffer
        VkBufferCreateInfo info = {};
        info.sType                  = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        info.pNext                  = nullptr;
        info.flags                  = 0;
        info.queueFamilyIndexCount  = 0;
        info.pQueueFamilyIndices    = nullptr;
        info.sharingMode            = VK_SHARING_MODE_EXCLUSIVE;
        info.size                   = sizeInBytes;
        info.usage                  = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
        
		VKNDevice& device = VKNDevice::Get();
        if (vkCreateBuffer(device.GetDevice(), &info, nullptr, &m_Buffer) != VK_SUCCESS)
        {
            LOG_DEBUG_ERROR("Vulkan: Failed to create buffer for UploadBuffer\n");
            return false;
        }
        else
        {
            LOG_DEBUG_INFO("Vulkan: Created buffer for UploadBuffer\n");
            m_SizeInBytes = m_BytesLeft = sizeInBytes;
        }
        
        
		//Allocate memory
		VkMemoryRequirements memoryRequirements = {};
		vkGetBufferMemoryRequirements(device.GetDevice(), m_Buffer, &memoryRequirements);
		if (m_pAllocator->Allocate(&m_Memory, memoryRequirements, RESOURCE_USAGE_DYNAMIC))
		{
			vkBindBufferMemory(device.GetDevice(), m_Buffer, m_Memory.DeviceMemory, m_Memory.DeviceMemoryOffset);
            Reset();
            return true;
		}
		else
		{
			return false;
		}
    }


	void VKNUploadBuffer::Reallocate(uint64 sizeInBytes)
	{
		VkBuffer	newBuffer = VK_NULL_HANDLE;
		VKNMemory	newMemory = {};

		//Create buffer
		VkBufferCreateInfo info = {};
		info.sType					= VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		info.pNext					= nullptr;
		info.flags					= 0;
		info.queueFamilyIndexCount	= 0;
		info.pQueueFamilyIndices	= nullptr;
		info.sharingMode			= VK_SHARING_MODE_EXCLUSIVE;
		info.size					= sizeInBytes;
		info.usage					= VK_BUFFER_USAGE_TRANSFER_SRC_BIT;


		VKNDevice& device = VKNDevice::Get();
		if (vkCreateBuffer(device.GetDevice(), &info, nullptr, &newBuffer) != VK_SUCCESS)
		{
			LOG_DEBUG_ERROR("Vulkan: Failed to reallocate buffer for UploadBuffer\n");
		}
		else
		{
			LOG_DEBUG_WARNING("Vulkan: Reallocated buffer for UploadBuffer\n");
		}


		//Allocate memory
		VkMemoryRequirements memoryRequirements = {};
		vkGetBufferMemoryRequirements(device.GetDevice(), newBuffer, &memoryRequirements);
		if (m_pAllocator->Allocate(&newMemory, memoryRequirements, RESOURCE_USAGE_DYNAMIC))
		{
			vkBindBufferMemory(device.GetDevice(), newBuffer, newMemory.DeviceMemory, newMemory.DeviceMemoryOffset);
			
			//Reset (Not calling Reset() since that is only valid when not in flight on GPU)
			m_SizeInBytes	= m_BytesLeft = sizeInBytes;
			m_pCurrent		= newMemory.pHostMemory;

			//DeallocatePool old buffer and set them to the new handles
			m_OldBuffers.emplace_back(m_Buffer);
			m_OldMemory.emplace_back(m_Memory);

			m_Buffer = newBuffer;
			m_Memory = newMemory;
		}
	}
    
    
    void* VKNUploadBuffer::Allocate(uint64 bytesToAllocate)
    {
        //Do we have enough space? If no reallocate
        if (bytesToAllocate > m_BytesLeft)
        {
			Reallocate(bytesToAllocate + MB(1));
        }

		//Move pointer
		void* pAllocation = reinterpret_cast<void*>(m_pCurrent);
		m_pCurrent	+= bytesToAllocate;
		m_BytesLeft -= bytesToAllocate;

		return pAllocation;
    }
    
    
    void VKNUploadBuffer::Reset()
    {
        //Reset buffer by resetting current to the start
        m_pCurrent	= m_Memory.pHostMemory;
		m_BytesLeft = m_SizeInBytes;

		//Destroy old buffers
		if (m_OldBuffers.size() > 0)
		{
			VKNDevice& device = VKNDevice::Get();
			for (auto& buffer : m_OldBuffers)
			{
				vkDestroyBuffer(device.GetDevice(), buffer, nullptr);
				buffer = VK_NULL_HANDLE;
			}
			m_OldBuffers.clear();
        }
    }
    
    
    void VKNUploadBuffer::Destroy(VkDevice device)
    {
		LAMBDA_ASSERT(device != VK_NULL_HANDLE);
        
		Reset();
        
        //Deallocate old memory
        if (m_OldMemory.size() > 0)
        {
            for (auto& mem : m_OldMemory)
            {
                m_pAllocator->Deallocate(&mem);
            }
            m_OldMemory.clear();
        }

        if (m_Buffer != VK_NULL_HANDLE)
        {
			m_pAllocator->Deallocate(&m_Memory);

            vkDestroyBuffer(device, m_Buffer, nullptr);
            m_Buffer = VK_NULL_HANDLE;
        }
        
        LOG_DEBUG_INFO("Vulkan: Destroyed UploadBuffer\n");
		delete this;
    }
}
