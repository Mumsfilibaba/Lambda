#pragma once
#include "DeviceObjectBase.h"
#include "IShaderVariableTable.h"

namespace Lambda
{
	//------------------
	//ShaderVariableBase
	//------------------

	template<typename TDeviceImpl>
	class ShaderVariableBase : public DeviceObjectBase<TDeviceImpl, IShaderVariable>
	{
	public:
		LAMBDA_NO_COPY(ShaderVariableBase);

		ShaderVariableBase(TDeviceImpl* pDevice)
			: DeviceObjectBase<TDeviceImpl, IShaderVariable>(pDevice),
			m_Desc()
		{
		}
		~ShaderVariableBase() = default;


		virtual const ShaderVariableDesc& GetDesc() const override
		{
			return m_Desc;
		}
	protected:
		ShaderVariableDesc m_Desc;
	};
}