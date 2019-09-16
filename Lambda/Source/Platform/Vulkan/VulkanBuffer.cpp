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

    VulkanBuffer::VulkanBuffer(IVulkanAllocator* pAllocator, const BufferDesc& desc)
		: m_pAllocator(pAllocator),
		m_Buffer(VK_NULL_HANDLE),
		m_Memory(),
		m_Desc(),
		m_CurrentFrame(0),
		m_SizePerFrame(0),
		m_SizePerUpdate(0),
		m_DynamicOffset(0),
		m_TotalDynamicOffset(0),
		m_IsDirty(false)
    {
		LAMBDA_ASSERT(pAllocator != nullptr);
        Init(desc);
    }
    
    
    void VulkanBuffer::Init(const BufferDesc& desc)
    {
        VkBufferCreateInfo info = {};
        info.sType					= VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        info.pNext					= nullptr;
        info.flags					= 0;
        info.queueFamilyIndexCount	= 0;
        info.pQueueFamilyIndices	= nullptr;
        info.sharingMode			= VK_SHARING_MODE_EXCLUSIVE;

		//If dynamic we allocate extra size so we can use dynamic offsets
        VulkanGraphicsDevice& device = VulkanGraphicsDevice::GetInstance();
		if (desc.Usage == RESOURCE_USAGE_DYNAMIC)
		{
            DeviceSettings settings                  = device.GetDeviceSettings();
            VkPhysicalDeviceProperties properties    = device.GetPhysicalDeviceProperties();

			//If dynamic we allocate extra space (NUM_UPDATES updates per frame)
			constexpr uint32 numUpdatesPerFrame = 16;
			m_SizePerUpdate = Math::AlignUp<uint32>(uint32(desc.SizeInBytes), properties.limits.minUniformBufferOffsetAlignment);
			m_SizePerFrame	= m_SizePerUpdate * numUpdatesPerFrame;
            info.size		= VkDeviceSize(m_SizePerFrame * settings.FramesAhead);
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
		if (vkCreateBuffer(device.GetDevice(), &info, nullptr, &m_Buffer) != VK_SUCCESS)
        {
            LOG_DEBUG_ERROR("Vulkan: Failed to create Buffer\n");
            return;
        }
        else
        {
            LOG_DEBUG_INFO("Vulkan: Created Buffer\n");
            m_Desc = desc;
            
            device.SetVulkanObjectName(VK_OBJECT_TYPE_BUFFER, uint64(m_Buffer), std::string(m_Desc.pName));
        }
        
		//Allocate memory
		VkMemoryRequirements memoryRequirements = {};
		vkGetBufferMemoryRequirements(device.GetDevice(), m_Buffer, &memoryRequirements);
        if (m_pAllocator->Allocate(&m_Memory, memoryRequirements, m_Desc.Usage))
		{
            vkBindBufferMemory(device.GetDevice(), m_Buffer, m_Memory.DeviceMemory, m_Memory.DeviceMemoryOffset);
        }
    }
    
    
    void VulkanBuffer::Map(void** ppMem)
    {
		LAMBDA_ASSERT(ppMem != nullptr);
        (*ppMem) = m_Memory.pHostMemory + m_TotalDynamicOffset;
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


	bool VulkanBuffer::IsDirty() const
	{
		return m_IsDirty;
	}


	void VulkanBuffer::SetIsClean()
	{
		m_IsDirty = false;
	}


	void VulkanBuffer::AdvanceFrame(uint32 frameCount)
	{
		//Move on a frame
		m_CurrentFrame	= frameCount;
		//Reset offset
		m_DynamicOffset      = 0;
        m_TotalDynamicOffset = 0;
	}


	void VulkanBuffer::DynamicUpdate(const ResourceData* pData)
	{
		//Calculate offset and see if there are enough space
		uint32 offset = m_DynamicOffset + m_SizePerUpdate;
		if (offset >= m_SizePerFrame)
		{
			Reallocate(m_SizePerFrame * 2);
			offset = 0;
		}

		//Calculate offset in buffer
		uint32 frameOffset		= m_CurrentFrame * m_SizePerFrame;	//Offset of the current frame
		m_DynamicOffset			= (offset) % m_SizePerFrame;		//Ringbuffer-offset per frame
		m_TotalDynamicOffset	= frameOffset + m_DynamicOffset;						
        
		//Update buffer
		uint8* pCurrent = m_Memory.pHostMemory + m_TotalDynamicOffset;
		memcpy(pCurrent, pData->pData, pData->SizeInBytes);
	}


	void VulkanBuffer::Reallocate(uint32 sizeInBytes)
	{
		VkBufferCreateInfo info = {};
		info.sType					= VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		info.pNext					= nullptr;
		info.flags					= 0;
		info.queueFamilyIndexCount	= 0;
		info.pQueueFamilyIndices	= nullptr;
		info.sharingMode			= VK_SHARING_MODE_EXCLUSIVE;

        VulkanGraphicsDevice& device    = VulkanGraphicsDevice::GetInstance();
        GraphicsDeviceDesc desc         = device.GetDesc();
        
        //Set new size
        m_SizePerFrame       = sizeInBytes;
        m_DynamicOffset      = 0;
        m_TotalDynamicOffset = 0;
        info.size            = VkDeviceSize(sizeInBytes * desc.BackBufferCount);

        LOG_DEBUG_WARNING("Vulkan: Reallocated buffer. Old size: %llu bytes, New size: %llu\n", m_Memory.Size, info.size);
        
        //Set usage
        info.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
        if (m_Desc.Flags & BUFFER_FLAGS_VERTEX_BUFFER)
        {
            info.usage |= VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
        }
        if (m_Desc.Flags & BUFFER_FLAGS_INDEX_BUFFER)
        {
            info.usage |= VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
        }
        if (m_Desc.Flags & BUFFER_FLAGS_CONSTANT_BUFFER)
        {
            info.usage |= VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
        }

        //Create buffer
        VkBuffer newBuffer = VK_NULL_HANDLE;
		if (vkCreateBuffer(device.GetDevice(), &info, nullptr, &newBuffer) != VK_SUCCESS)
		{
			LOG_DEBUG_ERROR("Vulkan: Failed to create Buffer\n");
			return;
		}
		else
		{
			LOG_DEBUG_INFO("Vulkan: Created Buffer\n");
            device.SetVulkanObjectName(VK_OBJECT_TYPE_BUFFER, uint64(m_Buffer), std::string(m_Desc.pName));
		}

		//Allocate memory
		VkMemoryRequirements memoryRequirements = {};
		vkGetBufferMemoryRequirements(device.GetDevice(), newBuffer, &memoryRequirements);

        VulkanMemory newMemory = {};
		if (m_pAllocator->Allocate(&newMemory, memoryRequirements, m_Desc.Usage))
		{
			vkBindBufferMemory(device.GetDevice(), newBuffer, newMemory.DeviceMemory, newMemory.DeviceMemoryOffset);
		}
		else
		{
			return;
		}

		//Set the new handles and delete the old ones
		m_pAllocator->DefferedDeallocate(&m_Memory, m_CurrentFrame);
        
        VulkanDynamicBufferManager& bufferManager = VulkanDynamicBufferManager::GetInstance();
        bufferManager.DestroyBuffer(m_Buffer, m_CurrentFrame);
        
		m_Buffer = newBuffer;
		m_Memory = newMemory;

		//Set to dirty
		m_IsDirty = true;
	}


	uint32 VulkanBuffer::GetDynamicOffset() const
	{
		return m_TotalDynamicOffset;
	}
    
    
    void VulkanBuffer::Release(VkDevice device)
    {
		LAMBDA_ASSERT(device != VK_NULL_HANDLE);
        
        if (m_Buffer != VK_NULL_HANDLE)
        {
			m_pAllocator->Deallocate(&m_Memory);

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
        
        VulkanGraphicsDevice& device = VulkanGraphicsDevice::GetInstance();
        GraphicsDeviceDesc desc = device.GetDesc();
        
        m_BuffersToDelete.resize(desc.BackBufferCount);
	}


	VulkanDynamicBufferManager::~VulkanDynamicBufferManager()
	{
		if (s_pInstance == this)
			s_pInstance = nullptr;
	}
    
    
    void VulkanDynamicBufferManager::DestroyBuffer(VkBuffer buffer, uint32 frameCount)
    {
        if (buffer != VK_NULL_HANDLE)
        {
            auto& buffers = m_BuffersToDelete[frameCount];
            buffers.push_back(buffer);
        }
    }


	void VulkanDynamicBufferManager::MoveToNextFrame(uint32 frameCount)
	{
        VulkanGraphicsDevice& device = VulkanGraphicsDevice::GetInstance();
        
        //Delete all buffers to delete
        auto& buffers = m_BuffersToDelete[frameCount];
        for (auto& buffer : buffers)
        {
            vkDestroyBuffer(device.GetDevice(), buffer, nullptr);
            buffer = VK_NULL_HANDLE;
        }
        buffers.clear();
        
        //Update dynamic buffers
		for (auto buffer : m_Buffers)
			buffer->AdvanceFrame(frameCount);
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
