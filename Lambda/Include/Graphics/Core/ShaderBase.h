#pragma once
#include "DeviceObjectBase.h"
#include "IShader.h"

namespace Lambda
{
	//----------
	//ShaderBase
	//----------

	template<typename TDeviceImpl>
	class ShaderBase : public DeviceObjectBase<TDeviceImpl, IShader>
	{
	public:
		LAMBDA_NO_COPY(ShaderBase);

		ShaderBase(TDeviceImpl* pDevice, const ShaderDesc& desc)
			: DeviceObjectBase<TDeviceImpl, IShader>(pDevice),
			m_Desc(desc)
		{
		}
		~ShaderBase() = default;

		virtual void* GetNativeHandle() const override = 0;


		virtual const ShaderDesc& GetDesc() const override
		{
			return m_Desc;
		}
	protected:
		ShaderDesc m_Desc;
	};
}