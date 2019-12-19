#pragma once
#include "Defines.h"
#include "Types.h"
#include "Core/Input/EKey.h"

namespace Lambda
{
	//---------------
	//EMessageBoxType
	//---------------

	enum class EMessageBoxType : uint8
	{
		MESSAGE_BOX_TYPE_NONE	= 0,
		MESSAGE_BOX_TYPE_ERROR	= 1,
	};

	//-------------
	//EConsoleColor
	//-------------

	enum class EConsoleColor : uint8
	{
		CONSOLE_COLOR_UNKNOWN	= 0,
		CONSOLE_COLOR_RED		= 1,
		CONSOLE_COLOR_BLUE		= 2,
		CONSOLE_COLOR_GREEN		= 3,
		CONSOLE_COLOR_YELLOW	= 4,
		CONSOLE_COLOR_WHITE		= 5,
	};

	//-----------
	//EMessageBox
	//-----------

	void LMessageBox(const char* pCaption, const char* pText, EMessageBoxType type);

    //---------------------
    //Interface for console
    //---------------------

    bool LInitializeConsole();
    void LSetConsoleColor(EConsoleColor color);
    void LPrintConsole(const char* pFormat, ...);
    void LReleaseConsole();

	//-------------------
	//Interface for mouse
	//-------------------

	void LShowCursor(bool bVisible);
	void LSetCursorPosition(int32 x, int32 y);
	void LGetCursorPosition(int32& outX, int32& outY);

	//-------------------------------------------------------
	//Functions for converting keycodes from system to engine
	//-------------------------------------------------------

	void LInitializeKeysLookupTable();
	EKey LConvertKeyFromSystem(uint32 key);
	uint32 LSystemKeyFromEngine(EKey key);
}