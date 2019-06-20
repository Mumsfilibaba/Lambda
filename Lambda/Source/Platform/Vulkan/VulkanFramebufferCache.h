#pragma once
#include "LambdaPch.h"
#include <vulkan/vulkan.h>
#include <unordered_map>

namespace Lambda
{
    //Forward declarations
    class ITexture2D;
    
    //Stores all framebuffers in the program
    class VulkanFramebufferCache final
    {
    public:
        LAMBDA_STATIC_CLASS(VulkanFramebufferCache);
        
        static VkFramebuffer GetFramebuffer(VkDevice device, VkRenderPass renderpass, const ITexture2D* const * const ppRendertargets, uint32 numRenderTargets, const ITexture2D* const pDepthStencil);
        static void Release(VkDevice device);
        
    private:
        static std::unordered_map<const ITexture2D*, VkFramebuffer> s_Framebuffers;
    };
}
