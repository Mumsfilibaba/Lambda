#pragma once
#include "Defines.h"
#include "Types.h"

namespace Lambda
{
	class IBuffer;
	class ISamplerState;
	class ITexture2D;


    struct ResourceSlot
    {
        ResourceType Type = RESOURCE_TYPE_UNKNOWN;
        ShaderStage Stage = SHADER_STAGE_UNKNOWN;
        uint32 Slot = 0;
    };
    
    
    struct ResourceStateDesc
    {
        uint32 NumResourceSlots = 0;
        const ResourceSlot* pResourceSlots = nullptr;
    };
    
    
    class LAMBDA_API IResourceState
    {
    public:
        LAMBDA_INTERFACE(IResourceState);
        
        IResourceState() = default;
        ~IResourceState() = default;
        
		virtual void SetTextures(ITexture2D** ppTextures, uint32 numTextures, uint32 startSlot) = 0;
		virtual void SetSamplerStates(ISamplerState** ppSamplerStates, uint32 numSamplerStates, uint32 startSlot) = 0;
		virtual void SetConstantBuffers(IBuffer** ppBuffers, uint32 numBuffers, uint32 startSlot) = 0;

        virtual void* GetNativeHandle() const = 0;
    };
}
