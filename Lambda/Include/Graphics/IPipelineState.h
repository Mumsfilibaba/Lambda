#pragma once
#include "IObject.h"

namespace Lambda
{    
    //Forwarddeclarations
	class IShader;
	class IRenderPass;

    
    //Struct describing an VertexInputElement
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
    
    
    //Struct describing a graphicspipeline
	struct GraphicsPipelineStateDesc
	{
        //Name
        const char* pName = nullptr;
        //Shaders
		IShader* pVertexShader = nullptr;
		IShader* pPixelShader = nullptr;
        //Input Layout
        InputElement* pInputElements = nullptr;
        uint32 InputElementCount = 0;
        //State
        PrimitiveTopology Topology = PRIMITIVE_TOPOLOGY_TRIANGLELIST;
        CullMode Cull = CULL_MODE_BACK;
        //Depth test
        bool DepthTest = true;
        //RenderPass defines the framebuffer
		IRenderPass* pRenderPass = nullptr;
	};


    //GraphicsPipeline
	class LAMBDA_API IGraphicsPipelineState
	{
	public:
		LAMBDA_INTERFACE(IGraphicsPipelineState);

		IGraphicsPipelineState() = default;
		~IGraphicsPipelineState() = default;
        
        virtual void SetName(const char* pName) = 0;
        virtual void* GetNativeHandle() const = 0;
	};


    //Struct describing a computepipeline
	struct ComputePipelineStateDesc
	{
		IShader* pComputeShader = nullptr;
	};

    
    //ComputePipeline
	class LAMBDA_API IComputePipelineState
	{
	public:
		LAMBDA_INTERFACE(IComputePipelineState);

		IComputePipelineState() = default;
		~IComputePipelineState() = default;
        
        virtual void* GetNativeHandle() const = 0; 
	};
}
