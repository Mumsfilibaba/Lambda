#pragma once
#include "Graphics/Core/TextureBase.h"
#include "Memory/VKNDeviceAllocator.h"
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
		using TTexture = TextureBase<VKNDevice>;

    public:
        LAMBDA_NO_COPY(VKNTexture);

        VKNTexture(VKNDevice* pDevice, const ResourceData* pInitalData, const TextureDesc& desc);
        VKNTexture(VKNDevice* pDevice, VkImage image, const TextureDesc& desc);
        ~VKNTexture();
        
        virtual void* GetNativeHandle() const override final;
		virtual void SetName(const char* pName) override final;

		inline VkFormat GetVkFormat() const	{ return ConvertFormat(m_Desc.Format); }
		inline VkImageAspectFlags GetVkAspectFlags() const { return m_VkAspectFlags; }
		inline VkImage GetVkImage() const		  { return m_VkImage; }
		inline VkImageView GetVkImageView() const { return m_ImageView; }
    private:
        void Init(const ResourceData* pInitalData, const TextureDesc& desc);
        void InitFromResource(VkImage image, const TextureDesc& desc);
        void CreateImageView(); 
    private:
		VKNAllocation m_Memory;
        VkImage     m_VkImage;
        VkImageView m_ImageView;
        VkImageAspectFlags m_VkAspectFlags;
        bool m_IsOwner;
    };
}
