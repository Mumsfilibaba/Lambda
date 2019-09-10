#pragma once
#include "IObject.h"

namespace Lambda
{    
	class IShader;
	class IRenderPass;
	class IPipelineResourceState;

    
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
    
    
	struct GraphicsPipelineStateDesc
	{
        const char* pName = nullptr;
        //Shaders
		IShader* pVertexShader = nullptr;
		IShader* pPixelShader = nullptr;
        //Input Layout
        InputElement* pInputElements = nullptr;
        uint32 InputElementCount = 0;
        //State
		PolygonMode Mode = POLYGON_MODE_FILL;
        PrimitiveTopology Topology = PRIMITIVE_TOPOLOGY_TRIANGLELIST;
        CullMode Cull = CULL_MODE_BACK;
        //Depth test
        bool DepthTest = true;
        //RenderPass defines the framebuffer
		IRenderPass* pRenderPass = nullptr;
		//Resourcestate hold info about what resources will be bound to the pipeline
		IPipelineResourceState* pResourceState = nullptr;
	};


	class LAMBDA_API IGraphicsPipelineState
	{
	public:
		LAMBDA_INTERFACE(IGraphicsPipelineState);

		IGraphicsPipelineState() = default;
		~IGraphicsPipelineState() = default;
        
        virtual void SetName(const char* pName) = 0;
        virtual void* GetNativeHandle() const = 0;
	};


	struct ComputePipelineStateDesc
	{
		IShader* pComputeShader = nullptr;
	};


	class LAMBDA_API IComputePipelineState
	{
	public:
		LAMBDA_INTERFACE(IComputePipelineState);

		IComputePipelineState() = default;
		~IComputePipelineState() = default;
        
        virtual void* GetNativeHandle() const = 0; 
	};
}
