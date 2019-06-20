#pragma once
#include "Graphics/ITexture2D.h"
#include <vulkan/vulkan.h>
#include <vector>

namespace Lambda
{
    class VulkanTexture2D final : public ITexture2D
    {
        friend class VulkanGraphicsDevice;
        friend class VulkanCommandList;
        friend class VulkanFramebufferCache;
        
    public:
        LAMBDA_NO_COPY(VulkanTexture2D);

        VulkanTexture2D(VkImage image, VkExtent2D extent, ResourceFormat format, OptimizedClearValue clearValue, uint32 flags, uint32 miplevels, uint32 arraySize);
        VulkanTexture2D(VkDevice device, const Texture2DDesc& desc);
        VulkanTexture2D() = default;
        
        virtual Texture2DDesc GetDesc() const override final;
        
        VkImage GetResource() const;
        void Destroy(VkDevice device);
        
    private:
        void Init(VkDevice device, const Texture2DDesc& desc);
        void InitFromResource(VkImage image, VkExtent2D extent, ResourceFormat format, OptimizedClearValue clearValue, uint32 flags, uint32 miplevels, uint32 arraySize);
        
        void SetImageView(VkImageView imageView);
        VkImageView GetImageView(uint32 index) const;
        
    private:
        VkImage m_Texture;
        std::vector<VkImageView> m_Views;
        Texture2DDesc m_Desc;
        bool m_IsOwner;
    };
    
    
    inline VkImage VulkanTexture2D::GetResource() const
    {
        return m_Texture;
    }
    
    
    inline VkImageView VulkanTexture2D::GetImageView(uint32 index) const
    {
        return m_Views[index];
    }
    
    
    inline void VulkanTexture2D::SetImageView(VkImageView imageView)
    {
        if (m_Views.size() < 1)
        {
             m_Views.push_back(imageView);
        }
        else
        {
            m_Views[0] = imageView;
        }
    }
}
