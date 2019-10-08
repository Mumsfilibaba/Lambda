#pragma once
#include "IShaderVariableTable.h"

namespace Lambda
{    
	class IShader;
	class IBuffer;
	class ITexture;
	class ISamplerState;

	//------------
	//PipelineType
	//------------

	enum PipelineType : uint32
	{
		PIPELINE_TYPE_UNKNOWN	= 0,
		PIPELINE_TYPE_GRAPHICS	= 1,
		PIPELINE_TYPE_COMPUTE	= 2
	};

    //----------------
    //InputElementDesc
    //----------------
    
    struct InputElementDesc
    {
        const char* pName       = nullptr;
        Format Format			= FORMAT_UNKNOWN;
        uint32 BindingSlot      = 0;
        uint32 InputSlot        = 0;
        uint32 Stride           = 0;
        uint32 StructureOffset  = 0;
        bool IsInstanced        = false;
    };

	//---------------
	//InputLayoutDesc
	//---------------

	struct InputLayoutDesc
	{
		InputElementDesc* pElements = nullptr;
		uint32 ElementCount			= 0;
	};

	//-------------------
	//RasterizerStateDesc
	//-------------------

	struct RasterizerStateDesc
	{
		CullMode Cull					= CULL_MODE_BACK;
		PolygonMode	FillMode			= POLYGON_MODE_FILL;
		bool FrontFaceCounterClockWise	= false;
	};

	//--------------
	//BlendStateDesc
	//--------------

	struct BlendStateDesc
	{
		bool EnableBlending = false;
	};

	//---------------------
	//DepthStencilStateDesc
	//---------------------

	struct DepthStencilStateDesc
	{
		bool DepthTest = true;
	};

    //-------------------------
    //GraphicsPipelineStateDesc
    //-------------------------

    struct GraphicsPipelineStateDesc
    {
        IShader* pVertexShader      = nullptr;
        IShader* pHullShader        = nullptr;
        IShader* pDomainShader      = nullptr;
        IShader* pGeometryShader    = nullptr;
        IShader* pPixelShader       = nullptr;
        PrimitiveTopology Topology  = PRIMITIVE_TOPOLOGY_TRIANGLELIST;
        uint32 SampleCount          = 1;
        Format DepthStencilFormat   = FORMAT_D24_UNORM_S8_UINT;
        Format RenderTargetFormats[LAMBDA_MAX_RENDERTARGET_COUNT];
        InputLayoutDesc VertexInput;
        DepthStencilStateDesc DepthStencilState;
        BlendStateDesc BlendState;
        RasterizerStateDesc RasterizerState;
    };

    //------------------------
    //ComputePipelineStateDesc
    //------------------------

    struct ComputePipelineStateDesc
    {
        IShader* pComputeShader = nullptr;
    };

    //-----------------
    //PipelineStateDesc
    //-----------------
    
	struct PipelineStateDesc
	{
        const char* pName = nullptr;
		PipelineType Type = PIPELINE_TYPE_GRAPHICS;
		ShaderVariableTableDesc ShaderVariableTable;
		GraphicsPipelineStateDesc GraphicsPipeline;
		ComputePipelineStateDesc ComputePipeline;
	};

    //--------------
    //IPipelineState
    //--------------

	class LAMBDA_API IPipelineState : public IDeviceObject
	{
	public:
		LAMBDA_IOBJECT_INTERFACE(IPipelineState);
        
		virtual void CreateShaderVariableTable(IShaderVariableTable** ppVariableTable) = 0;
        virtual void SetName(const char* pName) = 0;
        virtual void* GetNativeHandle() const = 0;
		virtual const PipelineStateDesc& GetDesc() const = 0;
	};
}
