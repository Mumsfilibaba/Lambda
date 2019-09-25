#pragma once
#include "IObject.h"

namespace Lambda
{    
	class IShader;
	class IRenderPass;
	class IPipelineResourceState;

    //------------
    //InputElement
    //------------
    
    struct InputElement
    {
        const char* pName       = nullptr;
        ResourceFormat Format   = FORMAT_UNKNOWN;
        uint32 BindingSlot      = 0;
        uint32 InputSlot        = 0;
        uint32 Stride           = 0;
        uint32 StructureOffset  = 0;
        bool IsInstanced        = false;
    };
    
    //-------------------------
    //GraphicsPipelineStateDesc
    //-------------------------
    
	struct GraphicsPipelineStateDesc
	{
        const char* pName		= nullptr;
        //Shaders
		IShader* pVertexShader	= nullptr;
		IShader* pPixelShader	= nullptr;
        //Input Layout
        InputElement*	pInputElements		= nullptr;
        uint32			InputElementCount	= 0;
        //Rasterizer State
        CullMode			Cull						= CULL_MODE_BACK;
		PolygonMode			FillMode						= POLYGON_MODE_FILL;
        PrimitiveTopology	Topology					= PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		bool				FrontFaceCounterClockWise	= false;
		//Blending
		bool EnableBlending = false;
		//Depth test
        bool DepthTest = true;
        //RenderPass defines the framebuffer
		IRenderPass* pRenderPass = nullptr;
		//Resourcestate hold info about what resources will be bound to the pipeline
		IPipelineResourceState* pResourceState = nullptr;
	};

    //----------------------
    //IGraphicsPipelineState
    //----------------------

	class LAMBDA_API IGraphicsPipelineState
	{
	public:
		LAMBDA_INTERFACE(IGraphicsPipelineState);

		IGraphicsPipelineState() = default;
		~IGraphicsPipelineState() = default;
        
        virtual void SetName(const char* pName) = 0;
        virtual void* GetNativeHandle() const = 0;
	};

    //------------------------
    //ComputePipelineStateDesc
    //------------------------
    
	struct ComputePipelineStateDesc
	{
		IShader* pComputeShader = nullptr;
	};

    //---------------------
    //IComputePipelineState
    //---------------------

	class LAMBDA_API IComputePipelineState
	{
	public:
		LAMBDA_INTERFACE(IComputePipelineState);

		IComputePipelineState() = default;
		~IComputePipelineState() = default;
        
        virtual void* GetNativeHandle() const = 0; 
	};
}
