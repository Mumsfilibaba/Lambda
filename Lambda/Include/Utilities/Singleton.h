#pragma once
#include "LambdaCore.h"
#include <cassert>

namespace Lambda
{
	//---------
	//Singleton
	//---------

	template <typename T>
	class LAMBDA_API Singleton
	{
	protected:
		LAMBDA_NO_COPY(Singleton);

		_forceinline Singleton()
		{
			assert(s_pInstance == nullptr);
			s_pInstance = static_cast<T*>(this);
		}


		_forceinline ~Singleton()
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