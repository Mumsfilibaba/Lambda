#include "LambdaPch.h"
#include "Platform.h"

#if defined(LAMBDA_PLAT_WINDOWS)
#include "../Source/Platform/Windows/WindowsPlatform.inl"
#else
#error No platform defined
#endif

namespace Lambda
{
	//-----------
	//EMessageBox
	//-----------

	void LMessageBox(const char* pCaption, const char* pText, EMessageBoxType type)
	{
		_LMessageBoxImpl(pCaption, pText, type);
	}

	//---------------------
	//Interface for console
	//---------------------

	bool LInitializeConsole()
	{
		return _LInitializeConsoleImpl();
	}


	void LSetConsoleColor(EConsoleColor color)
	{
		_LSetConsoleColorImpl(color);
	}


	void LPrintConsole(const char* pFormat, ...)
	{
		va_list args;
		va_start(args, pFormat);
		_LPrintConsoleImpl(pFormat, args);
		va_end(args);
	}


	void LReleaseConsole()
	{
		_LReleaseConsoleImpl();
	}

	//-------------------
	//Interface for mouse
	//-------------------

	void LShowCursor(bool bVisible)
	{
		_LShowMouseImpl(bVisible);
	}


	void LSetCursorPosition(int32 x, int32 y)
	{
		_LSetCursorPositionImpl(x, y);
	}


	void LGetCursorPosition(int32& outX, int32& outY)
	{
		_LGetCursorPositionImpl(outX, outY);
	}
}