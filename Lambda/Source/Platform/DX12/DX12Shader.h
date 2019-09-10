#pragma once
#include "Graphics/IShader.h"
#if defined(LAMBDA_PLAT_WINDOWS)
	#include <wrl/client.h>
	#include <d3d12.h>

namespace Lambda
{
	class DX12Shader final : public IShader
	{
		friend class DX12GraphicsDevice;
		friend class DX12GraphicsPipelineState;

	public:
		LAMBDA_NO_COPY(DX12Shader);

		DX12Shader(const ShaderDesc& desc);
		~DX12Shader() = default;

		virtual void* GetNativeHandle() const override final;
		virtual ShaderDesc GetDesc() const override final;

	private:
		void Init(const ShaderDesc& desc);

		const void* GetShaderBlobData() const;
		uint64 GetShaderBlobSize() const;

	private:
		std::vector<uint8> m_ShaderBlob;
		std::string m_EntryPoint;
		ShaderStage m_Type;

	private:
		static const char* GetTarget(ShaderStage type);
	};


	inline const void* DX12Shader::GetShaderBlobData() const
	{
		return (const void*)m_ShaderBlob.data();
	}


	inline uint64 DX12Shader::GetShaderBlobSize() const
	{
		return (uint64)m_ShaderBlob.size();
	}
}
#endif
