#include "LambdaPch.h"
#if defined(LAMBDA_PLAT_WINDOWS)
	#include "DX12Shader.h"

namespace Lambda
{
	DX12Shader::DX12Shader(const ShaderDesc& desc)
		: m_ShaderBlob(),
		m_Type(SHADER_STAGE_UNKNOWN)
	{
		Init(desc);
	}
    

	ShaderStage DX12Shader::GetType() const
	{
		return m_Type;
	}

	void* DX12Shader::GetNativeHandle() const
	{
		return nullptr;
	}


	const char* DX12Shader::GetEntryPoint() const
	{
		return m_EntryPoint.c_str();
	}


	void DX12Shader::Init(const ShaderDesc& desc)
	{
		using namespace Microsoft::WRL;

		//Copy compiled source into blob
		if (desc.Languange == SHADER_LANG_HLSL_COMPILED)
		{
			uint64 len = strlen(desc.pSource);

			m_ShaderBlob.resize(len);
			memcpy(m_ShaderBlob.data(), desc.pSource, len);

			LOG_DEBUG_INFO("DX12: Loaded shader from compiled source.\n");
		}
		else if (desc.Languange == SHADER_LANG_HLSL)
		{
			//Setup compile flags
			uint32 flags = D3DCOMPILE_WARNINGS_ARE_ERRORS;
			if (desc.Flags & SHADER_FLAG_COMPILE_DEBUG)
				flags |= D3DCOMPILE_SKIP_OPTIMIZATION | D3DCOMPILE_DEBUG;
			else
				flags |= D3DCOMPILE_OPTIMIZATION_LEVEL3;

			//Compile shader
			ComPtr<ID3DBlob> error = nullptr;
			ComPtr<ID3DBlob> shaderBlob = nullptr;
			HRESULT hr = D3DCompile2(desc.pSource, strlen(desc.pSource), nullptr, nullptr, nullptr, desc.pEntryPoint, GetTarget(desc.Type), flags, 0, 0, 0, 0, &shaderBlob, &error);
			if (FAILED(hr))
			{
	#if defined(LAMBDA_DEBUG)
				const char* pMessage = reinterpret_cast<const char*>(error->GetBufferPointer());
	#endif
				LOG_DEBUG_ERROR("DX12: Failed to compile shader. Error-message:\n%s", pMessage);
			}
			else
			{
				LOG_DEBUG_INFO("DX12: Compiled shader.\n");

				m_ShaderBlob.resize(shaderBlob->GetBufferSize());
				memcpy(m_ShaderBlob.data(), shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize());
			}
		}

		//Init
		m_Type = desc.Type;
	}


	const char* DX12Shader::GetTarget(ShaderStage type)
	{
		switch (type)
		{
		case SHADER_STAGE_VERTEX: return "vs_5_0";
		case SHADER_STAGE_HULL: return "hs_5_0";
		case SHADER_STAGE_DOMAIN: return "ds_5_0";
		case SHADER_STAGE_GEOMETRY: return "gs_5_0";
		case SHADER_STAGE_PIXEL: return "ps_5_0";
		case SHADER_STAGE_COMPUTE: return "cs_5_0";
		case SHADER_STAGE_UNKNOWN:
		default: return nullptr;
		}
	}
}
#endif
