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
        
        VulkanBuffer(VkDevice device, VkPhysicalDevice adapter, const ResourceData* pInitalData, const BufferDesc& desc);
        ~VulkanBuffer() = default;
        
        virtual void Map(void** ppMem) override final;
        virtual void Unmap() override final;
        
        virtual BufferDesc GetDesc() const override final;
        
        void Destroy(VkDevice device);
        VkBuffer GetBuffer() const;
        
    private:
        void Init(VkDevice device, VkPhysicalDevice adapter, const ResourceData* pInitalData, const BufferDesc& desc);
        
        VkBufferView GetBufferView(uint32 index) const;
        void SetBufferView(VkBufferView view);
        
    private:
        VkBuffer m_Buffer;
        VkDeviceMemory m_BufferMemory;
        std::vector<VkBufferView> m_Views;
        BufferDesc m_Desc;
    };
    
    
    inline VkBuffer VulkanBuffer::GetBuffer() const
    {
        return m_Buffer;
    }
    
    
    inline VkBufferView VulkanBuffer::GetBufferView(uint32 index) const
    {
        return m_Views[index];
    }
    
    
    inline void VulkanBuffer::SetBufferView(VkBufferView view)
    {
        if (m_Views.size() < 1)
            m_Views.push_back(view);
        else
            m_Views[0] = view;
    }
}
