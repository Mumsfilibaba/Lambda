#pragma once
#include "Defines.h"
#include "Types.h"

namespace Lambda
{
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
    
    
    class IResourceState
    {
    public:
        LAMBDA_INTERFACE(IResourceState);
        
        IResourceState() = default;
        ~IResourceState() = default;
        
        virtual void* GetNativeHandle() const = 0;
    };
}
