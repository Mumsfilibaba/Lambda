#pragma once
#include "Defines.h"
#include "Types.h"

namespace Lambda
{   
	class IGraphicsDevice;

    //-----------
    //ShaderFlags
    //-----------
    
	enum ShaderFlags
	{
		SHADER_FLAG_NONE            = 0,
		SHADER_FLAG_COMPILE_DEBUG   = 1,
	};

    //----------
    //ShaderLang
    //----------
    
	enum ShaderLang
	{
		SHADER_LANG_UNKNOWN         = 0,
		SHADER_LANG_HLSL            = 1,
		SHADER_LANG_HLSL_COMPILED   = 2,
        SHADER_LANG_SPIRV           = 3,
	};

    //----------
    //ShaderDesc
    //----------
    
	struct ShaderDesc
	{
		ShaderStage Type        = SHADER_STAGE_UNKNOWN;
		ShaderLang Languange    = SHADER_LANG_UNKNOWN;
		uint32 Flags            = SHADER_FLAG_NONE;
        uint32 SourceLength     = 0;
		const char* pEntryPoint = nullptr;
		const char* pSource     = nullptr;
	};

    //-------
    //IShader
    //-------
    
	class LAMBDA_API IShader
	{
	public:
		LAMBDA_INTERFACE(IShader);

		IShader() = default;
		~IShader() = default;

        virtual void* GetNativeHandle() const = 0;
        virtual ShaderDesc GetDesc() const = 0;
	public:
		static IShader* CreateShaderFromFile(IGraphicsDevice* pDevice, const char* pFilename, const char* pEntryPoint, ShaderStage type, ShaderLang languange = SHADER_LANG_HLSL);
	};
}
