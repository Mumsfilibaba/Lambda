#pragma once
#include "DeviceObjectBase.h"
#include "IQuery.h"

namespace Lambda
{
	//---------
	//QueryBase
	//---------

	template<typename TDeviceImpl>
	class QueryBase : public DeviceObjectBase<TDeviceImpl, IQuery>
	{
	public:
		LAMBDA_NO_COPY(QueryBase);

		QueryBase(TDeviceImpl* pDevice, const QueryDesc& desc)
			: DeviceObjectBase<TDeviceImpl, IQuery>(pDevice),
			m_Desc(desc)
		{
		}
		~QueryBase() = default;

		virtual void GetResults(uint64* pResults, uint32 numResults, uint32 startQuery) override = 0;
		virtual void* GetNativeHandle() const override = 0;


		virtual const QueryDesc& GetDesc() const override
		{
			return m_Desc;
		}
	protected:
		QueryDesc m_Desc;
	};
}