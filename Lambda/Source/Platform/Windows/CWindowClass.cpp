#include "LambdaPch.h"
#if defined(LAMBDA_PLAT_WINDOWS)
#include "CWindowClass.h"

namespace Lambda
{
	//------------
	//CWindowClass
	//------------

	std::unordered_map<std::wstring, uint32> CWindowClass::s_ClassTable;

	
    void CWindowClass::Register(WNDCLASSEX& wc)
	{
		auto entry = s_ClassTable.find(wc.lpszClassName);
		if (entry != s_ClassTable.end())
		{
			entry->second++;
		}
		else
		{
			RegisterClassEx(&wc);
			s_ClassTable[wc.lpszClassName] = 1;
		}
	}
    

	void CWindowClass::Unregister(LPCTSTR name)
	{
		auto entry = s_ClassTable.find(name);
		if (entry != s_ClassTable.end())
		{
			if (entry->second < 2)
			{
				UnregisterClass(name, reinterpret_cast<HINSTANCE>(GetModuleHandle(NULL)));
				s_ClassTable.erase(entry);
			}
			else
			{
				entry->second--;
			}
		}
	}
}
#endif
