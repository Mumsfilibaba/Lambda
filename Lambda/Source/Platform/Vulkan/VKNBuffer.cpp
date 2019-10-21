#include "LambdaPch.h"
#include "Utilities/MathHelper.h"
#include "VKNBuffer.h"
#include "VKNUtilities.h"
#include "VKNDevice.h"

namespace Lambda
{
	//---------
	//VKNBuffer
	//---------

    VKNBuffer::VKNBuffer(VKNDevice* pDevice, const BufferDesc& desc)
		: BufferBase<VKNDevice>(pDevice),
        m_Memory(),
		m_Buffer(VK_NULL_HANDLE),
        m_FrameOffset(0),
        m_TotalSize(0),
        m_SizePerFrame(0),
        m_SizePerUpdate(0),
        m_DynamicOffset(0),
		m_IsDirty(false)
    {
		//Add a ref to the refcounter
		this->AddRef();
        Init(desc);
    }
    
    
    void VKNBuffer::Init(const BufferDesc& desc)
    {
        VkBufferCreateInfo info = {};
        info.sType					= VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        info.pNext					= nullptr;
        info.flags					= 0;
        info.queueFamilyIndexCount	= 0;
        info.pQueueFamilyIndices	= nullptr;
        info.sharingMode			= VK_SHARING_MODE_EXCLUSIVE;
		//Set usage
		info.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
		
		if (desc.Flags & BUFFER_FLAGS_VERTEX_BUFFER)
            info.usage |= VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
		
		if (desc.Flags & BUFFER_FLAGS_INDEX_BUFFER)
            info.usage |= VK_BUFFER_USAGE_INDEX_BUFFER_BIT;

		if (desc.Flags & BUFFER_FLAGS_CONSTANT_BUFFER)
            info.usage |= VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;

		//If dynamic we allocate extra size so we can use dynamic offsets
		if (desc.Usage == RESOURCE_USAGE_DYNAMIC)
		{
            VkPhysicalDeviceProperties properties = m_pDevice->GetPhysicalDeviceProperties();

			//If dynamic we allocate extra space (NUM_UPDATES updates per frame)
			constexpr uint32 numUpdatesPerFrame = 4;
			m_SizePerUpdate = Math::AlignUp<uint64>(desc.SizeInBytes, properties.limits.minUniformBufferOffsetAlignment);
			m_SizePerFrame	= m_SizePerUpdate * numUpdatesPerFrame;
            m_TotalSize     = m_SizePerFrame * FRAMES_AHEAD;
            info.size		= m_TotalSize;
		}
		else
		{
			info.size = desc.SizeInBytes;
		}
      
		//Create buffer
		if (vkCreateBuffer(m_pDevice->GetVkDevice(), &info, nullptr, &m_Buffer) != VK_SUCCESS)
        {
            LOG_DEBUG_ERROR("Vulkan: Failed to create Buffer\n");
            return;
        }
        else
        {
            m_Desc = desc;
			if (m_Desc.pName)
			{
				LOG_DEBUG_INFO("Vulkan: Created Buffer. Name=\"%s\". '%p'\n", m_Desc.pName, m_Buffer);
				m_pDevice->SetVulkanObjectName(VK_OBJECT_TYPE_BUFFER, uint64(m_Buffer), std::string(m_Desc.pName));
			}
			else
			{
				LOG_DEBUG_INFO("Vulkan: Created Buffer '%p'\n", m_Buffer);
			}
        }
        
		//Allocate memory
        if (!m_pDevice->AllocateBuffer(m_Memory, m_Buffer, m_Desc.Usage))
        {
            LOG_DEBUG_ERROR("Vulkan: Failed to allocate memory for buffer '%p'\n", m_Buffer);
        }
    }
    
    
	VKNBuffer::~VKNBuffer()
	{
		//Set the new handles and delete the old ones
		m_pDevice->Deallocate(m_Memory);
		if (m_Buffer != VK_NULL_HANDLE)
			m_pDevice->SafeReleaseVulkanResource<VkBuffer>(m_Buffer);

		VKNBufferManager& bufferManager = VKNBufferManager::GetInstance();
		bufferManager.UnregisterBuffer(this);

		LOG_DEBUG_INFO("Vulkan: Destroyed buffer '%p'\n", m_Buffer);
	}


	void VKNBuffer::Map(void** ppMem)
    {
		LAMBDA_ASSERT(ppMem != nullptr);

		uint8* pMemory = m_Memory.pHostMemory + GetDynamicOffset();
        (*ppMem) = reinterpret_cast<void*>(pMemory);
    }
    
    
    void VKNBuffer::Unmap()
    {
		//Unmapping a vulkan buffer does nothing since a whole memoryblock is mapped and stays mapped during its lifetime.
    }
    
    
    void* VKNBuffer::GetNativeHandle() const
    {
        return reinterpret_cast<void*>(m_Buffer);
    }


