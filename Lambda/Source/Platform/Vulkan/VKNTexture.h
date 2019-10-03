#pragma once
#include "Graphics/Core/ITexture.h"
#include "Graphics/Core/DeviceObjectBase.h"
#include "VKNAllocator.h"
#include "VKNConversions.inl"

namespace Lambda
{
	class VKNDevice;

    //----------
    //VKNTexture
    //----------
    
    class VKNTexture final : public DeviceObjectBase<VKNDevice, ITexture>
    {
        friend class VKNDevice;  
    public:
        LAMBDA_NO_COPY(VKNTexture);

        VKNTexture(VKNDevice* pDevice, IVKNAllocator* pAllocator, const TextureDesc& desc);
        VKNTexture(VKNDevice* pDevice, VkImage image, const TextureDesc& desc);
        ~VKNTexture();
        
        virtual void* GetNativeHandle() const override final;
        virtual const TextureDesc& GetDesc() const override final;

		inline void SetResolveResource(VKNTexture* pResolveResource) const	{ m_Desc.pResolveResource = pResolveResource; }
		inline void SetVkImageLayout(VkImageLayout resourceState) const		{ m_ImageLayout = resourceState; }
		inline VkFormat GetVkFormat() const									{ return ConvertResourceFormat(m_Desc.Format); }
		inline VKNTexture* GetResolveResource() const						{ return reinterpret_cast<VKNTexture*>(m_Desc.pResolveResource); }
		inline VkImageAspectFlags GetAspectFlags() const					{ return m_AspectFlags; }
		inline VkImage GetVkImage() const									{ return m_Image; }
		inline VkImageView GetVkImageView() const							{ return m_ImageView; }
		inline VkImageLayout GetVkImageLayout() const						{ return m_ImageLayout; }
    private:
        void Init(const TextureDesc& desc);
        void InitFromResource(VkImage image, const TextureDesc& desc);
        void CreateImageView(); 
    private:
		IVKNAllocator* const	m_pAllocator;
		VKNMemory				m_Memory;
        VkImage                 m_Image;
        VkImageView             m_ImageView;
        VkImageAspectFlags      m_AspectFlags;
        mutable TextureDesc     m_Desc;
        mutable VkImageLayout   m_ImageLayout;
        bool                    m_IsOwner;
    };
}
