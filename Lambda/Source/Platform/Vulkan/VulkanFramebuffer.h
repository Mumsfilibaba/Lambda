#pragma once
#include "LambdaPch.h"
#include <vulkan/vulkan.h>
#include <unordered_map>

namespace Lambda
{
    //Forward declarations
    class ITexture2D;
    
    
    //Struct for describing a framebuffer
    struct VulkanFramebufferInfo
    {
    public:
        inline bool operator==(const VulkanFramebufferInfo& other) const
        {
            //Does it contain the same amount of rendertargets?
            if (NumColorAttachments != other.NumColorAttachments)
                return false;
            
            //Are the rendertargets the same?
            for (uint32 i = 0; i < NumColorAttachments; i++)
            {
                if (ppColorAttachments[i] != other.ppColorAttachments[i])
                    return false;
            }
            
            //Are the renderpass and, depthbuffer and size the same?
            return  (RenderPass == other.RenderPass) && (pDepthStencil == other.pDepthStencil);
        }
        
        
        inline bool Contains(const ITexture2D* pTexture) const
        {
            //Check if this texture is amongst the rendertargets
            for (uint32 i = 0; i < NumColorAttachments; i++)
            {
                if (ppColorAttachments[i] == pTexture)
                    return true;
            }
            
            //Check if it is the depthbuffer
            return (pDepthStencil == pTexture);
        }
        
    public:
        VkRenderPass RenderPass             = VK_NULL_HANDLE;
        VkFramebuffer FrameBuffer           = VK_NULL_HANDLE;
        uint32 NumColorAttachments          = 0;
        const ITexture2D* pDepthStencil     = nullptr;
        const ITexture2D* ppColorAttachments[LAMBDA_RENDERTARGET_COUNT];
    };
    
    
    //Stores all framebuffers in the program
    class VulkanFramebufferCache final
    {
    public:
        LAMBDA_STATIC_CLASS(VulkanFramebufferCache);
        
        static VkFramebuffer GetFramebuffer(VkDevice device, VkRenderPass renderpass, const ITexture2D* const * const ppColorAttachments, uint32 numColorAttachments, const ITexture2D* const pDepthStencil);
        static void ReleaseAllContainingTexture(VkDevice device, const ITexture2D* pTexture);
        static void ReleaseAll(VkDevice device);
        
    private:
        static std::unordered_map<const ITexture2D*, VulkanFramebufferInfo> s_Framebuffers;
    };
    
    
    //Info about a renderpass
    struct VulkanRenderPassInfo
    {
    public:
        inline bool operator==(const VulkanRenderPassInfo& other)
        {
            //Check if all the format of the colorattachments are the same
            for (uint32 i = 0; i < NumColorAttachmentFormats; i++)
            {
                if (ColorAttachmentFormats[i] != other.ColorAttachmentFormats[i])
                    return false;
            }
            
            //Check the rest of the members
            return  (SampleCount == other.SampleCount) &&
                    (NumColorAttachmentFormats == other.NumColorAttachmentFormats) &&
                    (DepthStencilFormat == other.DepthStencilFormat);
        }
        
    public:
        VkRenderPass RenderPass             = VK_NULL_HANDLE;
        VkSampleCountFlagBits SampleCount   = VK_SAMPLE_COUNT_1_BIT;
        uint32 NumColorAttachmentFormats    = 0;
        VkFormat DepthStencilFormat        = VK_FORMAT_UNDEFINED;
        VkFormat ColorAttachmentFormats[LAMBDA_RENDERTARGET_COUNT];
    };
    
    
    //Stores all the renderpasses in the program
    class VulkanRenderPassCache final
    {
    public:
        LAMBDA_STATIC_CLASS(VulkanRenderPassCache);
        
        static VkRenderPass GetRenderPass(VkDevice device, VkFormat* pColorAttachmentFormats, uint32 numFormats, VkFormat depthStencilFormat, VkSampleCountFlagBits sampleCount);
        static void ReleaseAll(VkDevice device);
        
    private:
        static std::unordered_map<VkFormat, VulkanRenderPassInfo> s_RenderPasses;
    };
}
