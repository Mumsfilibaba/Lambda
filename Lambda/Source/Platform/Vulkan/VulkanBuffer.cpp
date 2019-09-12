#include "LambdaPch.h"
#include "Utilities/MathHelper.h"
#include "VulkanBuffer.h"
#include "VulkanUtilities.h"
#include "VulkanGraphicsDevice.h"

namespace Lambda
{
	//------------
	//VulkanBuffer
	//------------

    VulkanBuffer::VulkanBuffer(VkDevice device, IVulkanAllocator* pAllocator, const BufferDesc& desc)
		: m_pAllocator(pAllocator),
		m_Buffer(VK_NULL_HANDLE),
		m_Memory(),
		m_Desc(),
		m_CurrentFrame(0),
		m_FrameCount(0),
		m_DynamicOffset(0),
        m_DynamicAlignment(0)
    {
		LAMBDA_ASSERT(pAllocator != nullptr);
		LAMBDA_ASSERT(device != VK_NULL_HANDLE);
        Init(device, desc);
    }
    
    
    void VulkanBuffer::Init(VkDevice device, const BufferDesc& desc)
    {
        VkBufferCreateInfo info = {};
        info.sType					= VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        info.pNext					= nullptr;
        info.flags					= 0;
        info.queueFamilyIndexCount	= 0;
        info.pQueueFamilyIndices	= nullptr;
        info.sharingMode			= VK_SHARING_MODE_EXCLUSIVE;

		//If dynamic we allocate extra size so we can use dynamic offsets
		if (desc.Usage == RESOURCE_USAGE_DYNAMIC)
		{
			DeviceLimits limits = VulkanGraphicsDevice::GetInstance().GetDeviceLimits();
			m_DynamicAlignment = limits.UniformBufferAlignment;

			DeviceSettings settings = VulkanGraphicsDevice::GetInstance().GetDeviceSettings();
			m_FrameCount = settings.FramesAhead;
			
			//If dynamic we allocate extra space (1024 updates per frame)
			info.size = uint64(desc.SizeInBytes) * 1024 * m_FrameCount;		
		}
		else
		{
			info.size = desc.SizeInBytes;
		}
        
		//Set usage
		info.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
		if (desc.Flags & BUFFER_FLAGS_VERTEX_BUFFER)
		{
            info.usage |= VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
		}
		if (desc.Flags & BUFFER_FLAGS_INDEX_BUFFER)
		{
            info.usage |= VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
		}
		if (desc.Flags & BUFFER_FLAGS_CONSTANT_BUFFER)
		{
            info.usage |= VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
		}
        
		//Create buffer
		if (vkCreateBuffer(device, &info, nullptr, &m_Buffer) != VK_SUCCESS)
        {
            LOG_DEBUG_ERROR("Vulkan: Failed to create Buffer\n");
            return;
        }
        else
        {
            LOG_DEBUG_INFO("Vulkan: Created Buffer\n");
            m_Desc = desc;
        }
        
		//Allocate memory
		VkMemoryRequirements memoryRequirements = {};
		vkGetBufferMemoryRequirements(device, m_Buffer, &memoryRequirements);

		m_Memory = m_pAllocator->Allocate(memoryRequirements, desc.Usage);
        if (m_Memory.Memory != VK_NULL_HANDLE)
		{
            vkBindBufferMemory(device, m_Buffer, m_Memory.Memory, m_Memory.Offset);
        }
    }
    
    
    void VulkanBuffer::Map(void** ppMem)
    {
		LAMBDA_ASSERT(ppMem != nullptr);
		(*ppMem) = m_Memory.pMemory + m_DynamicOffset;
    }
    
    
    void VulkanBuffer::Unmap()
    {
		//Unmapping a vulkan buffer does nothing since a whole memoryblock is mapped and stays mapped during its lifetime.
    }
    
    
    void* VulkanBuffer::GetNativeHandle() const
    {
        return reinterpret_cast<void*>(m_Buffer);
    }

    
    BufferDesc VulkanBuffer::GetDesc() const
    {
        return m_Desc;
    }


	void VulkanBuffer::AdvanceFrame()
	{
		m_CurrentFrame = (m_CurrentFrame + 1) % m_FrameCount;
	}


	void VulkanBuffer::DynamicUpdate(const ResourceData* pData)
	{
		//Calculate offset in buffer
		uint32 sizeInBytes = m_Desc.SizeInBytes * 1024;		//Total size of buffer
		uint32 frameOffset = m_CurrentFrame * sizeInBytes;	//Offset of the current frame
		uint32 dynamicOffset = Math::AlignUp<uint32>(m_DynamicOffset + m_Desc.SizeInBytes, m_DynamicAlignment) % sizeInBytes; //Ringbuffer-offset per frame
		m_DynamicOffset = frameOffset + dynamicOffset;

		//Update buffer
		uint8* pCurrent = m_Memory.pMemory + m_DynamicOffset;
		memcpy(pCurrent, pData->pData, pData->SizeInBytes);
	}


	uint32 VulkanBuffer::GetDynamicOffset() const
	{
		return m_DynamicOffset;
	}
    
    
    void VulkanBuffer::Release(VkDevice device)
    {
		LAMBDA_ASSERT(device != VK_NULL_HANDLE);
        
        if (m_Buffer != VK_NULL_HANDLE)
        {
			m_pAllocator->Deallocate(m_Memory);

            vkDestroyBuffer(device, m_Buffer, nullptr);
            m_Buffer = VK_NULL_HANDLE;
        }
    }
    
    
    void VulkanBuffer::Destroy(VkDevice device)
    {
        Release(device);
        delete this;
    }

	//--------------------------
	//VulkanDynamicBufferManager
	//--------------------------

	VulkanDynamicBufferManager* VulkanDynamicBufferManager::s_pInstance = nullptr;

	VulkanDynamicBufferManager::VulkanDynamicBufferManager()
		: m_Buffers()
	{
		LAMBDA_ASSERT(s_pInstance == nullptr);
		s_pInstance = this;
	}


	VulkanDynamicBufferManager::~VulkanDynamicBufferManager()
	{
		if (s_pInstance == this)
			s_pInstance = nullptr;
	}


	void VulkanDynamicBufferManager::MoveToNextFrame()
	{
		for (auto buffer : m_Buffers)
			buffer->AdvanceFrame();
	}


	void VulkanDynamicBufferManager::RegisterBuffer(VulkanBuffer* pBuffer)
	{
		for (auto buffer : m_Buffers)
		{
			if (buffer == pBuffer)
			{
				return;
			}
		}

		LOG_DEBUG_INFO("Vulkan: Registered buffer\n");

		m_Buffers.emplace_back(pBuffer);
	}


	void VulkanDynamicBufferManager::UnregisterBuffer(VulkanBuffer* pBuffer)
	{
		for (auto i = m_Buffers.begin(); i < m_Buffers.end(); i++)
		{
			if (*i == pBuffer)
			{
				LOG_DEBUG_INFO("Vulkan: Unregistered buffer\n");

				m_Buffers.erase(i);
				return;
			}
		}
	}
	
	
	VulkanDynamicBufferManager& VulkanDynamicBufferManager::GetInstance()
	{
		LAMBDA_ASSERT(s_pInstance != nullptr);
		return *s_pInstance;
	}
}
