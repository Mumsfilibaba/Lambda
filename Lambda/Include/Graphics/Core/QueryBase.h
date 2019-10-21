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

		QueryBase(TDeviceImpl* pDevice)
			: DeviceObjectBase<TDeviceImpl, IQuery>(pDevice),
			m_Desc()
		{
		}
		~QueryBase() = default;


		virtual const QueryDesc& GetDesc() const override
		{
			return m_Desc;
		}
	protected:
		QueryDesc m_Desc;
	};
}