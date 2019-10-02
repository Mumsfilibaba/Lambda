#pragma once
#include "IDeviceObject.h"

namespace Lambda
{    
	class IShader;
	class IBuffer;
	class ITexture;
	class IRenderPass;
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

	//------------------
	//ShaderVariableDesc
	//------------------

	struct ShaderVariableDesc
	{
		ISamplerState* pSamplerState = nullptr;
		ResourceType Type	= RESOURCE_TYPE_UNKNOWN;
		ShaderStage	Stage	= SHADER_STAGE_UNKNOWN;
		ResourceUsage Usage = RESOURCE_USAGE_UNKNOWN;
		uint32 Slot			= 0;
	};

	//------------
	//ConstantSlot
	//------------

	struct ConstantBlock
	{
		ShaderStage	Stage	= SHADER_STAGE_UNKNOWN;
		uint32 SizeInBytes	= 0;
	};

	//------------------------
	//ShaderVariableLayoutDesc
	//------------------------

	struct ShaderVariableLayoutDesc
	{
		uint32 NumResourceSlots						= 0;
		const ShaderVariableDesc* pResourceSlots	= nullptr;
		uint32 NumConstantBlocks					= 0;
		const ConstantBlock* pConstantBlocks		= nullptr;
	};

	//-------------------------
	//GraphicsPipelineStateDesc
	//-------------------------

	struct GraphicsPipelineStateDesc
	{
		IShader* pVertexShader		= nullptr;
		IShader* pHullShader		= nullptr;
		IShader* pDomainShader		= nullptr;
		IShader* pGeometryShader	= nullptr;
		IShader* pPixelShader		= nullptr;
		IRenderPass* pRenderPass	= nullptr;
		PrimitiveTopology Topology	= PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		InputLayoutDesc	VertexInput;
		RasterizerStateDesc	RasterizerState;
		BlendStateDesc BlendState;
		DepthStencilStateDesc DepthStencilState;
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
		ShaderVariableLayoutDesc ShaderResources;
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
        
		virtual void SetTextures(ITexture** ppTextures, uint32 numTextures, uint32 startSlot) = 0;
		virtual void SetSamplerStates(ISamplerState** ppSamplerStates, uint32 numSamplerStates, uint32 startSlot) = 0;
		virtual void SetConstantBuffers(IBuffer** ppBuffers, uint32 numBuffers, uint32 startSlot) = 0;

        virtual void SetName(const char* pName) = 0;
        virtual void* GetNativeHandle() const = 0;
	};
}
