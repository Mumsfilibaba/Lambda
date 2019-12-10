#pragma once
#include "IShaderVariableTable.h"

namespace Lambda
{    
	class IShader;
	class IBuffer;
	class ITexture;
	class ISamplerState;

	//-------------
	//EPipelineType
	//-------------

	enum EPipelineType : uint32
	{
		PIPELINE_TYPE_UNKNOWN	= 0,
		PIPELINE_TYPE_GRAPHICS	= 1,
		PIPELINE_TYPE_COMPUTE	= 2
	};

    //-----------------
    //SInputElementDesc
    //-----------------
    
    struct SInputElementDesc
    {
        const char* pName       = nullptr;
        EFormat Format			= FORMAT_UNKNOWN;
        uint32 BindingSlot      = 0;
        uint32 InputSlot        = 0;
        uint32 Stride           = 0;
        uint32 StructureOffset  = 0;
        bool IsInstanced        = false;
    };

	//----------------
	//SInputLayoutDesc
	//----------------

	struct SInputLayoutDesc
	{
		SInputElementDesc* pElements = nullptr;
		uint32 ElementCount			= 0;
	};

	//--------------------
	//SRasterizerStateDesc
	//--------------------

	struct SRasterizerStateDesc
	{
		ECullMode CullMode  			= CULL_MODE_BACK;
		EPolygonMode PolygonMode	    = POLYGON_MODE_FILL;
		bool FrontFaceCounterClockWise	= false;
	};

	//---------------
	//SBlendStateDesc
	//---------------

	struct SBlendStateDesc
	{
		bool EnableBlending = false;
	};

	//----------------------
	//SDepthStencilStateDesc
	//----------------------

	struct SDepthStencilStateDesc
	{
		bool DepthTest = true;
	};

    //--------------------------
    //SGraphicsPipelineStateDesc
    //--------------------------

    struct SGraphicsPipelineStateDesc
    {
        IShader* pVertexShader      = nullptr;
        IShader* pHullShader        = nullptr;
        IShader* pDomainShader      = nullptr;
        IShader* pGeometryShader    = nullptr;
        IShader* pPixelShader       = nullptr;
        EPrimitiveTopology Topology  = PRIMITIVE_TOPOLOGY_TRIANGLELIST;
        uint32 SampleCount          = 1;
		uint32 NumRenderTargets		= 0;
        EFormat DepthStencilFormat   = FORMAT_D24_UNORM_S8_UINT;
        EFormat RenderTargetFormats[LAMBDA_MAX_RENDERTARGET_COUNT];
        SInputLayoutDesc InputLayout;
        SDepthStencilStateDesc DepthStencilState;
        SBlendStateDesc BlendState;
        SRasterizerStateDesc RasterizerState;
    };

    //-------------------------
    //SComputePipelineStateDesc
    //-------------------------

    struct SComputePipelineStateDesc
    {
        IShader* pComputeShader = nullptr;
    };

    //------------------
    //SPipelineStateDesc
    //------------------
    
	struct SPipelineStateDesc
	{
        const char* pName = nullptr;
		EPipelineType Type = PIPELINE_TYPE_GRAPHICS;
		SShaderVariableTableDesc ShaderVariableTable;
		SGraphicsPipelineStateDesc GraphicsPipeline;
		SComputePipelineStateDesc ComputePipeline;
	};

    //--------------
    //IPipelineState
    //--------------

	class LAMBDA_API IPipelineState : public IDeviceObject
	{
	public:
		LAMBDA_IOBJECT_INTERFACE(IPipelineState);
        
		virtual void CreateShaderVariableTable(IShaderVariableTable** ppVariableTable) = 0;
        virtual void* GetNativeHandle() const = 0;
		virtual const SPipelineStateDesc& GetDesc() const = 0;
	};
}
