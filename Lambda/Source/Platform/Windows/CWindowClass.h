#pragma once
#include "Defines.h"
#if defined(LAMBDA_PLAT_WINDOWS)
	#if !defined(WIN32_LEAN_AND_MEAN)
		#define WIN32_LEAN_AND_MEAN 1
	#endif
	#include <Windows.h>

namespace Lambda
{
	//------------
	//CWindowClass
	//------------

	class CWindowClass final
	{
	public:
		LAMBDA_STATIC_CLASS(CWindowClass);

		static void Register(WNDCLASSEX& wc);
		static void Unregister(LPCTSTR className);
	private:
		static std::unordered_map<std::wstring, uint32> s_ClassTable;
	};
}
#endif