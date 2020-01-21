#include "LambdaPch.h"

#include "Platform/Windows/WindowsMisc.h"

namespace Lambda
{
	void CWindowsMisc::MessageBox(const char* pCaption, const char* pText, uint32 type)
	{
		uint32 uType = 0;
		if (type & EMessageBox::MESSAGE_BOX_OK)
			uType |= MB_OK;
		if (type & EMessageBox::MESSAGE_BOX_ERRORICON)
			uType |= MB_ICONERROR;

		MessageBoxA(0, pText, pCaption, uType);
	}

	void CWindowsMisc::DebuggerOutput(const char* pMessage)
	{
		OutputDebugStringA(pMessage);
	}
}