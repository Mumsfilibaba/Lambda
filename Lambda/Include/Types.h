#pragma once

namespace Lambda
{
    //Integer types
	typedef char				int8;
	typedef short				int16;
	typedef int					int32;
	typedef long long			int64;
	typedef unsigned char		uint8;
	typedef unsigned short		uint16;
	typedef unsigned int		uint32;
	typedef unsigned long long	uint64;

    //Topology to use
	enum EPrimitiveTopology : uint32
	{
		PRIMITIVE_TOPOLOGY_UNKNOWN          = 0,
		PRIMITIVE_TOPOLOGY_TRIANGLELIST     = 1,
		PRIMITIVE_TOPOLOGY_TRIANGLESTRIP    = 2,
		PRIMITIVE_TOPOLOGY_POINTLIST		= 3,
	};


	//How polygons will be drawn
	enum EPolygonMode : uint32
	{
		POLYGON_MODE_UNKNOWN = 0,
		POLYGON_MODE_FILL = 1,
		POLYGON_MODE_LINE = 2,
		POLYGON_MODE_POINT = 3,
	};


    //Usage of a resource
	enum EUsage : uint32
	{
		USAGE_UNKNOWN = 0,
		USAGE_DEFAULT = 1,
		USAGE_DYNAMIC = 2,
	};


    //Capabilities of a buffer
	enum EBufferFlags : uint32
	{
		BUFFER_FLAGS_NONE               = 0,
		BUFFER_FLAGS_VERTEX_BUFFER      = (1 << 0),
		BUFFER_FLAGS_INDEX_BUFFER       = (1 << 2),
		BUFFER_FLAGS_CONSTANT_BUFFER    = (1 << 3),
	};


    //Capabilities of a texture
	enum ETextureFlags : uint32
	{
		TEXTURE_FLAGS_NONE              = 0,
		TEXTURE_FLAGS_RENDER_TARGET     = (1 << 0),
		TEXTURE_FLAGS_DEPTH_STENCIL     = (1 << 2),
		TEXTURE_FLAGS_SHADER_RESOURCE   = (1 << 3),
		TEXTURE_FLAGS_TEXTURE_CUBE      = (1 << 4),
        TEXTURE_FLAGS_GENEATE_MIPS      = (1 << 5),
	};


	//How a resource should be mapped to a context
	enum EMapFlag : uint32
	{
		MAP_FLAG_UNKNOWN		= 0,
		MAP_FLAG_WRITE			= (1 << 0),
		MAP_FLAG_WRITE_DISCARD	= (1 << 1),
	};

    
    //Stage of pipeline
    enum EPipelineStage : uint32
    {
        PIPELINE_STAGE_UNKNOWN   = 0,
        PIPELINE_STAGE_VERTEX    = 1,
        PIPELINE_STAGE_HULL      = 2,
        PIPELINE_STAGE_DOMAIN    = 3,
        PIPELINE_STAGE_GEOMETRY  = 4,
        PIPELINE_STAGE_PIXEL     = 5,
        PIPELINE_STAGE_COMPUTE   = 6,
    };
    

	//Types of texture
	enum ETextureType : uint32
	{
		TEXTURE_TYPE_UNKNOWN = 0,
		TEXTURE_TYPE_1D = 1,
		TEXTURE_TYPE_2D = 2,
		TEXTURE_TYPE_3D = 3,
	};


    //Format
	enum EFormat : uint32
	{
		FORMAT_UNKNOWN                  = 0,
		FORMAT_R32G32B32A32_FLOAT       = 1,
		FORMAT_R32G32B32A32_UINT        = 2,
		FORMAT_R32G32B32A32_SINT        = 3,
		FORMAT_R32G32B32_FLOAT          = 4,
		FORMAT_R32G32B32_UINT           = 5,
		FORMAT_R32G32B32_SINT           = 6,
		FORMAT_R16G16B16A16_FLOAT       = 7,
		FORMAT_R16G16B16A16_UNORM       = 8,
		FORMAT_R16G16B16A16_UINT        = 9,
		FORMAT_R16G16B16A16_SNORM       = 10,
		FORMAT_R16G16B16A16_SINT        = 11,
		FORMAT_R32G32_FLOAT             = 12,
		FORMAT_R32G32_UINT              = 13,
		FORMAT_R32G32_SINT              = 14,
		FORMAT_D32_FLOAT_S8X24_UINT     = 15,
		FORMAT_R8G8B8A8_TYPELESS        = 19,
		FORMAT_R8G8B8A8_UNORM           = 20,
		FORMAT_R8G8B8A8_UNORM_SRGB      = 21,
		FORMAT_R8G8B8A8_UINT            = 22,
		FORMAT_R8G8B8A8_SNORM           = 23,
		FORMAT_R8G8B8A8_SINT            = 24,
		FORMAT_R16G16_FLOAT             = 25,
		FORMAT_R16G16_UNORM             = 26,
		FORMAT_R16G16_UINT              = 27,
		FORMAT_R16G16_SNORM             = 28,
		FORMAT_R16G16_SINT              = 29,
		FORMAT_D32_FLOAT                = 30,
		FORMAT_R32_FLOAT                = 31,
		FORMAT_R32_UINT                 = 32,
		FORMAT_R32_SINT                 = 33,
		FORMAT_D24_UNORM_S8_UINT        = 34,
		FORMAT_R8G8_UNORM               = 35,
		FORMAT_R8G8_UINT                = 36,
		FORMAT_R8G8_SNORM               = 37,
		FORMAT_R8G8_SINT                = 38,
		FORMAT_R16_FLOAT                = 39,
		FORMAT_D16_UNORM                = 40,
		FORMAT_R16_UNORM                = 41,
		FORMAT_R16_UINT                 = 42,
		FORMAT_R16_SNORM                = 43,
		FORMAT_R16_SINT                 = 44,
		FORMAT_R8_UNORM                 = 45,
		FORMAT_R8_UINT                  = 46,
		FORMAT_R8_SNORM                 = 47,
		FORMAT_R8_SINT                  = 48,
		FORMAT_B8G8R8A8_UNORM           = 49,
		FORMAT_B8G8R8A8_UNORM_SRGB      = 50,
	};


