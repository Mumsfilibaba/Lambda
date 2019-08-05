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
            if (NumRenderTargets != other.NumRenderTargets)
                return false;
            
            //Are the rendertargets the same?
            for (uint32 i = 0; i < NumRenderTargets; i++)
            {
                if (ppRenderTargets[i] != other.ppRenderTargets[i])
                    return false;
            }
            
            //Are the renderpass and, depthbuffer and size the same
            return  (RenderPass == other.RenderPass) && (pDepthStencil == other.pDepthStencil) &&
                    (Width == other.Width) && (Height == other.Height);
        }
        
        
        inline bool Contains(const ITexture2D* pTexture) const
        {
            //Check if this texture is amongst the rendertargets
            for (uint32 i = 0; i < NumRenderTargets; i++)
            {
                if (ppRenderTargets[i] == pTexture)
                    return true;
            }
            
            //Check if it is the depthbuffer
            return pDepthStencil == pTexture;
        }
        
    public:
        const ITexture2D* ppRenderTargets[LAMBDA_RENDERTARGET_COUNT];
        uint32 NumRenderTargets             = 0;
        const ITexture2D* pDepthStencil     = nullptr;
        VkRenderPass RenderPass             = VK_NULL_HANDLE;
        VkFramebuffer FrameBuffer           = VK_NULL_HANDLE;
        uint32 Width                        = 0;
        uint32 Height                       = 0;
    };
    
    
    //Stores all framebuffers in the program
    class VulkanFramebufferCache final
    {
    public:
        LAMBDA_STATIC_CLASS(VulkanFramebufferCache);
        
        static VkFramebuffer GetFramebuffer(VkDevice device, VkRenderPass renderpass, const ITexture2D* const * const ppRendertargets, uint32 numRenderTargets, const ITexture2D* const pDepthStencil);
        static void ReleaseTexture(VkDevice device, const ITexture2D* pTexture);
        static void Release(VkDevice device);
        
    private:
        static std::unordered_map<const ITexture2D*, VulkanFramebufferInfo> s_Framebuffers;
    };
}
