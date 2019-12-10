#include "LambdaPch.h"
#include "Graphics/Core/IShader.h"
#include "Graphics/Core/IDevice.h"
#include <iostream>

namespace Lambda
{
	//-------
	//IShader
	//-------

	IShader* IShader::CreateShaderFromFile(IDevice* pDevice, const char* pFilename, const char* pEntryPoint, EShaderStage type, EShaderLang languange)
	{
		LAMBDA_ASSERT(pDevice != nullptr);

		IShader* pShader = nullptr;

		//Setup mode
		std::ios_base::openmode mode = std::ios::in;
		if (languange == SHADER_LANG_SPIRV)
		{
			mode = std::ios::in | std::ios::binary;
		}
        
        //Load and read file
		std::ifstream file(pFilename, mode);
		if (file.is_open())
		{
			LOG_ENGINE_INFO("Loaded shaderfile '%s' successfully\n", pFilename);
            
			//Load file into string
			std::string source;
			std::stringstream buf;
			buf << file.rdbuf();
			source = buf.str();
			file.close();
            
            //Setup desc
			SShaderDesc desc = {};
			desc.pEntryPoint = pEntryPoint;
			desc.Type = type;
			desc.Languange = languange;
            desc.SourceLength = uint32(source.size());
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
			LOG_ENGINE_ERROR("Failed to read shaderfile '%s'\n", pFilename);
		}

		return pShader;
	}
}
