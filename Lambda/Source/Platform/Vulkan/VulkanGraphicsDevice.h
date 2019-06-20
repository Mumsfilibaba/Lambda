#pragma once
#include "Graphics/IGraphicsDevice.h"
#include <vulkan/vulkan.h>

namespace Lambda
{
    //Helperstruct for when finding queuefamilies
    struct QueueFamilyIndices
    {
        int32 GraphicsFamily    = -1;
        int32 PresentFamily     = -1;
        
        inline bool Valid()
        {
            return (GraphicsFamily >= 0) && (PresentFamily >= 0);
        }
    };
    
    
    //SwapChain support helper struct
    struct SwapChainCapabilities
    {
        VkSurfaceCapabilitiesKHR Capabilities;
        std::vector<VkSurfaceFormatKHR> Formats;
        std::vector<VkPresentModeKHR> PresentModes;
        
        inline bool Valid()
        {
            return !Formats.empty() && !PresentModes.empty();
        }
    };
    
    
    //Forward declarations
    class VulkanTexture2D;
    
    //Vulkan implementation of graphics device
    class VulkanGraphicsDevice final : public IGraphicsDevice
    {
    public:
        LAMBDA_NO_COPY(VulkanGraphicsDevice);
        
        VulkanGraphicsDevice(IWindow* pWindow, const GraphicsDeviceDesc& desc);
        ~VulkanGraphicsDevice();
        
        virtual void CreateCommandList(ICommandList** ppList, CommandListType type) const override final;
        virtual void CreateBuffer(IBuffer** ppBuffer, const ResourceData* pInitalData, const BufferDesc& desc) const override final;
        virtual void CreateTexture2D(ITexture2D** ppTexture, const ResourceData* pInitalData, const Texture2DDesc& desc) const override final;
        virtual void CreateShader(IShader** ppShader, const ShaderDesc& desc) const override final;
        virtual void CreateSamplerState(ISamplerState** ppSamplerState, const SamplerDesc& desc) const override final;
        virtual void CreateGraphicsPipelineState(IGraphicsPipelineState** ppPSO, const GraphicsPipelineStateDesc& desc) const override final;
        
        virtual void DestroyCommandList(ICommandList** ppList) const override final;
        virtual void DestroyBuffer(IBuffer** ppBuffer) const override final;
        virtual void DestroyTexture2D(ITexture2D** ppTexture) const override final;
        virtual void DestroyShader(IShader** ppShader) const override final;
        virtual void DestroySamplerState(ISamplerState** ppSamplerState) const override final;
        virtual void DestroyGraphicsPipelineState(IGraphicsPipelineState** ppPSO) const override final;
        virtual void Destroy() const override final;
        
        virtual void ExecuteCommandList(ICommandList* const * ppLists, uint32 numLists) const override final;
        virtual void Present(uint32 verticalSync) const override final;
        virtual void GPUWaitForFrame() const override final;
        virtual void WaitForGPU() const override final;
        
        virtual void* GetNativeHandle() const override final;
        virtual ITexture2D* GetCurrentRenderTarget() override final;
        virtual uint32 GetCurrentBackBufferIndex() const override final;
    
        VkFormat GetBackBufferFormat() const;
        QueueFamilyIndices GetQueueFamilyIndices() const;
        
    private:
        void Init(IWindow* pWindow, const GraphicsDeviceDesc& desc);
        void InitDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo);
        
        bool CreateInstance(const GraphicsDeviceDesc& desc);
        bool CreateDebugDebugMessenger(const GraphicsDeviceDesc& desc);
        bool QueryAdapter(const GraphicsDeviceDesc& desc);
        bool CreateDeviceAndQueues(const GraphicsDeviceDesc& desc);
        bool CreateSurface(IWindow* pWindow);
        bool CreateSwapChain(uint16 width, uint16 height);
        bool CreateTextures();
        bool CreateSemaphores();
        
        void ReleaseSwapChain();
        
        bool AdapterIsSuitable(VkPhysicalDevice adapter, const GraphicsDeviceDesc& desc);
        SwapChainCapabilities QuerySwapChainSupport(VkPhysicalDevice adapter);
        QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice adapter); //Returns -1 on failure, otherwise index of queuefamily
        
        std::vector<const char*> GetRequiredValidationLayers(bool debug);
        std::vector<const char*> GetRequiredDeviceExtensions(bool debug);
        
        virtual bool InternalOnEvent(const Event& event) override final;
        
    private:
        VkInstance m_Instance;
        VkDebugUtilsMessengerEXT m_DebugMessenger;
        VkDevice m_Device;
        VkQueue m_GraphicsQueue;
        VkQueue m_PresentationQueue;
        VkSemaphore m_ImageSemaphore;
        VkSemaphore m_RenderSemaphore;
        QueueFamilyIndices m_FamiliyIndices;
        
        VkPhysicalDevice m_Adapter;
        VkPhysicalDeviceProperties m_AdapterProperties;
        
        VkSurfaceKHR m_Surface;
        VkSwapchainKHR m_SwapChain;
        VkFormat m_SwapChainFormat;
        VkExtent2D m_SwapChainSize;
        
        mutable uint32 m_CurrentBackbufferIndex;
        std::vector<VulkanTexture2D*> m_BackBuffers;
        
    private:
        static VKAPI_ATTR VkBool32 VKAPI_CALL VulkanDebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
            VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData);
    };
    
    
    inline VkFormat VulkanGraphicsDevice::GetBackBufferFormat() const
    {
        return m_SwapChainFormat;
    }
    
    
    inline QueueFamilyIndices VulkanGraphicsDevice::GetQueueFamilyIndices() const
    {
        return m_FamiliyIndices;
    }
}