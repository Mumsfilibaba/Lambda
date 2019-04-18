#include <LambdaPch.h>
#if defined(LAMBDA_PLAT_WINDOWS)
	#include "DX12Shader.h"

namespace Lambda
{
	DX12Shader::DX12Shader(const ShaderDesc& desc)
		: m_ShaderBlob(),
		m_Type(SHADER_TYPE_UNKNOWN),
		m_References(0)
	{
		AddRef();
		Init(desc);
	}

	DX12Shader::~DX12Shader()
	{
	}
	
	ShaderType DX12Shader::GetType() const
	{
		return m_Type;
	}

	uint32 DX12Shader::Release()
	{
		IOBJECT_IMPLEMENT_RELEASE(m_References);
	}
	
	uint32 DX12Shader::AddRef()
	{
		return ++m_References;
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
			
				DEBUG_BREAK();
			}
			else
			{
				LOG_DEBUG_INFO("DX12: Compiled shader.\n");

				m_ShaderBlob.resize(shaderBlob->GetBufferSize());
				memcpy(m_ShaderBlob.data(), shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize());
			}
		}
	}

	const char* DX12Shader::GetTarget(ShaderType type)
	{
		switch (type)
		{
		case SHADER_TYPE_VERTEX: return "vs_5_0";
		case SHADER_TYPE_HULL: return "hs_5_0";
		case SHADER_TYPE_DOMAIN: return "ds_5_0";
		case SHADER_TYPE_GEOMETRY: return "gs_5_0";
		case SHADER_TYPE_PIXEL: return "ps_5_0";
		case SHADER_TYPE_COMPUTE: return "cs_5_0";
		case SHADER_TYPE_UNKNOWN:
		default: return nullptr;
		}
	}
}
#endif