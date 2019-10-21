#pragma once
#include "Graphics/Core/TextureBase.h"
#include "VKNAllocator.h"
#include "VKNConversions.inl"

namespace Lambda
{
	class VKNDevice;

    //----------
    //VKNTexture
    //----------
    
    class VKNTexture final : public TextureBase<VKNDevice>
    {
        friend class VKNDevice;

    public:
        LAMBDA_NO_COPY(VKNTexture);

        VKNTexture(VKNDevice* pDevice, const TextureDesc& desc);
        VKNTexture(VKNDevice* pDevice, VkImage image, const TextureDesc& desc);
        ~VKNTexture();
        
        virtual void* GetNativeHandle() const override final;

		inline VkFormat GetVkFormat() const	{ return ConvertFormat(m_Desc.Format); }
		inline VkImageAspectFlags GetVkAspectFlags() const { return m_AspectFlags; }
		inline VkImage GetVkImage() const		  { return m_Image; }
		inline VkImageView GetVkImageView() const { return m_ImageView; }
    private:
        void Init(const TextureDesc& desc);
        void InitFromResource(VkImage image, const TextureDesc& desc);
        void CreateImageView(); 
    private:
		VKNAllocation m_Memory;
        VkImage     m_Image;
        VkImageView m_ImageView;
        VkImageAspectFlags m_AspectFlags;
        bool m_IsOwner;
    };
}
