#pragma once
#include "LambdaCore.h"

namespace Lambda
{
	//---------
	//Singleton
	//---------

	template <typename T>
	class Singleton
	{
	public:
		inline Singleton()
		{
			LAMBDA_ASSERT_PRINT(s_pInstance == nullptr, "Instance already created");
			s_pInstance = static_cast<T*>(this);
		}

		inline ~Singleton()
		{
			s_pInstance = nullptr;
		}

		LAMBDA_NO_COPY(Singleton);
	public:
		inline static T* GetPtr()
		{
			LAMBDA_ASSERT_PRINT(s_pInstance != nullptr, "Instance not created");
			return s_pInstance;
		}

		inline static T& Get()
		{
			LAMBDA_ASSERT_PRINT(s_pInstance != nullptr, "Instance not created");
			return *s_pInstance;
		}
	private:
		inline static T* s_pInstance = nullptr;
	};
}
