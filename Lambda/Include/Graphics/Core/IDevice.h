#pragma once
#include "IObject.h"
#include "Events/WindowEvent.h"
#include "System/IWindow.h"
#include "IDeviceContext.h"

namespace Lambda
{
	class IQuery;
	class IWindow;
	class IBuffer;
	class IShader;
	class ITexture;
	class IRenderPass;
	class ISamplerState;
	class IPipelineState;

	struct QueryDesc;
	struct BufferDesc;
	struct ShaderDesc;
	struct TextureDesc;
	struct RenderPassDesc;
	struct SamplerStateDesc;
	struct PipelineStateDesc;

	//--------
	//VendorID
	//--------

	enum VendorID : uint32
	{
		VENDOR_ID_UNKNOWN	= 0,
		VENDOR_ID_NVIDIA	= 0x10DE,
		VENDOR_ID_AMD		= 0x1002,
		VENDOR_ID_INTEL		= 0x8086,
		VENDOR_ID_ARM		= 0x13B5,
		VENDOR_ID_QUALCOMM  = 0x5143,
		VENDOR_ID_IMGTEC	= 0x1010,
	};

    //----------
    //DeviceDesc
    //----------
    
    struct DeviceDesc
    {
        IWindow* pWindow       = nullptr;
        GraphicsApi Api        = GRAPHICS_API_VULKAN;
        uint32 Flags           = DEVICE_FLAG_NONE;
		uint32 SampleCount     = 1;
        uint32 BackBufferCount = 3;
        bool VerticalSync      = true;
    };

	//----------------
	//DeviceProperties
	//----------------

	struct DeviceProperties
	{
		char VendorString[256]	= { 0 };
		char AdapterString[256]	= { 0 };
	};

    //-------
    //IDevice
    //-------
    
	class LAMBDA_API IDevice : public IObject
	{
        friend class Application;
	public:
		LAMBDA_IOBJECT_INTERFACE(IDevice);

		virtual void CreateCommandList(IDeviceContext** ppList, CommandListType type) = 0;
		virtual void CreateBuffer(IBuffer** ppBuffer, const ResourceData* pInitalData, const BufferDesc& desc) = 0;
		virtual void CreateTexture(ITexture** ppTexture, const ResourceData* pInitalData, const TextureDesc& desc) = 0;
		virtual void CreateShader(IShader** ppShader, const ShaderDesc& desc) = 0;
		virtual void CreateSamplerState(ISamplerState** ppSamplerState, const SamplerStateDesc& desc) = 0;
		virtual void CreatePipelineState(IPipelineState** ppPipelineState, const PipelineStateDesc& desc) = 0;
		virtual void CreateRenderPass(IRenderPass** ppRenderPass, const RenderPassDesc& desc) = 0;
        virtual void CreateQuery(IQuery** ppQuery, const QueryDesc& desc) = 0;

		virtual void ExecuteCommandList(IDeviceContext* const * ppLists, uint32 numLists) const = 0;
		
		virtual void PresentBegin() const = 0;
		virtual void PresentEnd(IDeviceContext* const* ppLists, uint32 numLists) const = 0;
		
		virtual void WaitForGPU() const = 0;
		virtual void GPUWaitForFrame() const = 0;

		virtual DeviceProperties GetProperties() const = 0;
        virtual const DeviceDesc& GetDesc() const = 0;
        virtual void* GetNativeHandle() const = 0;
		virtual ITexture* GetDepthStencil() const = 0;
		virtual ITexture* GetRenderTarget() const = 0;
		virtual Format GetBackBufferFormat() const = 0;
		virtual uint32 GetBackBufferIndex() const = 0;
		virtual uint32 GetSwapChainWidth() const = 0;
		virtual uint32 GetSwapChainHeight() const = 0;
	private:
		virtual bool OnResize(const WindowResizeEvent& event) = 0;
	public:
		static IDevice* Get();
	protected:
		static IDevice* s_pInstance;
	};
}
