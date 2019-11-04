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

        VKNTexture(VKNDevice* pVkDevice, const ResourceData* pInitalData, const TextureDesc& desc);
        VKNTexture(VKNDevice* pVkDevice, VkImage image, const TextureDesc& desc);
        ~VKNTexture();
        
        virtual void* GetNativeHandle() const override final;
		virtual void SetName(const char* pName) override final;

		
		_forceinline VkFormat GetVkFormat() const	
		{ 
			return ConvertFormat(m_Desc.Format); 
		}
		
		
		_forceinline VkImageAspectFlags GetVkAspectFlags() const 
		{ 
			return m_VkAspectFlags; 
		}
		
		
		_forceinline VkImage GetVkImage() const		  
		{ 
			return m_VkImage; 
		}
		
		
		_forceinline VkImageView GetVkImageView() const 
		{ 
			return m_VkImageView; 
		}
    private:
        void Init(const ResourceData* pInitalData, const TextureDesc& desc);
        void InitFromResource(VkImage image, const TextureDesc& desc);
        void CreateImageView(); 
    private:
		VKNAllocation m_Memory;
        VkImage m_VkImage;
        VkImageView m_VkImageView;
        VkImageAspectFlags m_VkAspectFlags;
        bool m_IsOwner;
    };
}
