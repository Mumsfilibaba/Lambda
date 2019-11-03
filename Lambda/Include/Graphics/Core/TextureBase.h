#pragma once
#include "DeviceObjectBase.h"
#include "ITexture.h"

namespace Lambda
{
	//-----------
	//TextureBase
	//-----------

	template<typename TDeviceImpl>
	class TextureBase : public DeviceObjectBase<TDeviceImpl, ITexture>
	{
	public:
		LAMBDA_NO_COPY(TextureBase);

		TextureBase(TDeviceImpl* pDevice, const TextureDesc& desc)
			: DeviceObjectBase<TDeviceImpl, ITexture>(pDevice),
			m_Desc(desc)
		{
		}
		~TextureBase() = default;

		virtual void* GetNativeHandle() const override = 0;


		virtual const TextureDesc& GetDesc() const override
		{
			return m_Desc;
		}
	protected:
		TextureDesc m_Desc;
	};
}