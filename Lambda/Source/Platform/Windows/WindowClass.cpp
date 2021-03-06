#include "LambdaPch.h"
#if defined(LAMBDA_PLAT_WINDOWS)
    #include "WindowClass.h"
namespace Lambda
{
	//-----------
	//WindowClass
	//-----------

	std::unordered_map<std::wstring, uint32> WindowClass::s_ClassTable;

	
    void WindowClass::Register(WNDCLASSEX& wc)
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
    

	void WindowClass::Unregister(LPCTSTR name)
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
