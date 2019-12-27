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
		_forceinline CSingleton()
		{
			LAMBDA_ASSERT(s_pInstance == nullptr);
			s_pInstance = static_cast<T*>(this);
        }

        _forceinline ~CSingleton()
		{
			LAMBDA_ASSERT(s_pInstance != nullptr);
			s_pInstance = nullptr;
		}

		LAMBDA_NO_COPY(CSingleton);
	public:
		_forceinline static T& Get()
		{
			LAMBDA_ASSERT(s_pInstance != nullptr);
			return *s_pInstance;
		}

		_forceinline static T* GetPtr()
		{
			LAMBDA_ASSERT(s_pInstance != nullptr);
			return s_pInstance;
		}
	protected:
		static T* s_pInstance;
	};
}
