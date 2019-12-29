#pragma once
#include "Defines.h"
#include "Types.h"
#ifdef MessageBox
	#undef MessageBox
#endif


namespace Lambda
{
	class CEngine;
	class CApplication;

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
		CApplication* CreateApplication(CEngine* pEngine);

		void LAMBDA_API ShowCursor(bool bVisible);
		void LAMBDA_API SetCursorPosition(int32 x, int32 y);
		void LAMBDA_API GetCursorPosition(int32& outX, int32& outY);
	}
}
