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


    //All keys
	enum Key : uint8
	{
		KEY_UNKNOWN         = 0,
		KEY_SPACE           = 1,
		KEY_APOSTROPHE      = 2,	/* ' */
		KEY_COMMA           = 3,	/* , */
		KEY_MINUS           = 4,	/* - */
		KEY_PERIOD          = 5,	/* . */
		KEY_SLASH           = 6,	/* / */
		KEY_0               = 7,
		KEY_1               = 8,
		KEY_2               = 9,
		KEY_3               = 10,
		KEY_4               = 11,
		KEY_5               = 12,
		KEY_6               = 13,
		KEY_7               = 14,
		KEY_8               = 15,
		KEY_9               = 16,
		KEY_SEMICOLON       = 17,	/* ; */
		KEY_EQUAL           = 18,	/* = */
		KEY_A               = 19,
		KEY_B               = 20,
		KEY_C               = 21,
		KEY_D               = 22,
		KEY_E               = 23,
		KEY_F               = 24,
		KEY_G               = 25,
		KEY_H               = 26,
		KEY_I               = 27,
		KEY_J               = 28,
		KEY_K               = 29,
		KEY_L               = 30,
		KEY_M               = 31,
		KEY_N               = 32,
		KEY_O               = 33,
		KEY_P               = 34,
		KEY_Q               = 35,
		KEY_R               = 36,
		KEY_S               = 37,
		KEY_T               = 38,
		KEY_U               = 39,
		KEY_V               = 40,
		KEY_W               = 41,
		KEY_X               = 42,
		KEY_Y               = 43,
		KEY_Z               = 44,
		KEY_LEFT_BRACKET    = 45,	/* [ */
		KEY_BACKSLASH       = 46,	/* \ */
		KEY_RIGHT_BRACKET   = 47,	/* ] */
		KEY_GRAVE_ACCENT    = 48,	/* ` */
		KEY_WORLD_1         = 49,	/* Non-US #1 */
		KEY_WORLD_2         = 50,	/* Non-US #2 */
		KEY_ESCAPE          = 51,
		KEY_ENTER           = 52,
		KEY_TAB             = 53,
		KEY_BACKSPACE       = 54,
		KEY_INSERT          = 55,
		KEY_DELETE          = 56,
		KEY_RIGHT           = 57,
		KEY_LEFT            = 58,
		KEY_DOWN            = 59,
		KEY_UP              = 60,
		KEY_PAGE_UP         = 70,
		KEY_PAGE_DOWN       = 71,
		KEY_HOME            = 72,
		KEY_END             = 73,
		KEY_CAPS_LOCK       = 74,
		KEY_SCROLL_LOCK     = 75,
		KEY_NUM_LOCK        = 76,
		KEY_PRINT_SCREEN    = 77,
		KEY_PAUSE           = 78,
		KEY_F1              = 79,
		KEY_F2              = 80,
		KEY_F3              = 81,
		KEY_F4              = 82,
		KEY_F5              = 83,
		KEY_F6              = 84,
		KEY_F7              = 85,
		KEY_F8              = 86,
		KEY_F9              = 87,
		KEY_F10             = 88,
		KEY_F11             = 89,
		KEY_F12             = 90,
		KEY_F13             = 91,
		KEY_F14             = 92,
		KEY_F15             = 93,
		KEY_F16             = 94,
		KEY_F17             = 95,
		KEY_F18             = 96,
		KEY_F19             = 97,
		KEY_F20             = 98,
		KEY_F21             = 99,
		KEY_F22             = 100,
		KEY_F23             = 101,
		KEY_F24             = 102,
		KEY_F25             = 103,
		KEY_KEYPAD_0        = 104,
		KEY_KEYPAD_1        = 105,
		KEY_KEYPAD_2        = 106,
		KEY_KEYPAD_3        = 107,
		KEY_KEYPAD_4        = 108,
		KEY_KEYPAD_5        = 109,
		KEY_KEYPAD_6        = 110,
		KEY_KEYPAD_7        = 111,
		KEY_KEYPAD_8        = 112,
		KEY_KEYPAD_9        = 113,
		KEY_KEYPAD_DECIMAL  = 114,
		KEY_KEYPAD_DIVIDE   = 115,
		KEY_KEYPAD_MULTIPLY = 116,
		KEY_KEYPAD_SUBTRACT = 117,
		KEY_KEYPAD_ADD      = 118,
		KEY_KEYPAD_ENTER    = 119,
		KEY_KEYPAD_EQUAL    = 120,
		KEY_LEFT_SHIFT      = 121,
		KEY_LEFT_CONTROL    = 122,
		KEY_LEFT_ALT        = 123,
		KEY_LEFT_SUPER      = 124,
		KEY_RIGHT_SHIFT     = 125,
		KEY_RIGHT_CONTROL   = 126,
		KEY_RIGHT_ALT       = 127,
		KEY_RIGHT_SUPER     = 128,
		KEY_MENU            = 129,
        KEY_LAST            = KEY_MENU
	};


    //Key modifiers
    enum KeyModifier : uint32
    {
        KEY_MODIFIER_UNKNOWN    = 0,
        KEY_MODIFIER_SHIFT      = (1 << 0),
        KEY_MODIFIER_ALT        = (1 << 1),
        KEY_MODIFIER_CONTROL    = (1 << 2),
        KEY_MODIFIER_CAPS_LOCK  = (1 << 3),
        KEY_MODIFIER_SUPER      = (1 << 4),
        KEY_MODIFIER_NUM_LOCK   = (1 << 5),
    };


    //Mousebuttons
	enum MouseButton : uint8
	{
		MOUSEBUTTON_UNKNOWN     = 0,
		MOUSEBUTTON_LEFT        = 1,
		MOUSEBUTTON_MIDDLE      = 2,
		MOUSEBUTTON_RIGHT       = 3,
		MOUSEBUTTON_FORWARD     = 4,
		MOUSEBUTTON_BACKWARD    = 5,
	};


    //Topology to use
	enum PrimitiveTopology : uint32
	{
		PRIMITIVE_TOPOLOGY_UNKNOWN          = 0,
		PRIMITIVE_TOPOLOGY_TRIANGLELIST     = 1,
		PRIMITIVE_TOPOLOGY_TRIANGLESTRIP    = 2,
		PRIMITIVE_TOPOLOGY_POINTLIST		= 3,
	};


	//How polygons will be drawn
	enum PolygonMode : uint32
	{
		POLYGON_MODE_UNKNOWN = 0,
		POLYGON_MODE_FILL = 1,
		POLYGON_MODE_LINE = 2,
		POLYGON_MODE_POINT = 3,
	};


    //Usage of a resource
	enum Usage : uint32
	{
		USAGE_UNKNOWN = 0,
		USAGE_DEFAULT = 1,
		USAGE_DYNAMIC = 2,
	};


    //Capabilities of a buffer
	enum BufferFlags : uint32
	{
		BUFFER_FLAGS_NONE               = 0,
		BUFFER_FLAGS_VERTEX_BUFFER      = (1 << 0),
		BUFFER_FLAGS_INDEX_BUFFER       = (1 << 2),
		BUFFER_FLAGS_CONSTANT_BUFFER    = (1 << 3),
	};


    //Capabilities of a texture
	enum TextureFlags : uint32
	{
		TEXTURE_FLAGS_NONE              = 0,
		TEXTURE_FLAGS_RENDER_TARGET     = (1 << 0),
		TEXTURE_FLAGS_DEPTH_STENCIL     = (1 << 2),
		TEXTURE_FLAGS_SHADER_RESOURCE   = (1 << 3),
		TEXTURE_FLAGS_TEXTURE_CUBE      = (1 << 4),
        TEXTURE_FLAGS_GENEATE_MIPS      = (1 << 5),
	};


	//How a resource should be mapped to a context
	enum MapFlag : uint32
	{
		MAP_FLAG_UNKNOWN		= 0,
		MAP_FLAG_WRITE			= (1 << 0),
		MAP_FLAG_WRITE_DISCARD	= (1 << 1),
	};

    
    //Stage of pipeline
    enum PipelineStage : uint32
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
	enum TextureType : uint32
	{
		TEXTURE_TYPE_UNKNOWN = 0,
		TEXTURE_TYPE_1D = 1,
		TEXTURE_TYPE_2D = 2,
		TEXTURE_TYPE_3D = 3,
	};


    //Format
	enum Format : uint32
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
	enum ResourceState
	{
		RESOURCE_STATE_UNKNOWN                      = 0,
		RESOURCE_STATE_RENDERTARGET                 = 1,
        RESOURCE_STATE_RENDERTARGET_CLEAR           = 2,
        RESOURCE_STATE_DEPTH_STENCIL_CLEAR          = 3,
		RESOURCE_STATE_DEPTH_STENCIL                = 4,
		RESOURCE_STATE_DEPTH_READ                   = 5,
        RESOURCE_STATE_PRESENT         = 6,
		RESOURCE_STATE_COPY_DEST                    = 7,
		RESOURCE_STATE_COPY_SRC                     = 8,
		RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER   = 9,
		RESOURCE_STATE_PIXEL_SHADER_RESOURCE        = 10,
		RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE    = 11,
		RESOURCE_STATE_GENERAL						= 12,
	};
    
    
    //Adress mode for samplers
    enum SamplerAddressMode : uint32
    {
        SAMPLER_ADDRESS_MODE_UNKNOWN                = 0,
        SAMPLER_ADDRESS_MODE_REPEAT                 = 1,
        SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT        = 2,
        SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE          = 3,
        SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER        = 4,
        SAMPLER_ADDRESS_MODE_MIRROR_CLAMP_TO_EDGE   = 5
    };
    
    
    //The cull mode
    enum CullMode : uint32
    {
        CULL_MODE_UNKNOWN   = 0,
        CULL_MODE_BACK      = 1,
        CULL_MODE_FRONT     = 2,
        CULL_MODE_NONE      = 3
    };

    
    //Flags for graphics context
    enum DeviceFlags : uint32
    {
        DEVICE_FLAG_NONE                      = 0,
        DEVICE_FLAG_DEBUG                     = (1 << 0),
        DEVICE_FLAG_ALLOW_SOFTWARE_ADAPTER    = (1 << 1),
    };
    
    
    //Graphics-APIs defines
    enum GraphicsAPI : uint32
    {
        GRAPHICS_API_UNKNOWN    = 0,
        GRAPHICS_API_D3D12      = 1,
        GRAPHICS_API_VULKAN     = 2
    };
    
    
    //Enum describing shaderstages
    enum ShaderStage
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
    enum ResourceType
    {
        RESOURCE_TYPE_UNKNOWN			= 0,
        RESOURCE_TYPE_TEXTURE			= 1,
        RESOURCE_TYPE_SAMPLER_STATE		= 2,
        RESOURCE_TYPE_CONSTANT_BUFFER	= 3
    };


	//Enum describing a commandlisttype
	enum DeviceContextType : uint32
	{
		DEVICE_CONTEXT_TYPE_UNKNOWN	    = 0,
		DEVICE_CONTEXT_TYPE_GRAPHICS    = 1,
		DEVICE_CONTEXT_TYPE_COMPUTE	    = 2,
		DEVICE_CONTEXT_TYPE_COPY		= 3,
        DEVICE_CONTEXT_TYPE_IMMEDIATE   = 4,
        DEVICE_CONTEXT_TYPE_DEFFERED    = 5,
	};
    

    //Struct for updating data in a buffer or texture
	struct ResourceData
	{
		const void* pData = nullptr;
		uint64 SizeInBytes = 0;
	};


	//Struct for Transition a texture
	class ITexture;
	struct TextureTransitionBarrier
	{
		ITexture*		pTexture	= nullptr;
		ResourceState	AfterState	= RESOURCE_STATE_UNKNOWN;
		uint32			MipLevel	= 0;
	};
    
    
    //Viewport struct
    struct Viewport
    {
        float TopX;
        float TopY;
        float Width;
        float Height;
        float MinDepth;
        float MaxDepth;
    };
    
    
    //Rectangle struct
    struct Rectangle
    {
        float X;
        float Y;
        float Width;
        float Height;
    };
}
