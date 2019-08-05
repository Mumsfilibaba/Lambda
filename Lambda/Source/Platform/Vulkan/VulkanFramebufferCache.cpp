#include "VulkanFramebufferCache.h"
#include "VulkanGraphicsDevice.h"
#include "VulkanTexture2D.h"

namespace Lambda
{
    //Static variable declarations
    std::unordered_map<const ITexture2D*, VulkanFramebufferInfo> VulkanFramebufferCache::s_Framebuffers = std::unordered_map<const ITexture2D*, VulkanFramebufferInfo>();
    
    
    //VulkanFramebufferCache
    VkFramebuffer VulkanFramebufferCache::GetFramebuffer(VkDevice device, VkRenderPass renderPass, const ITexture2D* const * const ppRenderTargets, uint32 numRenderTargets, const ITexture2D* const pDepthStencil)
    {
        assert(device != VK_NULL_HANDLE);
        
        //Get key
        const ITexture2D* pKey = nullptr;
        if (ppRenderTargets)
        {
            pKey = ppRenderTargets[0];
        }
        else
        {
            pKey = pDepthStencil;
        }

        
        //If the key is nullptr return
        if (!pKey)
        {
            return VK_NULL_HANDLE;
        }

        
        //Info about the bound framebuffers
        VulkanFramebufferInfo fbInfo = {};
        fbInfo.RenderPass       = renderPass;
        fbInfo.Width            = pKey->GetWidth();
        fbInfo.Height           = pKey->GetHeight();
        fbInfo.pDepthStencil    = pDepthStencil;
        fbInfo.NumRenderTargets = numRenderTargets;
        memcpy(fbInfo.ppRenderTargets, ppRenderTargets, sizeof(const ITexture2D*) * numRenderTargets);
        
        
        //Check if a framebuffer exists
        auto range = s_Framebuffers.equal_range(pKey);
        for (auto i = range.first; i != range.second; i++)
        {
            //Find the matching framebuffer
            auto& value = i->second;
            if (value == fbInfo)
                return value.FrameBuffer;
        }
        
        
        //Loop through all textures and get the imageview, and check dimensions
        std::vector<VkImageView> attachments;
        for (uint32 i = 0; i < numRenderTargets; i++)
        {
            //Add attachment
            attachments.push_back(reinterpret_cast<const VulkanTexture2D*>(ppRenderTargets[i])->GetImageView());
            
            //Check dimensions
            if (ppRenderTargets[i]->GetWidth() != fbInfo.Width || ppRenderTargets[i]->GetHeight() != fbInfo.Height)
            {
                LOG_DEBUG_ERROR("Vulkan: Dimensions on textures are not equal\n");
                return VK_NULL_HANDLE;
            }
        }
        
        //Setup depthbuffer
        if (pDepthStencil)
        {
            //Add attachment
            attachments.push_back(reinterpret_cast<const VulkanTexture2D*>(pDepthStencil)->GetImageView());
            
            //Check dimensions
            if (pDepthStencil->GetWidth() != fbInfo.Width || pDepthStencil->GetHeight() != fbInfo.Height)
            {
                LOG_DEBUG_ERROR("Vulkan: Dimensions on textures are not equal\n");
                return VK_NULL_HANDLE;
            }
        }
        
        //Setup new framebuffer
        VkFramebufferCreateInfo info = {};
        info.sType              = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        info.pNext              = nullptr;
        info.renderPass         = renderPass;
        info.attachmentCount    = uint32(attachments.size());
        info.pAttachments       = attachments.data();
        info.width              = fbInfo.Width;
        info.height             = fbInfo.Height;
        info.layers             = 1;
        
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
            fbInfo.FrameBuffer = framebuffer;
            s_Framebuffers.insert(std::pair<const ITexture2D*, VulkanFramebufferInfo>(pKey, fbInfo));
            return framebuffer;
        }
    }
    
    
    void VulkanFramebufferCache::ReleaseTexture(VkDevice device, const ITexture2D* pTexture)
    {
        //Check if a framebuffer exists
        auto range = s_Framebuffers.equal_range(pTexture);
        for (auto i = range.first; i != range.second; i++)
        {
            //Find the matching framebuffer
            if (i->second.Contains(pTexture))
            {
                //Destroy the framebuffer in vulkan
                vkDestroyFramebuffer(device, i->second.FrameBuffer, nullptr);
                i->second.FrameBuffer = VK_NULL_HANDLE;
                
                //Erase from map
                s_Framebuffers.erase(i);
            }
        }
    }
    
    
    void VulkanFramebufferCache::Release(VkDevice device)
    {
        for (auto& buffer : s_Framebuffers)
        {
            //Destroy the framebuffer in vulkan
            vkDestroyFramebuffer(device, buffer.second.FrameBuffer, nullptr);
            buffer.second.FrameBuffer = VK_NULL_HANDLE;
        }
        
        s_Framebuffers.clear();
    }
}
