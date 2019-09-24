#pragma once
#include "Defines.h"
#include "Types.h"

namespace Lambda
{
	class IBuffer;
	class ITexture;
	class ISamplerState;

    //------------
    //ResourceSlot
    //------------

    struct ResourceSlot
    {
		ISamplerState*	pSamplerState	= nullptr;
        ResourceType	Type			= RESOURCE_TYPE_UNKNOWN;
        ShaderStage		Stage			= SHADER_STAGE_UNKNOWN;
		ResourceUsage	Usage			= RESOURCE_USAGE_UNKNOWN;
        uint32			Slot			= 0;
    };

	//------------
	//ConstantSlot
	//------------

	struct ConstantSlot
	{
		ShaderStage	Stage		= SHADER_STAGE_UNKNOWN;
		uint32		SizeInBytes	= 0;
	};
    
    //-------------------------
    //PipelineResourceStateDesc
    //-------------------------
    
    struct PipelineResourceStateDesc
    {
        uint32				NumResourceSlots	= 0;
        const ResourceSlot* pResourceSlots		= nullptr;
		uint32				NumConstants		= 0;
		const ConstantSlot* pConstantSlots		= nullptr;
    };
    
    //----------------------
    //IPipelineResourceState
    //----------------------
    
    class LAMBDA_API IPipelineResourceState
    {
    public:
        LAMBDA_INTERFACE(IPipelineResourceState);
        
        IPipelineResourceState() = default;
        ~IPipelineResourceState() = default;
        
		virtual void SetTextures(ITexture** ppTextures, uint32 numTextures, uint32 startSlot) = 0;
		virtual void SetSamplerStates(ISamplerState** ppSamplerStates, uint32 numSamplerStates, uint32 startSlot) = 0;
		virtual void SetConstantBuffers(IBuffer** ppBuffers, uint32 numBuffers, uint32 startSlot) = 0;

        virtual void* GetNativeHandle() const = 0;
    };
}
