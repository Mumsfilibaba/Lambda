#pragma once
#include "Defines.h"
#include "Types.h"

namespace Lambda
{
    enum ResourceType
    {
        RESOURCE_TYPE_UNKNOWN = 0,
        RESOURCE_TYPE_TEXTURE = 1,
        RESOURCE_TYPE_SAMPLER = 2,
        RESOURCE_TYPE_CONSTANT_BUFFER = 3
    };
    
    
    struct ResourceSlot
    {
        ResourceType Type = RESOURCE_TYPE_UNKNOWN;
        ShaderType ShaderStage = SHADER_TYPE_UNKNOWN;
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
