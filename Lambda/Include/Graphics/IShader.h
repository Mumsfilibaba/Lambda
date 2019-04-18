#pragma once
#include <IObject.h>

namespace Lambda
{
	enum ShaderType
	{
		SHADER_TYPE_UNKNOWN = 0,
		SHADER_TYPE_VERTEX = 1,
		SHADER_TYPE_HULL = 2,
		SHADER_TYPE_DOMAIN = 3,
		SHADER_TYPE_GEOMETRY = 4,
		SHADER_TYPE_PIXEL = 5,
		SHADER_TYPE_COMPUTE = 6,
	};

	enum ShaderFlags
	{
		SHADER_FLAG_NONE = 0,
		SHADER_FLAG_COMPILE_DEBUG = 0,
	};

	enum ShaderLang
	{
		SHADER_LANG_UNKNOWN = 0,
		SHADER_LANG_HLSL = 1,
		SHADER_LANG_HLSL_COMPILED = 2,
	};


	struct ShaderDesc
	{
		ShaderType Type = SHADER_TYPE_UNKNOWN;
		ShaderLang Languange = SHADER_LANG_UNKNOWN;
		uint32 Flags = SHADER_FLAG_NONE;
		const char* pEntryPoint = nullptr;
		const char* pSource = nullptr;
	};

	class IGraphicsDevice;

	class LAMBDA_API IShader : public IObject
	{
	public:
		LAMBDA_INTERFACE(IShader);

		IShader() = default;
		~IShader() = default;

		virtual ShaderType GetType() const = 0;

	public:
		static IShader* CreateShaderFromFile(const IGraphicsDevice* pDevice, const char* pFilename, const char* pEntryPoint, ShaderType type, ShaderLang languange = SHADER_LANG_HLSL);
	};
}