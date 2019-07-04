#pragma once
#include "Graphics/IBuffer.h"
#include <vulkan/vulkan.h>
#include <vector>

namespace Lambda
{
    class VulkanBuffer final : public IBuffer
    {
    public:
        LAMBDA_NO_COPY(VulkanBuffer);
        
        VulkanBuffer();
        VulkanBuffer(VkDevice device, VkPhysicalDevice adapter, const BufferDesc& desc);
        ~VulkanBuffer() = default;
        
        virtual void Map(void** ppMem) override final;
        virtual void Unmap() override final;
        
        virtual void* GetNativeHandle() const override final;
        virtual uint64 GetSizeInBytes() const override final;
        virtual BufferDesc GetDesc() const override final;
        
        //Release releases vulkan resources while destroy also calls 'delete this'
        void Release(VkDevice device);
        void Destroy(VkDevice device);
        
    private:
        void Init(VkDevice device, VkPhysicalDevice adapter, const BufferDesc& desc);
        
    private:
        VkBuffer m_Buffer;
        VkDeviceMemory m_BufferMemory;
        BufferDesc m_Desc;
    };
}
