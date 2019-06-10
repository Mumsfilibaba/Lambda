#include "LambdaPch.h"
#include "Graphics/IShader.h"
#include "Graphics/IGraphicsDevice.h"

namespace Lambda
{
	IShader* IShader::CreateShaderFromFile(const IGraphicsDevice* pDevice, const char* pFilename, const char* pEntryPoint, ShaderType type, ShaderLang languange)
	{
		IShader* pShader = nullptr;

		std::ifstream file(pFilename, std::ios::in);
		if (file.is_open())
		{
			//Load file into string
			std::string source;
			std::stringstream buf;
			buf << file.rdbuf();
			source = buf.str();
			file.close();

			//Setup desc
			ShaderDesc desc = {};
			desc.pEntryPoint = pEntryPoint;
			desc.Type = type;
			desc.Languange = languange;
			desc.pSource = source.c_str();
#if defined(LAMBDA_DEBUG)
			desc.Flags = SHADER_FLAG_COMPILE_DEBUG;
#else
			desc.Flags = SHADER_FLAG_NONE;
#endif

			//Create
			pDevice->CreateShader(&pShader, desc);
		}
		else
		{
			LOG_DEBUG_ERROR("DX12: Failed to read shaderfile '%s'\n", pFilename);
		}

		return pShader;
	}
}