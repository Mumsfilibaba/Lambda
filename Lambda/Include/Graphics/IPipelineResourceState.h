#pragma once
#include "Defines.h"
#include "Types.h"

namespace Lambda
{
	class IBuffer;
	class ISamplerState;
	class ITexture;


    struct ResourceSlot
    {
        ResourceType Type = RESOURCE_TYPE_UNKNOWN;
        ShaderStage Stage = SHADER_STAGE_UNKNOWN;
        uint32 Slot = 0;
    };
    
    
    struct PipelineResourceStateDesc
    {
        uint32 NumResourceSlots = 0;
        const ResourceSlot* pResourceSlots = nullptr;
    };
    
    
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