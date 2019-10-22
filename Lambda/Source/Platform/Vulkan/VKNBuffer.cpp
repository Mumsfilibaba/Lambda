#include "LambdaPch.h"
#include "Utilities/MathHelper.h"
#include "VKNBuffer.h"
#include "VKNUtilities.h"
#include "VKNDevice.h"
#include "VKNDeviceContext.h"

namespace Lambda
{
	//---------
	//VKNBuffer
	//---------

    VKNBuffer::VKNBuffer(VKNDevice* pDevice, const ResourceData* pInitalData, const BufferDesc& desc)
		: TBuffer(pDevice, desc),
		m_VkBuffer(VK_NULL_HANDLE),
        m_Memory(),
		m_DynamicState()
    {
		//Add a ref to the refcounter
		this->AddRef();
        Init(pInitalData, desc);
    }


	VKNBuffer::~VKNBuffer()
	{
		//Deallocate the dynamic resource
		if (m_Desc.Usage == RESOURCE_USAGE_DYNAMIC)
		{
			m_pDevice->DeallocateDynamicMemory(m_DynamicState);
		}
		else
		{
			//Deallocate from global memory if this is not a global resource and then release the native buffer handle
			m_pDevice->Deallocate(m_Memory);
			if (m_VkBuffer != VK_NULL_HANDLE)
				m_pDevice->SafeReleaseVulkanResource<VkBuffer>(m_VkBuffer);
		}

		LOG_DEBUG_INFO("Vulkan: Destroyed buffer '%p'\n", m_VkBuffer);
	}
    
    
    void VKNBuffer::Init(const ResourceData* pInitalData, const BufferDesc& desc)
    {
		//Set alignment for buffer
		VkPhysicalDeviceProperties properties = m_pDevice->GetPhysicalDeviceProperties();
		m_DynamicOffsetAlignment = std::max(VkDeviceSize(4), properties.limits.minUniformBufferOffsetAlignment);


		//Setup buffer info
		VkBufferCreateInfo info = {};
		info.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
		if (desc.Flags & BUFFER_FLAGS_CONSTANT_BUFFER)
		{
			info.usage |= VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
			m_DynamicOffsetAlignment = std::max(m_DynamicOffsetAlignment, properties.limits.minUniformBufferOffsetAlignment);
		}

		if (desc.Flags & BUFFER_FLAGS_VERTEX_BUFFER)
			info.usage |= VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;

		if (desc.Flags & BUFFER_FLAGS_INDEX_BUFFER)
			info.usage |= VK_BUFFER_USAGE_INDEX_BUFFER_BIT;


		//Create buffer
		if (desc.Usage == RESOURCE_USAGE_DYNAMIC)
		{
			/*std::vector<VKNDynamicAllocation> allocs;
			for (uint32 i = 0; i < 10; i++)
			{
				VKNDynamicAllocation mem = {};
				m_pDevice->AllocateDynamicMemory(mem, m_Desc.SizeInBytes, m_DynamicOffsetAlignment);
				allocs.emplace_back(mem);
			}

			for (uint32 i = 3; i < 8; i++)
				m_pDevice->DeallocateDynamicMemory(allocs[i]);


			for (uint32 i = 0; i < 10; i++)
			{
				VKNDynamicAllocation mem = {};
				m_pDevice->AllocateDynamicMemory(mem, m_Desc.SizeInBytes, m_DynamicOffsetAlignment);
				allocs.emplace_back(mem);
			}

			for (uint32 i = 0; i < 10; i++)
				m_pDevice->FinishFrame();


			for (uint32 i = 0; i < 10; i++)
			{
				VKNDynamicAllocation mem = {};
				m_pDevice->AllocateDynamicMemory(mem, m_Desc.SizeInBytes, m_DynamicOffsetAlignment);
				allocs.emplace_back(mem);
			}*/

			//If dynamic we allocate dynamic memory
			if (!m_pDevice->AllocateDynamicMemory(m_DynamicState, m_Desc.SizeInBytes, m_DynamicOffsetAlignment))
			{
				LOG_DEBUG_ERROR("Vulkan: Failed to allocate memory for Buffer\n");
				return;
			}
			else
			{
				LOG_DEBUG_INFO("Vulkan: Allocated memory for Dynamic Buffer\n");
				SetName(m_Desc.pName);
			}
		}
		else
		{
			//Otherwise create a vulkanbuffer
			info.sType	= VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
			info.pNext	= nullptr;
			info.flags	= 0;
			info.size	= desc.SizeInBytes;
			info.queueFamilyIndexCount	= 0;
			info.pQueueFamilyIndices	= nullptr;
			info.sharingMode			= VK_SHARING_MODE_EXCLUSIVE;
			if (vkCreateBuffer(m_pDevice->GetVkDevice(), &info, nullptr, &m_VkBuffer) != VK_SUCCESS)
			{
				LOG_DEBUG_ERROR("Vulkan: Failed to create Buffer\n");
				return;
			}
			else
			{
				SetName(m_Desc.pName);
				if (m_Desc.pName)
				{
					LOG_DEBUG_INFO("Vulkan: Created Buffer. Name=\"%s\". '%p'\n", m_Desc.pName, m_VkBuffer);
				}
				else
				{
					LOG_DEBUG_INFO("Vulkan: Created Buffer '%p'\n", m_VkBuffer);
				}
			}

			//Allocate memory
			if (!m_pDevice->AllocateBuffer(m_Memory, m_VkBuffer, m_Desc.Usage))
			{
				LOG_DEBUG_ERROR("Vulkan: Failed to allocate memory for buffer '%p'\n", m_VkBuffer);
			}
		}

		//Write to the memory if we have any inital data
		if (pInitalData)
		{
			LAMBDA_ASSERT_PRINT(pInitalData->pData != nullptr && pInitalData->SizeInBytes != 0, "Vulkan: pInitalData->pData cannot be null\n");
			LAMBDA_ASSERT_PRINT(pInitalData->SizeInBytes <= m_Desc.SizeInBytes, "Vulkan: pInitalData->SizeInBytes cannot be larger than the buffer. pInitalData->SizeInBytes=%d and BufferDesc::SizeInBytes=%d\n", pInitalData->SizeInBytes, m_Desc.SizeInBytes);

			if (desc.Usage == RESOURCE_USAGE_DYNAMIC)
			{
				//If the resource is dynamic we can simply write to the dynamic memory with memcpy
				memcpy(m_DynamicState.pHostMemory, pInitalData->pData, pInitalData->SizeInBytes);
			}
			else
			{
				//Otherwise we create a staging buffer
				VkBuffer stagingBuffer = VK_NULL_HANDLE;
				if (vkCreateBuffer(m_pDevice->GetVkDevice(), &info, nullptr, &stagingBuffer) != VK_SUCCESS)
				{
					LOG_DEBUG_ERROR("Vulkan: Failed to create Staging-Buffer\n");
					return;
				}
				else
				{
					LOG_DEBUG_INFO("Vulkan: Created Staging-Buffer '%p'\n", m_VkBuffer);
				}

				//Allocate memory
				VKNAllocation stagingMemory = {};
				if (!m_pDevice->AllocateBuffer(stagingMemory, stagingBuffer, RESOURCE_USAGE_DYNAMIC))
				{
					LOG_DEBUG_ERROR("Vulkan: Failed to allocate memory for Staging-Buffer '%p'\n", stagingBuffer);
					return;
				}
				else
				{
					//Copy over data
					memcpy(stagingMemory.pHostMemory, pInitalData->pData, pInitalData->SizeInBytes);
				}

				//Get devicecontext and copy over the stagingbuffers context to the buffer
				VKNDeviceContext* pContext = m_pDevice->GetVKNImmediateContext();
				pContext->CopyBuffer(m_VkBuffer, 0, stagingBuffer, 0, pInitalData->SizeInBytes);
				
				//Release this reference to the context
				pContext->Release();

				//Delete the stagingbuffer
				m_pDevice->Deallocate(stagingMemory);
				m_pDevice->SafeReleaseVulkanResource<VkBuffer>(stagingBuffer);
			}
		}
    }
    
    
    void* VKNBuffer::GetNativeHandle() const
    {
        return reinterpret_cast<void*>(m_VkBuffer);
    }

	
	void VKNBuffer::SetName(const char* pName)
	{
		TBuffer::SetName(pName);
		if (m_VkBuffer != VK_NULL_HANDLE)
			m_pDevice->SetVulkanObjectName(VK_OBJECT_TYPE_BUFFER, (uint64)m_VkBuffer, m_Name);
	}

	
	VkBuffer VKNBuffer::GetVkBuffer() const
	{
		if (m_Desc.Usage == RESOURCE_USAGE_DYNAMIC)
			return m_DynamicState.Buffer;
		
		return m_VkBuffer;
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
}
