#include "VulkanFramebufferCache.h"
#include "VulkanGraphicsDevice.h"
#include "VulkanTexture2D.h"

namespace Lambda
{
    //Static variable declarations
    std::unordered_map<const ITexture2D*, VkFramebuffer> VulkanFramebufferCache::s_Framebuffers = std::unordered_map<const ITexture2D*, VkFramebuffer>();
    
    //VulkanFramebufferCache
    VkFramebuffer VulkanFramebufferCache::GetFramebuffer(VkDevice device, VkRenderPass renderpass, const ITexture2D* const * const ppRendertargets, uint32 numRenderTargets, const ITexture2D* const pDepthStencil)
    {
        assert(device != VK_NULL_HANDLE);
        
        //Use first texture as key
        const ITexture2D* pKey = ppRendertargets[0];
        
        //Check if a framebuffer exists
        auto value = s_Framebuffers.find(pKey);
        if (value != s_Framebuffers.end())
        {
            return value->second;
        }
        
        //Framebuffer size, all textures must have the same size
        uint32 width    = 0;
        uint32 height   = 0;
        
        //Loop through all textures and get the imageview, and check dimensions
        std::vector<VkImageView> attachments;
        for (uint32 i = 0; i < numRenderTargets; i++)
        {
            //Add attachment
            attachments.push_back(reinterpret_cast<const VulkanTexture2D*>(ppRendertargets[i])->GetImageView());
            
            //Check dimensions
            Texture2DDesc desc = ppRendertargets[i]->GetDesc();
            if (width + height != 0)
            {
                if (desc.Width != width || desc.Height != height)
                {
                    LOG_DEBUG_ERROR("Vulkan: Dimensions on textures are not equal\n");
                    return VK_NULL_HANDLE;
                }
            }
            else
            {
                width   = desc.Width;
                height  = desc.Height;
            }
        }
        
        //Setup depthbuffer
        if (pDepthStencil)
        {
            //Add attachment
            attachments.push_back(reinterpret_cast<const VulkanTexture2D*>(pDepthStencil)->GetImageView());
            
            //Check dimensions
            Texture2DDesc desc = pDepthStencil->GetDesc();
            if (width + height != 0)
            {
                if (desc.Width != width || desc.Height != height)
                {
                    LOG_DEBUG_ERROR("Vulkan: Dimensions on textures are not equal\n");
                    return VK_NULL_HANDLE;
                }
            }
            else
            {
                width   = desc.Width;
                height  = desc.Height;
            }
        }
        
        //Setup new framebuffer
        VkFramebufferCreateInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        info.pNext = nullptr;
        info.renderPass = renderpass;
        info.attachmentCount = uint32(attachments.size());
        info.pAttachments = attachments.data();
        info.width = width;
        info.height = height;
        info.layers = 1;
        
        //Create new framebuffer
        VkFramebuffer framebuffer = VK_NULL_HANDLE;
        if (vkCreateFramebuffer(device, &info, nullptr, &framebuffer) != VK_SUCCESS)
        {
            LOG_DEBUG_ERROR("Vulkan: Failed to create Framebuffer\n");
            return VK_NULL_HANDLE;
        }
        else
        {
            LOG_DEBUG_INFO("Vulkan: Created new Framebuffer\n");
            
            //Insert new framebuffer
            s_Framebuffers.insert(std::pair<const ITexture2D*, VkFramebuffer>(pKey, framebuffer));
            return framebuffer;
        }
    }
    
    
    void VulkanFramebufferCache::ReleaseTexture(VkDevice device, const ITexture2D* pTexture)
    {
        //Release framebuffer if it exists
        auto value = s_Framebuffers.find(pTexture);
        if (value != s_Framebuffers.end())
        {
            LOG_DEBUG_INFO("Vulkan: Deleting Framebuffer '%p'\n", value->second);
            
            //Destroy the framebuffer in vulkan
            vkDestroyFramebuffer(device, value->second, nullptr);
            value->second = VK_NULL_HANDLE;
            
            //Delete it from the framebuffers
            s_Framebuffers.erase(value);
        }

    }
    
    
    void VulkanFramebufferCache::Release(VkDevice device)
    {
        LOG_DEBUG_INFO("Vulkan: Num framebuffers before '%u'\n", s_Framebuffers.size());
        
        for (auto& buffer : s_Framebuffers)
        {
            LOG_DEBUG_INFO("Vulkan: Deleting Framebuffer '%p'\n", buffer.second);
            
            //Destroy the framebuffer in vulkan
            vkDestroyFramebuffer(device, buffer.second, nullptr);
            buffer.second = VK_NULL_HANDLE;
        }
        
        s_Framebuffers.clear();
        LOG_DEBUG_INFO("Vulkan: Num framebuffers after '%u'\n", s_Framebuffers.size());
    }
}
