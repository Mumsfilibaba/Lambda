#pragma once
#include "IDeviceContext.h"

namespace Lambda
{
	class IQuery;
	class IBuffer;
	class IShader;
	class ITexture;
	class ISamplerState;
	class IPipelineState;

	struct SQueryDesc;
	struct SBufferDesc;
	struct SShaderDesc;
	struct STextureDesc;
	struct SSamplerStateDesc;
	struct SPipelineStateDesc;

	//---------
	//EVendorID
	//---------

	enum EVendorID : uint32
	{
		VENDOR_ID_UNKNOWN	= 0,
		VENDOR_ID_NVIDIA	= 0x10DE,
		VENDOR_ID_AMD		= 0x1002,
		VENDOR_ID_INTEL		= 0x8086,
		VENDOR_ID_ARM		= 0x13B5,
		VENDOR_ID_QUALCOMM  = 0x5143,
		VENDOR_ID_IMGTEC	= 0x1010,
	};

    //-----------
    //SDeviceDesc
    //-----------
    
    struct SDeviceDesc
    {
        uint32 Flags = DEVICE_FLAG_NONE;
    };

	//-----------------
	//SDeviceProperties
	//-----------------

	struct SDeviceProperties
	{
        EGraphicsAPI Api        = GRAPHICS_API_UNKNOWN;
		char VendorString[256]	= { 0 };
		char AdapterString[256]	= { 0 };
	};

	//---------------
	//SDeviceFeatures
	//---------------

	struct SDeviceFeatures
	{
		bool MeshShaders				= false;
		bool HardwareRayTracing			= false;
		bool VariableRateShading		= false;
		bool SwapChainFullscreenState	= false;
	};

    //-------
    //IDevice
    //-------
    
	class LAMBDA_API IDevice : public IObject
	{
	public:
		LAMBDA_IOBJECT_INTERFACE(IDevice);

		virtual void CreateDefferedContext(IDeviceContext** ppList) = 0;
		virtual void CreateBuffer(IBuffer** ppBuffer, const SResourceData* pInitalData, const SBufferDesc& desc) = 0;
		virtual void CreateTexture(ITexture** ppTexture, const SResourceData* pInitalData, const STextureDesc& desc) = 0;
		virtual void CreateShader(IShader** ppShader, const SShaderDesc& desc) = 0;
		virtual void CreateSamplerState(ISamplerState** ppSamplerState, const SSamplerStateDesc& desc) = 0;
		virtual void CreatePipelineState(IPipelineState** ppPipelineState, const SPipelineStateDesc& desc) = 0;
        virtual void CreateQuery(IQuery** ppQuery, const SQueryDesc& desc) = 0;
        
        virtual IDeviceContext* GetImmediateContext() const = 0;
        
        virtual const SDeviceDesc& GetDesc() const = 0;
		virtual const SDeviceFeatures& GetFeatures() const = 0;
        virtual const SDeviceProperties& GetProperties() const = 0;
        virtual void* GetNativeHandle() const = 0;
	};
}
