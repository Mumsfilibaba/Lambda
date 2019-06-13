#include "LambdaPch.h"
#include "VulkanGraphicsDevice.h"

namespace Lambda
{
    //Graphicsdevice
    IGraphicsDevice* IGraphicsDevice::s_pInstance = nullptr;
    
    IGraphicsDevice* IGraphicsDevice::Create(IWindow* pWindow, const GraphicsDeviceDesc& desc)
    {
        if (desc.Api == GRAPHICS_API_D3D12)
        {
            LOG_DEBUG_ERROR("Lambda Engine: D3D12 is only supported on the Windows-Platform\n");
            return nullptr;
        }
        else if (desc.Api == GRAPHICS_API_VULKAN)
        {
            LOG_DEBUG_ERROR("Lambda Engine: Creating GraphicsDevice\n");
            return DBG_NEW VulkanGraphicsDevice(pWindow, desc);
        }
        
        return nullptr;
    }
    
    
    IGraphicsDevice* IGraphicsDevice::GetInstance()
    {
        return s_pInstance;
    }
    
    
    bool IGraphicsDevice::OnEvent(const Event& event)
    {
        return (s_pInstance) ? s_pInstance->InternalOnEvent(event) : false;
    }
    
    
    VulkanGraphicsDevice::VulkanGraphicsDevice(IWindow* pWindow, const GraphicsDeviceDesc& desc)
    {
    }
    
    
    void VulkanGraphicsDevice::CreateCommandList(ICommandList** ppList, CommandListType type) const
    {
        
    }
    
    
    void VulkanGraphicsDevice::CreateBuffer(IBuffer** ppBuffer, const ResourceData* pInitalData, const BufferDesc& desc) const
    {
        
    }
    
    
    void VulkanGraphicsDevice::CreateTexture2D(ITexture2D** ppTexture, const ResourceData* pInitalData, const Texture2DDesc& desc) const
    {
        
    }
    
    
    void VulkanGraphicsDevice::CreateShader(IShader** ppShader, const ShaderDesc& desc) const
    {
        
    }
    
    
    void VulkanGraphicsDevice::CreateSamplerState(ISamplerState** ppSamplerState, const SamplerDesc& desc) const
    {
        
    }
    
    
    void VulkanGraphicsDevice::CreateGraphicsPipelineState(IGraphicsPipelineState** ppPSO, const GraphicsPipelineStateDesc& desc) const
    {
        
    }
    
    
    
    void VulkanGraphicsDevice::DestroyCommandList(ICommandList** ppList) const
    {
        
    }
    
    
    void VulkanGraphicsDevice::DestroyBuffer(IBuffer** ppBuffer) const
    {
        
    }
    
    
    void VulkanGraphicsDevice::DestroyTexture2D(ITexture2D** ppTexture) const
    {
        
    }
    
    
    void VulkanGraphicsDevice::DestroyShader(IShader** ppShader) const
    {
        
    }
    
    
    void VulkanGraphicsDevice::DestroySamplerState(ISamplerState** ppSamplerState) const
    {
        
    }
    
    
    void VulkanGraphicsDevice::DestroyGraphicsPipelineState(IGraphicsPipelineState** ppPSO) const
    {
        
    }
    
    
    void VulkanGraphicsDevice::Destroy() const
    {
        
    }
    
    
    void VulkanGraphicsDevice::ExecuteCommandList(ICommandList* const * ppLists, uint32 numLists) const
    {
        
    }
    
    
    void VulkanGraphicsDevice::Present(uint32 verticalSync) const
    {
        
    }
    
    
    void VulkanGraphicsDevice::GPUWaitForFrame() const
    {
        
    }
    
    
    void VulkanGraphicsDevice::WaitForGPU() const
    {
        
    }

    
    void* VulkanGraphicsDevice::GetNativeHandle() const
    {
        return nullptr;
    }
    
    
    ITexture2D* VulkanGraphicsDevice::GetCurrentRenderTarget()
    {
        return nullptr;
    }
    
    
    uint32 VulkanGraphicsDevice::GetCurrentBackBufferIndex() const
    {
        return 0;
    }
    
    
    bool VulkanGraphicsDevice::InternalOnEvent(const Event& event)
    {
        return false;
    }
}
