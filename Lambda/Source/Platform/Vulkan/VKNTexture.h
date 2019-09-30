#pragma once
#include "Graphics/ITexture.h"
#include "VKNAllocator.h"
#include "VKNConversions.inl"

namespace Lambda
{
	class VKNDevice;

    //----------
    //VKNTexture
    //----------
    
    class VKNTexture final : public RefCountedObject<ITexture>
    {
        friend class VKNDevice;  
    public:
        LAMBDA_NO_COPY(VKNTexture);

        VKNTexture(VKNDevice* pDevice, IVKNAllocator* pAllocator, const TextureDesc& desc);
        VKNTexture(VKNDevice* pDevice, VkImage image, const TextureDesc& desc);
        ~VKNTexture();
        
        virtual void* GetNativeHandle() const override final;
        virtual const TextureDesc& GetDesc() const override final;

        void SetResolveResource(VKNTexture* pResolveResource) const;
        void SetResourceState(VkImageLayout resourceState) const;
        VkFormat GetFormat() const;
        VKNTexture*	GetResolveResource() const;
        VkImageAspectFlags GetAspectFlags() const;
        VkImageView GetImageView() const;
        VkImageLayout GetResourceState() const; 
    private:
        void Init(const TextureDesc& desc);
        void InitFromResource(VkImage image, const TextureDesc& desc);
        void CreateImageView(); 
    private:
		VKNDevice*				m_pDevice;
		IVKNAllocator* const	m_pAllocator;
		VKNMemory				m_Memory;
        VkImage                 m_Image;
        VkImageView             m_ImageView;
        VkImageAspectFlags      m_AspectFlags;
        mutable TextureDesc     m_Desc;
        mutable VkImageLayout   m_ResourceState;
        bool                    m_IsOwner;
    };
    
    
    inline VkImageAspectFlags VKNTexture::GetAspectFlags() const
    {
        return m_AspectFlags;
    }
    
        
    inline VkImageView VKNTexture::GetImageView() const
    {
        return m_ImageView;
    }
    
    
    inline VkImageLayout VKNTexture::GetResourceState() const
    {
        return m_ResourceState;
    }
    
    
    inline void VKNTexture::SetResolveResource(VKNTexture* pResolveResource) const
    {
        m_Desc.pResolveResource = pResolveResource;
    }
    
    
    inline void VKNTexture::SetResourceState(VkImageLayout resourceState) const
    {
        m_ResourceState = resourceState;
    }
    
    
    inline VKNTexture* VKNTexture::GetResolveResource() const
    {
        return reinterpret_cast<VKNTexture*>(m_Desc.pResolveResource);
    }
    
    
    inline VkFormat VKNTexture::GetFormat() const
    {
        return ConvertResourceFormat(m_Desc.Format);
    }
}
