#pragma once
#include "Graphics/IBuffer.h"
#include "VulkanAllocator.h"

namespace Lambda
{
    class VulkanBuffer final : public IBuffer
    {
    public:
        LAMBDA_NO_COPY(VulkanBuffer);
        
        VulkanBuffer(VkDevice device, VkPhysicalDevice physicalDevice, IVulkanAllocator* pAllocator, const BufferDesc& desc);
        ~VulkanBuffer() = default;
        
        virtual void Map(void** ppMem) override final;
        virtual void Unmap() override final;
        
        virtual void* GetNativeHandle() const override final;
        virtual BufferDesc GetDesc() const override final;
        
		void SetDynamicOffset(uint64 dynamicOffset);
		uint64 GetDynamicOffset() const;

        //Release releases vulkan resources while destroy also calls 'delete this'
        void Release(VkDevice device);
        void Destroy(VkDevice device);
        
    private:
        void Init(VkDevice device, const BufferDesc& desc);
        
    private:
		IVulkanAllocator* const m_pAllocator;
        VulkanMemory m_Memory;
        VkBuffer m_Buffer;
        BufferDesc m_Desc;
		uint64 m_DynamicOffset;
        uint64 m_DynamicAlignment;
    };
}
