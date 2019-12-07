#pragma once
#include "LambdaCore.h"
#include <cassert>

namespace Lambda
{
	//---------
	//CSingleton
	//---------

	template <typename T>
	class LAMBDA_API CSingleton
	{
	protected:
		LAMBDA_NO_COPY(CSingleton);

		_forceinline CSingleton()
		{
			assert(s_pInstance == nullptr);
			s_pInstance = static_cast<T*>(this);
		}


		_forceinline ~CSingleton()
		{
			assert(s_pInstance != nullptr);
			s_pInstance = nullptr;
		}
	public:
		_forceinline static T& Get()
		{
			assert(s_pInstance != nullptr);
			return *s_pInstance;
		}
	protected:
		static T* s_pInstance;
	};
}
