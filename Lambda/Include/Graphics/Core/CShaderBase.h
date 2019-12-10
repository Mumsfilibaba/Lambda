#pragma once
#include "CDeviceObjectBase.h"
#include "IShader.h"

namespace Lambda
{
	//-----------
	//CShaderBase
	//-----------

	template<typename TDeviceImpl>
	class CShaderBase : public CDeviceObjectBase<TDeviceImpl, IShader>
	{
	public:
        CShaderBase(TDeviceImpl* pDevice, const SShaderDesc& desc)
            : CDeviceObjectBase<TDeviceImpl, IShader>(pDevice),
            m_Desc(desc)
        {
        }
        ~CShaderBase() = default;

        LAMBDA_NO_COPY(CShaderBase);

		virtual void* GetNativeHandle() const override = 0;
		virtual const SShaderDesc& GetDesc() const override
		{
			return m_Desc;
		}
	protected:
		SShaderDesc m_Desc;
	};
}
