#pragma once
#include "Graphics/Core/IShader.h"
#if defined(LAMBDA_PLAT_WINDOWS)
	#include <wrl/client.h>
	#include <d3d12.h>

namespace Lambda
{
	//----------
	//DX12Shader
	//----------

	class DX12Shader final : public RefCountedObject<IShader>
	{
	public:
		LAMBDA_NO_COPY(DX12Shader);

		DX12Shader(const ShaderDesc& desc);
		~DX12Shader() = default;

		virtual void* GetNativeHandle() const override final;
		virtual const ShaderDesc& GetDesc() const override final;

		const void* GetShaderBlobData() const;
		uint64 GetShaderBlobSize() const;
	private:
		void Init(const ShaderDesc& desc);
	private:
		std::string			m_EntryPoint;
		std::vector<uint8>	m_ShaderBlob;
		ShaderStage			m_Type;
	private:
		static const char* GetTarget(ShaderStage type);
	};


	inline const void* DX12Shader::GetShaderBlobData() const
	{
		return reinterpret_cast<const void*>(m_ShaderBlob.data());
	}


	inline uint64 DX12Shader::GetShaderBlobSize() const
	{
		return (uint64)m_ShaderBlob.size();
	}
}
#endif
