#pragma once
#include "LambdaCore.h"

namespace Lambda
{
	//----------
	//CSingleton
	//----------

	template <typename T>
	class LAMBDA_API CSingleton
	{
	protected:       
		LAMBDA_NO_COPY(CSingleton);

		_forceinline CSingleton()
		{
			s_pInstance = static_cast<T*>(this);
        }

        _forceinline ~CSingleton()
		{
			s_pInstance = nullptr;
		}
	public:
		_forceinline static T& Get()
		{
			return *s_pInstance;
		}
	protected:
		static T* s_pInstance;
	};
}
