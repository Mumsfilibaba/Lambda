#pragma once
#include "Graphics/IBuffer.h"
#include "VulkanAllocator.h"

namespace Lambda
{
	class VulkanGraphicsDevice;


    class VulkanBuffer final : public IBuffer
    {
    public:
        LAMBDA_NO_COPY(VulkanBuffer);
        
        VulkanBuffer(const VulkanGraphicsDevice* pVkDevice, IVulkanAllocator* pAllocator, const BufferDesc& desc);
        ~VulkanBuffer() = default;
        
        virtual void Map(void** ppMem) override final;
        virtual void Unmap() override final;
        
        virtual void* GetNativeHandle() const override final;
        virtual BufferDesc GetDesc() const override final;
        
        //Release releases vulkan resources while destroy also calls 'delete this'
        void Release(VkDevice device);
        void Destroy(VkDevice device);
        
    private:
        void Init(const VulkanGraphicsDevice* pVkDevice, const BufferDesc& desc);
        
    private:
		IVulkanAllocator* const m_pAllocator;
        VulkanAllocation m_Memory;
		VkDevice m_Device;
        VkBuffer m_Buffer;
        BufferDesc m_Desc;
    };
}
