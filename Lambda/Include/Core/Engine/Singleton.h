#pragma once
#include "LambdaCore.h"

namespace Lambda
{
	//----------
	//CSingleton
	//----------

	template <typename T>
	class CSingleton
	{
	protected:
		LAMBDA_NO_COPY(CSingleton);

		inline CSingleton()
		{
			LAMBDA_ASSERT_PRINT(s_pInstance == nullptr, "Instance already created");
			s_pInstance = static_cast<T*>(this);
		}

		inline ~CSingleton()
		{
			s_pInstance = nullptr;
		}
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
