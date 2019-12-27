#pragma once
#include "Defines.h"
#include "Types.h"

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

	//--------
	//Platform
	//--------
	namespace Platform
	{
		void LAMBDA_API MessageBox(const char* pCaption, const char* pText, EMessageBoxType type);

		void LAMBDA_API ShowCursor(bool bVisible);
		void LAMBDA_API SetCursorPosition(int32 x, int32 y);
		void LAMBDA_API GetCursorPosition(int32& outX, int32& outY);
	}
}
