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

    VKNBuffer::VKNBuffer(VKNDevice* pDevice, IVKNAllocator* pAllocator, const BufferDesc& desc)
		: DeviceObjectBase<VKNDevice, IBuffer>(pDevice),
		m_pAllocator(pAllocator),
        m_Memory(),
		m_Buffer(VK_NULL_HANDLE),
        m_FrameOffset(0),
        m_TotalSize(0),
        m_SizePerFrame(0),
        m_SizePerUpdate(0),
        m_DynamicOffset(0),
        m_Desc(),
		m_IsDirty(false)
    {
		//Add a ref to the refcounter
		this->AddRef();

		LAMBDA_ASSERT(pAllocator != nullptr && pDevice != nullptr);
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

		//If dynamic we allocate extra size so we can use dynamic offsets
		if (desc.Usage == RESOURCE_USAGE_DYNAMIC)
		{
            const DeviceDesc& deviceDesc          = m_pDevice->GetDesc();
            VkPhysicalDeviceProperties properties = m_pDevice->GetPhysicalDeviceProperties();

			//If dynamic we allocate extra space (NUM_UPDATES updates per frame)
			constexpr uint32 numUpdatesPerFrame = 4;
			m_SizePerUpdate = Math::AlignUp<uint64>(desc.SizeInBytes, properties.limits.minUniformBufferOffsetAlignment);
			m_SizePerFrame	= m_SizePerUpdate * numUpdatesPerFrame;
            m_TotalSize     = m_SizePerFrame * deviceDesc.BackBufferCount;
            info.size		= m_TotalSize;
		}
		else
		{
			info.size = desc.SizeInBytes;
		}
        
		//Set usage
		info.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
		
		if (desc.Flags & BUFFER_FLAGS_VERTEX_BUFFER)
            info.usage |= VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
		
		if (desc.Flags & BUFFER_FLAGS_INDEX_BUFFER)
            info.usage |= VK_BUFFER_USAGE_INDEX_BUFFER_BIT;

		if (desc.Flags & BUFFER_FLAGS_CONSTANT_BUFFER)
            info.usage |= VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
        
		//Create buffer
		if (vkCreateBuffer(m_pDevice->GetVkDevice(), &info, nullptr, &m_Buffer) != VK_SUCCESS)
        {
            LOG_DEBUG_ERROR("Vulkan: Failed to create Buffer\n");
            return;
        }
        else
        {
			if (m_Desc.pName)
			{
				LOG_DEBUG_INFO("Vulkan: Created Buffer. Name=\"%s\"\n", m_Desc.pName);
				m_pDevice->SetVulkanObjectName(VK_OBJECT_TYPE_BUFFER, uint64(m_Buffer), std::string(m_Desc.pName));
			}
			else
			{
				LOG_DEBUG_INFO("Vulkan: Created Buffer\n");
			}

            m_Desc = desc;
        }
        
		//Allocate memory
		VkMemoryRequirements memoryRequirements = {};
		vkGetBufferMemoryRequirements(m_pDevice->GetVkDevice(), m_Buffer, &memoryRequirements);
        if (m_pAllocator->Allocate(m_Memory, memoryRequirements, m_Desc.Usage))
		{
            vkBindBufferMemory(m_pDevice->GetVkDevice(), m_Buffer, m_Memory.DeviceMemory, m_Memory.DeviceMemoryOffset);
        }
        else
        {
            LOG_DEBUG_ERROR("Vulkan: Failed to allocate memory for buffer\n");
        }
    }
    
    
	VKNBuffer::~VKNBuffer()
	{
		//Set the new handles and delete the old ones
		m_pAllocator->Deallocate(m_Memory);

		VKNBufferManager& bufferManager = VKNBufferManager::GetInstance();
		bufferManager.DestroyBuffer(m_Buffer);
		bufferManager.UnregisterBuffer(this);

		LOG_DEBUG_INFO("Vulkan: Destroyed buffer\n");
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

    
    const BufferDesc& VKNBuffer::GetDesc() const
    {
        return m_Desc;
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

        const DeviceDesc& desc = m_pDevice->GetDesc(); 
        //Set new size
        m_SizePerFrame  = sizePerFrame;
        m_DynamicOffset = 0;
        m_FrameOffset   = 0;
        m_TotalSize     = m_SizePerFrame * desc.BackBufferCount;
        info.size       = m_TotalSize;

        LOG_DEBUG_WARNING("Vulkan: Reallocated buffer. Old size: %llu bytes, New size: %llu\n", m_Memory.Size, info.size);
        
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
				LOG_DEBUG_INFO("Vulkan: Recreated Buffer. Name=\"%s\"\n", m_Desc.pName);
				m_pDevice->SetVulkanObjectName(VK_OBJECT_TYPE_BUFFER, uint64(m_Buffer), std::string(m_Desc.pName));
			}
			else
			{
				LOG_DEBUG_INFO("Vulkan: Recreated Buffer\n");
			}
		}

		//Allocate memory
        VKNMemory newMemory = {};
        VkMemoryRequirements memoryRequirements = {};
        vkGetBufferMemoryRequirements(m_pDevice->GetVkDevice(), newBuffer, &memoryRequirements);
		if (m_pAllocator->Allocate(newMemory, memoryRequirements, m_Desc.Usage))
		{
			vkBindBufferMemory(m_pDevice->GetVkDevice(), newBuffer, newMemory.DeviceMemory, newMemory.DeviceMemoryOffset);
		}
		else
		{
            LOG_DEBUG_ERROR("Vulkan: Failed to allocate memory for resize of buffer\n");
		}

		//Set the new handles and delete the old ones
		m_pAllocator->Deallocate(m_Memory);

		VKNBufferManager& bufferManager = VKNBufferManager::GetInstance();
		bufferManager.DestroyBuffer(m_Buffer);
        
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

    VKNUploadBuffer::VKNUploadBuffer(IVKNAllocator* pAllocator, uint64 sizeInBytes)
        : m_pAllocator(pAllocator),
        m_pCurrent(nullptr),
        m_Memory(),
        m_Buffer(VK_NULL_HANDLE),
        m_BytesLeft(0),
        m_SizeInBytes(0)
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
        if (vkCreateBuffer(device.GetVkDevice(), &info, nullptr, &m_Buffer) != VK_SUCCESS)
        {
            LOG_DEBUG_ERROR("Vulkan: Failed to create buffer for UploadBuffer\n");
            return false;
        }
        else
        {
            LOG_DEBUG_INFO("Vulkan: Created buffer for UploadBuffer\n");
            m_SizeInBytes = sizeInBytes;
        }
        
        //Allocate memory
        VkMemoryRequirements memoryRequirements = {};
        vkGetBufferMemoryRequirements(device.GetVkDevice(), m_Buffer, &memoryRequirements);
        if (m_pAllocator->Allocate(m_Memory, memoryRequirements, RESOURCE_USAGE_DYNAMIC))
        {
            vkBindBufferMemory(device.GetVkDevice(), m_Buffer, m_Memory.DeviceMemory, m_Memory.DeviceMemoryOffset);
            Reset();
            return true;
        }
        else
        {
            LOG_DEBUG_ERROR("Vulkan: Failed to allocate memory for uploadbuffer\n");
            return false;
        }
    }


    void VKNUploadBuffer::Reallocate(uint64 sizeInBytes)
    {
        VkBuffer  newBuffer = VK_NULL_HANDLE;
        VKNMemory newMemory = {};

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
        if (vkCreateBuffer(device.GetVkDevice(), &info, nullptr, &newBuffer) != VK_SUCCESS)
        {
            LOG_DEBUG_ERROR("Vulkan: Failed to reallocate buffer for UploadBuffer\n");
        }
        else
        {
            LOG_DEBUG_WARNING("Vulkan: Reallocated buffer for UploadBuffer\n");
            m_SizeInBytes = sizeInBytes;
        }

        //Allocate memory
        VkMemoryRequirements memoryRequirements = {};
        vkGetBufferMemoryRequirements(device.GetVkDevice(), newBuffer, &memoryRequirements);
        if (m_pAllocator->Allocate(newMemory, memoryRequirements, RESOURCE_USAGE_DYNAMIC))
        {
            vkBindBufferMemory(device.GetVkDevice(), newBuffer, newMemory.DeviceMemory, newMemory.DeviceMemoryOffset);

            m_pAllocator->Deallocate(m_Memory);

            VKNBufferManager& bufferManager = VKNBufferManager::GetInstance();
            bufferManager.DestroyBuffer(m_Buffer);
            
            //Set new buffer and memory
            m_Buffer = newBuffer;
            m_Memory = newMemory;
            
            Reset();
        }
        else
        {
            LOG_DEBUG_ERROR("Vulkan: Failed to reallocate uploadbuffer\n");
        }
    }
    
    
    void* VKNUploadBuffer::Allocate(uint64 bytesToAllocate)
    {
        //Do we have enough space? If no reallocate
        if (bytesToAllocate > m_BytesLeft)
            Reallocate(bytesToAllocate + MB(1));

        //Move pointer
        void* pAllocation = reinterpret_cast<void*>(m_pCurrent);
        m_pCurrent  += bytesToAllocate;
        m_BytesLeft -= bytesToAllocate;
        return pAllocation;
    }
    
    
    void VKNUploadBuffer::Reset()
    {
        //Reset buffer by resetting current to the start
        m_pCurrent  = m_Memory.pHostMemory;
        m_BytesLeft = m_SizeInBytes;
    }
    
    
    void VKNUploadBuffer::Destroy(VkDevice device)
    {
        LAMBDA_ASSERT(device != VK_NULL_HANDLE);
        
        Reset();
        
        if (m_Buffer != VK_NULL_HANDLE)
        {
            m_pAllocator->Deallocate(m_Memory);

            VKNBufferManager& bufferManager = VKNBufferManager::GetInstance();
            bufferManager.DestroyBuffer(m_Buffer);
        }
        
        LOG_DEBUG_INFO("Vulkan: Destroyed UploadBuffer\n");
        delete this;
    }

	//----------------
	//VKNBufferManager
	//----------------

    constexpr size_t frameCount = 5;
	VKNBufferManager* VKNBufferManager::s_pInstance = nullptr;

	VKNBufferManager::VKNBufferManager()
		: m_FrameIndex(0),
        m_Buffers(),
        m_BuffersToDelete()
	{
		LAMBDA_ASSERT(s_pInstance == nullptr);
		s_pInstance = this;
        
        m_BuffersToDelete.resize(frameCount);
	}


	VKNBufferManager::~VKNBufferManager()
	{
		if (s_pInstance == this)
			s_pInstance = nullptr;

		//Delete all buffers
		for (uint32 i = 0; i < frameCount; i++)
            EmptyGarbageBuffers();
	}
    
    
    void VKNBufferManager::DestroyBuffer(VkBuffer buffer)
    {
        if (buffer != VK_NULL_HANDLE)
        {
            auto& buffers = m_BuffersToDelete[m_FrameIndex];
            buffers.push_back(buffer);
        }
    }


	void VKNBufferManager::AdvanceFrame()
	{
        //Delete all buffers to delete
        EmptyGarbageBuffers();
        
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

	
	void VKNBufferManager::EmptyGarbageBuffers()
	{
        //Advance frameindex
        m_FrameIndex = (m_FrameIndex+1) % frameCount;
        
        //Delete all buffers for the current frmae
        auto& buffers = m_BuffersToDelete[m_FrameIndex];
        if (buffers.size() > 1)
        {
			VKNDevice& device = VKNDevice::Get();
            for (auto& buffer : buffers)
            {
                vkDestroyBuffer(device.GetVkDevice(), buffer, nullptr);
                buffer = VK_NULL_HANDLE;
            }
            
            buffers.clear();
        }
	}
	
	
	VKNBufferManager& VKNBufferManager::GetInstance()
	{
		LAMBDA_ASSERT(s_pInstance != nullptr);
		return *s_pInstance;
	}
}
