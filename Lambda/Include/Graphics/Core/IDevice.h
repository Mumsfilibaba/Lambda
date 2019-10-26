#pragma once
#include "Events/WindowEvent.h"
#include "System/IWindow.h"
#include "IDeviceContext.h"

namespace Lambda
{
	class IQuery;
	class IBuffer;
	class IShader;
	class ITexture;
	class ISamplerState;
	class IPipelineState;

	struct QueryDesc;
	struct BufferDesc;
	struct ShaderDesc;
	struct TextureDesc;
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
        uint32 Flags = DEVICE_FLAG_NONE;
    };

	//----------------
	//DeviceProperties
	//----------------

	struct DeviceProperties
	{
        GraphicsAPI Api         = GRAPHICS_API_UNKNOWN;
		char VendorString[256]	= { 0 };
		char AdapterString[256]	= { 0 };
	};

	//------------------
	//DeviceCapabilities
	//------------------

	struct DeviceFeatures
	{
		bool SupportRTX				= false;
		bool SupportFullscreenState = false;
	};

    //-------
    //IDevice
    //-------
    
	class LAMBDA_API IDevice : public IObject
	{
	public:
		LAMBDA_IOBJECT_INTERFACE(IDevice);

		virtual void CreateDefferedContext(IDeviceContext** ppList) = 0;
		virtual void CreateBuffer(IBuffer** ppBuffer, const ResourceData* pInitalData, const BufferDesc& desc) = 0;
		virtual void CreateTexture(ITexture** ppTexture, const ResourceData* pInitalData, const TextureDesc& desc) = 0;
		virtual void CreateShader(IShader** ppShader, const ShaderDesc& desc) = 0;
		virtual void CreateSamplerState(ISamplerState** ppSamplerState, const SamplerStateDesc& desc) = 0;
		virtual void CreatePipelineState(IPipelineState** ppPipelineState, const PipelineStateDesc& desc) = 0;
        virtual void CreateQuery(IQuery** ppQuery, const QueryDesc& desc) = 0;
        
        virtual IDeviceContext* GetImmediateContext() const = 0;
        
        virtual const DeviceDesc& GetDesc() const = 0;
		virtual const DeviceFeatures& GetFeatures() const = 0;
        virtual const DeviceProperties& GetProperties() const = 0;
        virtual void* GetNativeHandle() const = 0;
	};
}
