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

    VKNBuffer::VKNBuffer(VKNDevice* pVkDevice, const SResourceData* pInitalData, const SBufferDesc& desc)
		: TBuffer(pVkDevice, desc),
		m_VkBuffer(VK_NULL_HANDLE),
        m_Memory(),
		m_DynamicMemory()
    {
		//Add a ref to the refcounter
		this->AddRef();
        Init(pInitalData, desc);
    }


	VKNBuffer::~VKNBuffer()
	{
		//Debug
		if (m_VkBuffer == VK_NULL_HANDLE)
		{
			LOG_RENDER_API_INFO("[Vulkan] Destroyed Buffer '%s'\n", m_Name.c_str());
		}
		else
		{
			LOG_RENDER_API_INFO("[Vulkan] Destroyed Buffer '%p' '%s'\n", m_VkBuffer, m_Name.c_str());
		}

		//Deallocate the dynamic resource
		if (m_Desc.Usage == USAGE_DYNAMIC)
		{
			m_pDevice->DeallocateDynamicMemory(m_DynamicMemory);
		}
		else
		{
			//Deallocate from global memory if this is not a global resource and then release the native buffer handle
			m_pDevice->Deallocate(m_Memory);
			if (m_VkBuffer != VK_NULL_HANDLE)
				m_pDevice->SafeReleaseVkResource<VkBuffer>(m_VkBuffer);
		}
	}
    
    
    void VKNBuffer::Init(const SResourceData* pInitalData, const SBufferDesc& desc)
    {
		//Set alignment for buffer
		VkPhysicalDeviceProperties properties = m_pDevice->GetVkPhysicalDeviceProperties();
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
		if (desc.Usage == USAGE_DYNAMIC)
		{
			//If dynamic we allocate dynamic memory
			if (!m_pDevice->AllocateDynamicMemory(m_DynamicMemory, m_Desc.SizeInBytes, m_DynamicOffsetAlignment))
			{
				LOG_RENDER_API_ERROR("[Vulkan] Failed to allocate memory for Buffer\n");
				return;
			}
			else
			{
				LOG_RENDER_API_INFO("[Vulkan] Allocated memory for Dynamic Buffer\n");
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
				LOG_RENDER_API_ERROR("[Vulkan] Failed to create Buffer\n");
				return;
			}
			else
			{
				SetName(m_Desc.pName);
				if (m_Desc.pName)
				{
					LOG_RENDER_API_INFO("[Vulkan] Created Buffer. Name=\"%s\". '%p'\n", m_Desc.pName, m_VkBuffer);
				}
				else
				{
					LOG_RENDER_API_INFO("[Vulkan] Created Buffer '%p'\n", m_VkBuffer);
				}
			}


			//Memory properties
			VkMemoryPropertyFlags memoryProperties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

			//Allocate memory
			VkMemoryRequirements memoryRequirements = {};
			vkGetBufferMemoryRequirements(m_pDevice->GetVkDevice(), m_VkBuffer, &memoryRequirements);
			if (!m_pDevice->Allocate(m_Memory, memoryRequirements, memoryProperties))
			{
				LOG_RENDER_API_ERROR("[Vulkan] Failed to allocate Dynamic Memory-Page '%p'\n", m_VkBuffer);
				return;
			}
			else
			{
				LOG_RENDER_API_WARNING("[Vulkan] Allocated '%d' bytes for Dynamic Memory-Page\n", m_Desc.SizeInBytes);
				if (vkBindBufferMemory(m_pDevice->GetVkDevice(), m_VkBuffer, m_Memory.DeviceMemory, m_Memory.DeviceMemoryOffset) != VK_SUCCESS)
				{
					LOG_RENDER_API_WARNING("[Vulkan] Failed to bind memory for Dynamic Memory-Page\n");
				}
			}
		}


		//Write to the memory if we have any inital data
		if (pInitalData)
		{
			LAMBDA_ASSERT_PRINT(pInitalData->pData != nullptr && pInitalData->SizeInBytes != 0, "[Vulkan] pInitalData->pData cannot be null\n");
			LAMBDA_ASSERT_PRINT(pInitalData->SizeInBytes <= m_Desc.SizeInBytes, "[Vulkan] pInitalData->SizeInBytes cannot be larger than the buffer. pInitalData->SizeInBytes=%d and BufferDesc::SizeInBytes=%d\n", pInitalData->SizeInBytes, m_Desc.SizeInBytes);

			if (desc.Usage == USAGE_DYNAMIC)
			{
				//If the resource is dynamic we can simply write to the dynamic memory with memcpy
				memcpy(m_DynamicMemory.pHostMemory, pInitalData->pData, pInitalData->SizeInBytes);
			}
			else
			{
				//Otherwise we create a staging buffer
				info.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;

				VkBuffer vkStagingBuffer = VK_NULL_HANDLE;
				if (vkCreateBuffer(m_pDevice->GetVkDevice(), &info, nullptr, &vkStagingBuffer) != VK_SUCCESS)
				{
					LOG_RENDER_API_ERROR("[Vulkan] Failed to create Staging-Buffer\n");
					return;
				}
				else
				{
					LOG_RENDER_API_INFO("[Vulkan] Created Staging-Buffer '%p'\n", vkStagingBuffer);
				}


				//Memory for the staging buffer
				VKNAllocation stagingMemory = {};

				//Memory properties
				VkMemoryPropertyFlags memoryProperties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

				//Allocate memory
				VkMemoryRequirements memoryRequirements = {};
				vkGetBufferMemoryRequirements(m_pDevice->GetVkDevice(), vkStagingBuffer, &memoryRequirements);
				if (!m_pDevice->Allocate(stagingMemory, memoryRequirements, memoryProperties))
				{
					LOG_RENDER_API_ERROR("[Vulkan] Failed to allocate StagingBuffer '%p'\n", vkStagingBuffer);
					return;
				}
				else
				{
					LOG_RENDER_API_WARNING("[Vulkan] Allocated '%d' bytes for StagingBuffer\n", m_Desc.SizeInBytes);
					if (vkBindBufferMemory(m_pDevice->GetVkDevice(), vkStagingBuffer, stagingMemory.DeviceMemory, stagingMemory.DeviceMemoryOffset) != VK_SUCCESS)
					{
						LOG_RENDER_API_WARNING("[Vulkan] Failed to bind memory StagingBuffer\n");
					}
					else
					{
						//Copy over data
						memcpy(stagingMemory.pHostMemory, pInitalData->pData, pInitalData->SizeInBytes);
					}
				}


				//Get devicecontext and copy over the stagingbuffers context to the buffer
				VKNDeviceContext* pVkContext = m_pDevice->GetVKNImmediateContext();
				pVkContext->CopyBuffer(m_VkBuffer, 0, vkStagingBuffer, 0, pInitalData->SizeInBytes);
				
				//Release this reference to the context
				pVkContext->Release();

				//Delete the stagingbuffer
				m_pDevice->Deallocate(stagingMemory);
				m_pDevice->SafeReleaseVkResource<VkBuffer>(vkStagingBuffer);
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
		{
			m_pDevice->SetVulkanObjectName(VK_OBJECT_TYPE_BUFFER, (uint64)m_VkBuffer, m_Name);
			m_Desc.pName = m_Name.c_str();
		}
	}
}