	void VKNBuffer::AdvanceFrame()
	{
		//Move on a frame
        m_FrameOffset   = 0;
        m_DynamicOffset = (m_DynamicOffset+m_SizePerFrame) % m_TotalSize;
	}


	void VKNBuffer::DynamicUpdate(const ResourceData* pData)
	{
		//Calculate offset and see if there are enough space
		m_FrameOffset += m_SizePerUpdate;
        if (m_FrameOffset >= m_SizePerFrame)
            Reallocate(m_SizePerFrame * 2);
        
		//Update buffer
		uint8* pCurrent = m_Memory.pHostMemory + GetDynamicOffset();
		memcpy(pCurrent, pData->pData, pData->SizeInBytes);
	}


	void VKNBuffer::Reallocate(uint32 sizePerFrame)
	{
		VkBufferCreateInfo info = {};
		info.sType					= VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		info.pNext					= nullptr;
		info.flags					= 0;
		info.queueFamilyIndexCount	= 0;
		info.pQueueFamilyIndices	= nullptr;
		info.sharingMode			= VK_SHARING_MODE_EXCLUSIVE;

        //Set new size
        m_SizePerFrame  = sizePerFrame;
        m_DynamicOffset = 0;
        m_FrameOffset   = 0;
        m_TotalSize     = m_SizePerFrame * FRAMES_AHEAD;
        info.size       = m_TotalSize;

        LOG_DEBUG_WARNING("Vulkan: Reallocated buffer. Old size: %llu bytes, New size: %llu\n", m_Memory.SizeInBytes, info.size);
        
        //Set usage
        info.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
        if (m_Desc.Flags & BUFFER_FLAGS_VERTEX_BUFFER)
            info.usage |= VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
        
        if (m_Desc.Flags & BUFFER_FLAGS_INDEX_BUFFER)
            info.usage |= VK_BUFFER_USAGE_INDEX_BUFFER_BIT;

        if (m_Desc.Flags & BUFFER_FLAGS_CONSTANT_BUFFER)
            info.usage |= VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;

        //Create buffer
        VkBuffer newBuffer = VK_NULL_HANDLE;
		if (vkCreateBuffer(m_pDevice->GetVkDevice(), &info, nullptr, &newBuffer) != VK_SUCCESS)
		{
			LOG_DEBUG_ERROR("Vulkan: Failed to recreate Buffer\n");
			return;
		}
		else
		{
			if (m_Desc.pName)
			{
				LOG_DEBUG_INFO("Vulkan: Recreated Buffer. Name=\"%s\". '%p'\n", m_Desc.pName, newBuffer);
				m_pDevice->SetVulkanObjectName(VK_OBJECT_TYPE_BUFFER, uint64(m_Buffer), std::string(m_Desc.pName));
			}
			else
			{
				LOG_DEBUG_INFO("Vulkan: Recreated Buffer '%p'\n", newBuffer);
			}
		}

		//Allocate memory
        VKNAllocation newMemory = {};
		if (!m_pDevice->AllocateBuffer(newMemory, newBuffer, m_Desc.Usage))
		{
			LOG_DEBUG_ERROR("Vulkan: Failed to allocate memory for resize of Buffer '%p'\n", newBuffer);
			return;
		}

		//Set the new handles and delete the old ones
		m_pDevice->Deallocate(m_Memory);
		m_pDevice->SafeReleaseVulkanResource<VkBuffer>(m_Buffer);
        
		m_Buffer = newBuffer;
		m_Memory = newMemory;
        
        //Set offsets
        m_FrameOffset   = 0;
        m_DynamicOffset = 0;

		//Set to dirty
		m_IsDirty = true;
	}

    //---------------
    //VKNUploadBuffer
    //---------------

    VKNUploadBuffer::VKNUploadBuffer(VKNDevice* pDevice, uint64 sizeInBytes)
        : m_pDevice(pDevice),
        m_Offset(0),
        m_Memory(),
        m_Buffer(VK_NULL_HANDLE)
    {
        Init(sizeInBytes);
    }


