#pragma once
#include "Defines.h"
#include "Types.h"
#include <cassert>

namespace Lambda
{
	//---------
	//Singleton
	//---------

	template <typename T>
	class Singleton
	{
	protected:
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

	template <typename T>
	T* Singleton<T>::s_pInstance = nullptr;
}