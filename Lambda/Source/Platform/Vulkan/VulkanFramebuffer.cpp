#include "LambdaPch.h"
#include "VulkanFramebuffer.h"
#include "VulkanGraphicsDevice.h"
#include "VulkanTexture2D.h"

namespace Lambda
{
    //Static variable declarations
    std::unordered_map<const ITexture2D*, VulkanFramebufferInfo> VulkanFramebufferCache::s_Framebuffers = std::unordered_map<const ITexture2D*, VulkanFramebufferInfo>();
    std::unordered_map<VkFormat, VulkanRenderPassInfo> VulkanRenderPassCache::s_RenderPasses = std::unordered_map<VkFormat, VulkanRenderPassInfo>();
    
    
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

        
        //Size of the framebuffer
        uint32 width  = pKey->GetWidth();
        uint32 height = pKey->GetHeight();
        
        
        //Info about the bound framebuffer
        VulkanFramebufferInfo fbInfo = {};
        fbInfo.RenderPass       = renderPass;
        fbInfo.pDepthStencil    = pDepthStencil;
        fbInfo.NumColorAttachments = numRenderTargets;
        memcpy(fbInfo.ppColorAttachments, ppRenderTargets, sizeof(const ITexture2D*) * numRenderTargets);
        
        
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
            if (ppRenderTargets[i]->GetWidth() != width || ppRenderTargets[i]->GetHeight() != height)
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
            if (pDepthStencil->GetWidth() != width || pDepthStencil->GetHeight() != height)
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
        info.width              = width;
        info.height             = height;
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
    
    
    void VulkanFramebufferCache::ReleaseAllContainingTexture(VkDevice device, const ITexture2D* pTexture)
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
    
    
    void VulkanFramebufferCache::ReleaseAll(VkDevice device)
    {
        for (auto& buffer : s_Framebuffers)
        {
            //Destroy the framebuffer in vulkan
            vkDestroyFramebuffer(device, buffer.second.FrameBuffer, nullptr);
            buffer.second.FrameBuffer = VK_NULL_HANDLE;
        }
        
        s_Framebuffers.clear();
    }
    
    
    //Vulkan renderpass cache
    VkRenderPass VulkanRenderPassCache::GetRenderPass(VkDevice device, VkFormat* pColorAttachmentFormats, uint32 numColorAttachmentFormats, VkFormat depthStencilFormat, VkSampleCountFlagBits sampleCount)
    {
        assert(device != VK_NULL_HANDLE);
        assert(pColorAttachmentFormats != nullptr && numColorAttachmentFormats > 0);
        
        //Must have atleast one attachment
        if (numColorAttachmentFormats < 1 && depthStencilFormat == VK_FORMAT_UNDEFINED)
        {
            return VK_NULL_HANDLE;
        }
        
        //Setup vulkanrenderpassinfo
        VulkanRenderPassInfo rpInfo = {};
        rpInfo.DepthStencilFormat = depthStencilFormat;
        rpInfo.NumColorAttachmentFormats = numColorAttachmentFormats;
        for (uint32 i = 0; i < numColorAttachmentFormats; i++)
            rpInfo.ColorAttachmentFormats[i] = pColorAttachmentFormats[i];
        
        //Setup key and check cache
        VkFormat key = (numColorAttachmentFormats > 0) ? pColorAttachmentFormats[0] : depthStencilFormat;
        
        //Check if a renderpass exists
        auto range = s_RenderPasses.equal_range(key);
        for (auto i = range.first; i != range.second; i++)
        {
            //Find the matching renderpass
            auto& value = i->second;
            if (value == rpInfo)
                return value.RenderPass;
        }
        
        //Setup color attachments
        std::vector<VkAttachmentReference> colorAttachentRefs;
        std::vector<VkAttachmentDescription> attachments;
        for (uint32 i = 0; i < numColorAttachmentFormats; i++)
        {
            //Setup attachments
            VkAttachmentDescription colorAttachment = {};
            colorAttachment.flags           = 0;
            colorAttachment.format          = pColorAttachmentFormats[i];
            colorAttachment.samples         = sampleCount;
            colorAttachment.loadOp          = VK_ATTACHMENT_LOAD_OP_LOAD;
            colorAttachment.storeOp         = VK_ATTACHMENT_STORE_OP_STORE;
            colorAttachment.stencilLoadOp   = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
            colorAttachment.stencilStoreOp  = VK_ATTACHMENT_STORE_OP_DONT_CARE;
            colorAttachment.initialLayout   = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
            colorAttachment.finalLayout     = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
            attachments.push_back(colorAttachment);
            
            //Descripe attachment bindpoint
            VkAttachmentReference colorAttachmentRef = {};
            colorAttachmentRef.attachment   = i;
            colorAttachmentRef.layout       = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
            colorAttachentRefs.push_back(colorAttachmentRef);
            
        }
        
        //Describe subpass
        VkSubpassDescription subpass = {};
        subpass.flags                       = 0;
        subpass.pipelineBindPoint           = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.colorAttachmentCount        = uint32(colorAttachentRefs.size());
        subpass.pColorAttachments           = colorAttachentRefs.data();
        subpass.preserveAttachmentCount     = 0;
        subpass.pPreserveAttachments        = nullptr;
        subpass.inputAttachmentCount        = 0;
        subpass.pInputAttachments           = nullptr;
        subpass.pResolveAttachments         = nullptr;
        
        //Setup depthstencil
        VkAttachmentReference depthAttachmentRef = {};
        if (depthStencilFormat == VK_FORMAT_UNDEFINED)
        {
            subpass.pDepthStencilAttachment = nullptr;
        }
        else
        {
            //Setup attachments
            VkAttachmentDescription depthAttachment = {};
            depthAttachment.flags           = 0;
            depthAttachment.format          = depthStencilFormat;
            depthAttachment.samples         = sampleCount;
            depthAttachment.loadOp          = VK_ATTACHMENT_LOAD_OP_LOAD;
            depthAttachment.storeOp         = VK_ATTACHMENT_STORE_OP_STORE;
            depthAttachment.stencilLoadOp   = VK_ATTACHMENT_LOAD_OP_LOAD;
            depthAttachment.stencilStoreOp  = VK_ATTACHMENT_STORE_OP_STORE;
            depthAttachment.initialLayout   = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
            depthAttachment.finalLayout     = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
            attachments.push_back(depthAttachment);
            
            //Setup ref
            depthAttachmentRef.attachment   = uint32(attachments.size() - 1);
            depthAttachmentRef.layout       = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
            
            //Set attachment
            subpass.pDepthStencilAttachment = &depthAttachmentRef;
        }
        
        
        //Setup renderpass
        VkRenderPassCreateInfo renderPassInfo = {};
        renderPassInfo.sType            = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassInfo.flags            = 0;
        renderPassInfo.pNext            = nullptr;
        renderPassInfo.attachmentCount  = uint32(attachments.size());
        renderPassInfo.pAttachments     = attachments.data();
        renderPassInfo.subpassCount     = 1;
        renderPassInfo.pSubpasses       = &subpass;
        renderPassInfo.pDependencies    = nullptr;
        
        //Create renderpass
        VkRenderPass renderPass = VK_NULL_HANDLE;
        if (vkCreateRenderPass(device, &renderPassInfo, nullptr, &renderPass) != VK_SUCCESS)
        {
            LOG_DEBUG_ERROR("Vulkan: Failed to create renderpass\n");
            return VK_NULL_HANDLE;
        }
        else
        {
            LOG_DEBUG_INFO("Vulkan: Created renderpass\n");
            
            //Insert the new renderpass
            rpInfo.RenderPass = renderPass;
            s_RenderPasses.insert(std::pair<VkFormat, VulkanRenderPassInfo>(key, rpInfo));
            return renderPass;
        }
    }
}
