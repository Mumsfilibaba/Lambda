#pragma once
#include "LambdaCore.h"
#include <cassert>

namespace Lambda
{
	//----------
	//CSingleton
	//----------

	template <typename T>
	class LAMBDA_API CSingleton
	{
	protected:
        CSingleton() = default;
        ~CSingleton() = default;
        
		LAMBDA_NO_COPY(CSingleton);

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
	public:
		_forceinline static T& Get()
		{
			LAMBDA_ASSERT(s_pInstance != nullptr);
			return *s_pInstance;
		}
	protected:
		static T* s_pInstance;
	};
}