    //State of a buffer or a texture
	enum EResourceState
	{
		RESOURCE_STATE_UNKNOWN                      = 0,
		RESOURCE_STATE_RENDERTARGET                 = 1,
        RESOURCE_STATE_RENDERTARGET_CLEAR           = 2,
        RESOURCE_STATE_DEPTH_STENCIL_CLEAR          = 3,
		RESOURCE_STATE_DEPTH_STENCIL                = 4,
		RESOURCE_STATE_DEPTH_READ                   = 5,
        RESOURCE_STATE_PRESENT						= 6,
		RESOURCE_STATE_COPY_DEST                    = 7,
		RESOURCE_STATE_COPY_SRC                     = 8,
		RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER   = 9,
		RESOURCE_STATE_PIXEL_SHADER_RESOURCE        = 10,
		RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE    = 11,
		RESOURCE_STATE_GENERAL						= 12,
	};
    
    
    //Adress mode for samplers
    enum ESamplerAddressMode : uint32
    {
        SAMPLER_ADDRESS_MODE_UNKNOWN                = 0,
        SAMPLER_ADDRESS_MODE_REPEAT                 = 1,
        SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT        = 2,
        SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE          = 3,
        SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER        = 4,
        SAMPLER_ADDRESS_MODE_MIRROR_CLAMP_TO_EDGE   = 5
    };
    
    
    //The cull mode
    enum ECullMode : uint32
    {
        CULL_MODE_UNKNOWN   = 0,
        CULL_MODE_BACK      = 1,
        CULL_MODE_FRONT     = 2,
        CULL_MODE_NONE      = 3
    };

    
    //Flags for graphics context
    enum EDeviceFlags : uint32
    {
        DEVICE_FLAG_NONE                      = 0,
        DEVICE_FLAG_DEBUG                     = (1 << 0),
        DEVICE_FLAG_ALLOW_SOFTWARE_ADAPTER    = (1 << 1),
    };
    
    
    //Graphics-APIs defines
    enum EGraphicsAPI : uint32
    {
        GRAPHICS_API_UNKNOWN    = 0,
        GRAPHICS_API_D3D12      = 1,
        GRAPHICS_API_VULKAN     = 2
    };
    
    
    //Enum describing shaderstages
    enum EShaderStage
    {
        SHADER_STAGE_UNKNOWN  = 0,
        SHADER_STAGE_VERTEX	  = (1 << 0),
        SHADER_STAGE_HULL	  = (1 << 1),
        SHADER_STAGE_DOMAIN	  = (1 << 2),
        SHADER_STAGE_GEOMETRY = (1 << 3),
        SHADER_STAGE_PIXEL	  = (1 << 4),
        SHADER_STAGE_COMPUTE  = (1 << 5),
    };
    
    
    //Type of resource in a resourcestate
    enum EResourceType
    {
        RESOURCE_TYPE_UNKNOWN			= 0,
        RESOURCE_TYPE_TEXTURE			= 1,
        RESOURCE_TYPE_SAMPLER_STATE		= 2,
        RESOURCE_TYPE_CONSTANT_BUFFER	= 3
    };


	//Enum describing a commandlisttype
	enum EDeviceContextType : uint32
	{
		DEVICE_CONTEXT_TYPE_UNKNOWN	    = 0,
		DEVICE_CONTEXT_TYPE_GRAPHICS    = 1,
		DEVICE_CONTEXT_TYPE_COMPUTE	    = 2,
		DEVICE_CONTEXT_TYPE_COPY		= 3,
        DEVICE_CONTEXT_TYPE_IMMEDIATE   = 4,
        DEVICE_CONTEXT_TYPE_DEFFERED    = 5,
	};
    

    //Struct for updating data in a buffer or texture
	struct SResourceData
	{
		const void* pData = nullptr;
		uint64 SizeInBytes = 0;
	};


	//Struct for Transition a texture
	class ITexture;
	struct STextureTransitionBarrier
	{
		ITexture*		pTexture	= nullptr;
		EResourceState	AfterState	= RESOURCE_STATE_UNKNOWN;
		uint32			MipLevel	= 0;
	};
    
    
    //Viewport struct
    struct SViewport
    {
        float TopX;
        float TopY;
        float Width;
        float Height;
        float MinDepth;
        float MaxDepth;
    };
    
    
    //Rectangle struct
    struct SRectangle
    {
        float X;
        float Y;
        float Width;
        float Height;
    };
}
