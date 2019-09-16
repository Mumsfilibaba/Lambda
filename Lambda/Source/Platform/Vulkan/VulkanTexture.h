#pragma once
#include "Graphics/ITexture.h"
#include "VulkanAllocator.h"
#include "VulkanConversions.inl"

namespace Lambda
{
    //-------------
    //VulkanTexture
    //-------------
    
    class VulkanTexture final : public ITexture
    {
        friend class VulkanGraphicsDevice;
        
    public:
        LAMBDA_NO_COPY(VulkanTexture);

        VulkanTexture(VkDevice device, IVulkanAllocator* pAllocator, const TextureDesc& desc);
        VulkanTexture(VkDevice device, VkImage image, const TextureDesc& desc);
        ~VulkanTexture() = default;
        
        virtual void*       GetNativeHandle() const override final;
        virtual TextureDesc GetDesc() const override final;
        
		void Release(VkDevice device);
        void Destroy(VkDevice device);

        void SetResolveResource(VulkanTexture* pResolveResource) const;
        void SetResourceState(VkImageLayout resourceState) const;
        VkFormat            GetFormat() const;
        VulkanTexture*      GetResolveResource() const;
        VkImageAspectFlags  GetAspectFlags() const;
        VkImageView         GetImageView() const;
        VkImageLayout       GetResourceState() const;
        
    private:
        void Init(VkDevice device, const TextureDesc& desc);
        void InitFromResource(VkDevice device, VkImage image, const TextureDesc& desc);
        void CreateImageView(VkDevice device);
        
    private:
		IVulkanAllocator* const m_pAllocator;
		VulkanMemory            m_Memory;
        VkImage                 m_Image;
        VkImageView             m_View;
        VkImageAspectFlags      m_AspectFlags;
        mutable TextureDesc     m_Desc;
        mutable VkImageLayout   m_ResourceState;
        bool                    m_IsOwner;
    };
    
    
    inline VkImageAspectFlags VulkanTexture::GetAspectFlags() const
    {
        return m_AspectFlags;
    }
    
        
    inline VkImageView VulkanTexture::GetImageView() const
    {
        return m_View;
    }
    
    
    inline VkImageLayout VulkanTexture::GetResourceState() const
    {
        return m_ResourceState;
    }
    
    
    inline void VulkanTexture::SetResolveResource(VulkanTexture* pResolveResource) const
    {
        m_Desc.pResolveResource = pResolveResource;
    }
    
    
    inline void VulkanTexture::SetResourceState(VkImageLayout resourceState) const
    {
        m_ResourceState = resourceState;
    }
    
    
    inline VulkanTexture* VulkanTexture::GetResolveResource() const
    {
        return reinterpret_cast<VulkanTexture*>(m_Desc.pResolveResource);
    }
    
    
    inline VkFormat VulkanTexture::GetFormat() const
    {
        return ConvertResourceFormat(m_Desc.Format);
    }
}