	VKNUploadBuffer::~VKNUploadBuffer()
	{
		Reset();

		if (m_Buffer != VK_NULL_HANDLE)
		{
			m_pDevice->Deallocate(m_Memory);
			m_pDevice->SafeReleaseVulkanResource<VkBuffer>(m_Buffer);
		}

		LOG_DEBUG_INFO("Vulkan: Destroyed UploadBuffer '%p'\n", m_Buffer);
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
        if (vkCreateBuffer(m_pDevice->GetVkDevice(), &info, nullptr, &m_Buffer) != VK_SUCCESS)
        {
            LOG_DEBUG_ERROR("Vulkan: Failed to create UploadBuffer\n");
            return false;
        }
        else
        {
            LOG_DEBUG_INFO("Vulkan: Created UploadBuffer '%p'\n", m_Buffer);
        }
        
        //Allocate memory
		if (m_pDevice->AllocateBuffer(m_Memory, m_Buffer, RESOURCE_USAGE_DYNAMIC))
		{
			Reset();
			return true;
		}
		else
		{
			LOG_DEBUG_ERROR("Vulkan: Failed to allocate memory for UploadBuffer '%p'\n", m_Buffer);
			return false;
		}
    }


    void VKNUploadBuffer::Reallocate(uint64 sizeInBytes)
    {
        VkBuffer  newBuffer = VK_NULL_HANDLE;
        VKNAllocation newMemory = {};

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
        if (vkCreateBuffer(m_pDevice->GetVkDevice(), &info, nullptr, &newBuffer) != VK_SUCCESS)
        {
            LOG_DEBUG_ERROR("Vulkan: Failed to recreate UploadBuffer\n");
        }
        else
        {
            LOG_DEBUG_WARNING("Vulkan: Recreated UploadBuffer '%p'\n", newBuffer);
        }

        //Allocate memory
		if (m_pDevice->AllocateBuffer(newMemory, newBuffer, RESOURCE_USAGE_DYNAMIC))
        {
			m_pDevice->Deallocate(m_Memory);
			m_pDevice->SafeReleaseVulkanResource<VkBuffer>(m_Buffer);
            
            //Set new buffer and memory
            m_Buffer = newBuffer;
            m_Memory = newMemory;
            
            Reset();
        }
        else
        {
            LOG_DEBUG_ERROR("Vulkan: Failed to reallocate memory for UploadBuffer '%p'\n", m_Buffer);
        }
    }


	VKNUploadAllocation VKNUploadBuffer::Allocate(uint64 bytesToAllocate, uint64 alignment)
    {
		VKNUploadAllocation allocation = {};

		//Align
		uint64 alignedOffset = Math::AlignUp<uint64>(m_Offset, alignment);
		uint64 padding = alignedOffset - m_Offset;

        //Do we have enough space? If no reallocate
		uint64 alignedSize = bytesToAllocate + padding;
        if ((alignedOffset + alignedSize) >= m_Memory.SizeInBytes)
            Reallocate(m_Memory.SizeInBytes + alignedSize + MB(1));

        //Move pointer
        allocation.pHostMemory = reinterpret_cast<void*>(m_Memory.pHostMemory + alignedOffset);
		m_Offset += alignedSize;

		//Set offset
		allocation.SizeInBytes	= alignedSize;
		allocation.DeviceOffset = alignedOffset;
		
		//Set buffer
		allocation.Buffer = m_Buffer;
        return allocation;
    }
    
    
    void VKNUploadBuffer::Reset()
    {
        //Reset buffer by resetting current to the start
		m_Offset = 0;
    }

	//----------------
	//VKNBufferManager
	//----------------

    constexpr size_t frameCount = 5;
	VKNBufferManager* VKNBufferManager::s_pInstance = nullptr;

	VKNBufferManager::VKNBufferManager()
		: m_FrameIndex(0),
        m_Buffers()
	{
		LAMBDA_ASSERT(s_pInstance == nullptr);
		s_pInstance = this;
	}


	VKNBufferManager::~VKNBufferManager()
	{
		if (s_pInstance == this)
			s_pInstance = nullptr;
	}
    

	void VKNBufferManager::AdvanceFrame()
	{       
        //Update dynamic buffers
		for (auto buffer : m_Buffers)
			buffer->AdvanceFrame();
	}


	void VKNBufferManager::RegisterBuffer(VKNBuffer* pBuffer)
	{
		for (auto buffer : m_Buffers)
		{
			if (buffer == pBuffer)
				return;
		}

		LOG_DEBUG_INFO("Vulkan: Registered buffer\n");
		m_Buffers.emplace_back(pBuffer);
	}


	void VKNBufferManager::UnregisterBuffer(VKNBuffer* pBuffer)
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
	
	
	VKNBufferManager& VKNBufferManager::GetInstance()
	{
		LAMBDA_ASSERT(s_pInstance != nullptr);
		return *s_pInstance;
	}
}
