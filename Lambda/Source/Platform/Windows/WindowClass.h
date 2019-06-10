#pragma once
#include "Defines.h"
#if defined(LAMBDA_PLAT_WINDOWS)
	#define WIN32_LEAN_AND_MEAN
	#include <Windows.h>

namespace Lambda
{
	class WindowClass final
	{
	public:
		LAMBDA_STATIC_CLASS(WindowClass);

		static void Register(WNDCLASSEX& wc);
		static void Unregister(LPCTSTR className);

	private:
		static std::unordered_map<std::wstring, uint32> s_ClassTable;
	};
}
#endif