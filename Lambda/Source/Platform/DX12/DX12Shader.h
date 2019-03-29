#pragma once
#include <Graphics/IShader.h>
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
		~DX12Shader();

		virtual ShaderType GetType() const override final;

		virtual uint32 Release() override final;
		virtual uint32 AddRef() override final;

	private:
		void Init(const ShaderDesc& desc);
		ID3DBlob* GetShaderBlob() const;

	private:
		Microsoft::WRL::ComPtr<ID3DBlob> m_ShaderBlob;
		ShaderType m_Type;
		uint32 m_References;

	private:
		static const char* GetTarget(ShaderType type);
	};

	inline ID3DBlob* DX12Shader::GetShaderBlob() const
	{
		return m_ShaderBlob.Get();
	}
}
#endif