#pragma once
#include "CDeviceObjectBase.h"
#include "IQuery.h"

namespace Lambda
{
	//----------
	//CQueryBase
	//----------

	template<typename TDeviceImpl>
	class CQueryBase : public CDeviceObjectBase<TDeviceImpl, IQuery>
	{
	public:
        CQueryBase(TDeviceImpl* pDevice, const SQueryDesc& desc)
            : CDeviceObjectBase<TDeviceImpl, IQuery>(pDevice),
            m_Desc(desc)
        {
        }
        ~CQueryBase() = default;

        LAMBDA_NO_COPY(CQueryBase);

		virtual void GetResults(uint64* pResults, uint32 numResults, uint32 startQuery) override = 0;
		virtual void* GetNativeHandle() const override = 0;


		virtual const SQueryDesc& GetDesc() const override
		{
			return m_Desc;
		}
	protected:
		SQueryDesc m_Desc;
	};
}
