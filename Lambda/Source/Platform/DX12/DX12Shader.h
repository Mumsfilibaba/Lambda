#pragma once
#include "Graphics/Core/IShader.h"
#if defined(LAMBDA_PLAT_WINDOWS)
	#include "Graphics/Core/DeviceObjectBase.h"

namespace Lambda
{
	class DX12Device;

	//----------
	//DX12Shader
	//----------

	class DX12Shader final : public DeviceObjectBase<DX12Device, IShader>
	{
	public:
		LAMBDA_NO_COPY(DX12Shader);

		DX12Shader(DX12Device* pDevice, const ShaderDesc& desc);
		~DX12Shader() = default;

		virtual void* GetNativeHandle() const override final;
		virtual const ShaderDesc& GetDesc() const override final;

		inline const void* GetShaderBlobData() const { return reinterpret_cast<const void*>(m_ShaderBlob.data()); }
		inline uint64 GetShaderBlobSize() const { return uint64(m_ShaderBlob.size()); }
	private:
		void Init(const ShaderDesc& desc);
	private:
		std::string			m_EntryPoint;
		std::vector<uint8>	m_ShaderBlob;
		ShaderStage			m_Type;
	private:
		static const char* GetTarget(ShaderStage type);
	};
}
#endif
