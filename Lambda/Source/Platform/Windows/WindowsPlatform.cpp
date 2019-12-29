#include "LambdaPch.h"
#include "Core/Input/EKey.h"

#if defined(LAMBDA_PLAT_WINDOWS)
	#include "Platform/Windows/WindowsPlatform.h"
	#include "Platform/Windows/CWindowsApplication.h"

namespace Lambda
{
	//--------------
	//WindowPlatform
	//--------------
	namespace WindowsPlatform
	{
		void MessageBox(const char* pCaption, const char* pText, EMessageBoxType type)
		{
			UINT uType = 0;
			if (type == EMessageBoxType::MESSAGE_BOX_TYPE_ERROR)
			{
				uType = MB_OK | MB_ICONERROR;
			}

			::MessageBoxA(0, pText, pCaption, uType);
		}


		CApplication* CreateApplication(CEngine* pEngine)
		{
			return DBG_NEW CWindowsApplication(pEngine);
		}


		EKey	g_WindowsToLambdaKeyTable[256];
		uint32	g_LambdaToWindowsKeyTable[uint32(EKey::KEY_LAST)+1];

		void InitializeKeycodeLookupTable()
		{
			//Set to zero
			memset(g_WindowsToLambdaKeyTable, 0, sizeof(g_WindowsToLambdaKeyTable));
			memset(g_LambdaToWindowsKeyTable, 0, sizeof(g_LambdaToWindowsKeyTable));

			g_WindowsToLambdaKeyTable[0x08] = EKey::KEY_BACKSPACE;
			g_WindowsToLambdaKeyTable[0x09] = EKey::KEY_TAB;
			g_WindowsToLambdaKeyTable[0x0D] = EKey::KEY_ENTER;
			g_WindowsToLambdaKeyTable[0x14] = EKey::KEY_PAUSE;
			g_WindowsToLambdaKeyTable[0x14] = EKey::KEY_CAPS_LOCK;
			g_WindowsToLambdaKeyTable[0x20] = EKey::KEY_SPACE;
			g_WindowsToLambdaKeyTable[0x21] = EKey::KEY_PAGE_UP;
			g_WindowsToLambdaKeyTable[0x22] = EKey::KEY_PAGE_DOWN;
			g_WindowsToLambdaKeyTable[0x23] = EKey::KEY_END;
			g_WindowsToLambdaKeyTable[0x24] = EKey::KEY_HOME;
			g_WindowsToLambdaKeyTable[0x25] = EKey::KEY_LEFT;
			g_WindowsToLambdaKeyTable[0x26] = EKey::KEY_UP;
			g_WindowsToLambdaKeyTable[0x27] = EKey::KEY_RIGHT;
			g_WindowsToLambdaKeyTable[0x28] = EKey::KEY_DOWN;
			g_WindowsToLambdaKeyTable[0x2C] = EKey::KEY_PRINT_SCREEN;
			g_WindowsToLambdaKeyTable[0x2D] = EKey::KEY_INSERT;
			g_WindowsToLambdaKeyTable[0x2E] = EKey::KEY_DELETE;
			g_WindowsToLambdaKeyTable[0x1B] = EKey::KEY_ESCAPE;

			g_WindowsToLambdaKeyTable[0x30] = EKey::KEY_0;
			g_WindowsToLambdaKeyTable[0x31] = EKey::KEY_1;
			g_WindowsToLambdaKeyTable[0x32] = EKey::KEY_2;
			g_WindowsToLambdaKeyTable[0x33] = EKey::KEY_3;
			g_WindowsToLambdaKeyTable[0x34] = EKey::KEY_4;
			g_WindowsToLambdaKeyTable[0x35] = EKey::KEY_5;
			g_WindowsToLambdaKeyTable[0x36] = EKey::KEY_6;
			g_WindowsToLambdaKeyTable[0x37] = EKey::KEY_7;
			g_WindowsToLambdaKeyTable[0x38] = EKey::KEY_8;
			g_WindowsToLambdaKeyTable[0x39] = EKey::KEY_9;

			g_WindowsToLambdaKeyTable[0x41] = EKey::KEY_A;
			g_WindowsToLambdaKeyTable[0x42] = EKey::KEY_B;
			g_WindowsToLambdaKeyTable[0x43] = EKey::KEY_C;
			g_WindowsToLambdaKeyTable[0x44] = EKey::KEY_D;
			g_WindowsToLambdaKeyTable[0x45] = EKey::KEY_E;
			g_WindowsToLambdaKeyTable[0x46] = EKey::KEY_F;
			g_WindowsToLambdaKeyTable[0x47] = EKey::KEY_G;
			g_WindowsToLambdaKeyTable[0x48] = EKey::KEY_H;
			g_WindowsToLambdaKeyTable[0x49] = EKey::KEY_I;
			g_WindowsToLambdaKeyTable[0x4A] = EKey::KEY_J;
			g_WindowsToLambdaKeyTable[0x4B] = EKey::KEY_K;
			g_WindowsToLambdaKeyTable[0x4C] = EKey::KEY_L;
			g_WindowsToLambdaKeyTable[0x4D] = EKey::KEY_M;
			g_WindowsToLambdaKeyTable[0x4E] = EKey::KEY_N;
			g_WindowsToLambdaKeyTable[0x4F] = EKey::KEY_O;
			g_WindowsToLambdaKeyTable[0x50] = EKey::KEY_P;
			g_WindowsToLambdaKeyTable[0x51] = EKey::KEY_Q;
			g_WindowsToLambdaKeyTable[0x52] = EKey::KEY_R;
			g_WindowsToLambdaKeyTable[0x53] = EKey::KEY_S;
			g_WindowsToLambdaKeyTable[0x54] = EKey::KEY_T;
			g_WindowsToLambdaKeyTable[0x55] = EKey::KEY_U;
			g_WindowsToLambdaKeyTable[0x56] = EKey::KEY_V;
			g_WindowsToLambdaKeyTable[0x57] = EKey::KEY_W;
			g_WindowsToLambdaKeyTable[0x58] = EKey::KEY_X;
			g_WindowsToLambdaKeyTable[0x59] = EKey::KEY_Y;
			g_WindowsToLambdaKeyTable[0x5A] = EKey::KEY_Z;

			g_WindowsToLambdaKeyTable[0x60] = EKey::KEY_KEYPAD_0;
			g_WindowsToLambdaKeyTable[0x61] = EKey::KEY_KEYPAD_1;
			g_WindowsToLambdaKeyTable[0x62] = EKey::KEY_KEYPAD_2;
			g_WindowsToLambdaKeyTable[0x63] = EKey::KEY_KEYPAD_3;
			g_WindowsToLambdaKeyTable[0x64] = EKey::KEY_KEYPAD_4;
			g_WindowsToLambdaKeyTable[0x65] = EKey::KEY_KEYPAD_5;
			g_WindowsToLambdaKeyTable[0x66] = EKey::KEY_KEYPAD_6;
			g_WindowsToLambdaKeyTable[0x67] = EKey::KEY_KEYPAD_7;
			g_WindowsToLambdaKeyTable[0x68] = EKey::KEY_KEYPAD_8;
			g_WindowsToLambdaKeyTable[0x69] = EKey::KEY_KEYPAD_9;
			g_WindowsToLambdaKeyTable[0x6A] = EKey::KEY_KEYPAD_MULTIPLY;
			g_WindowsToLambdaKeyTable[0x6B] = EKey::KEY_KEYPAD_ADD;
			g_WindowsToLambdaKeyTable[0x6D] = EKey::KEY_KEYPAD_SUBTRACT;
			g_WindowsToLambdaKeyTable[0x6E] = EKey::KEY_KEYPAD_DECIMAL;
			g_WindowsToLambdaKeyTable[0x6F] = EKey::KEY_KEYPAD_DIVIDE;

			g_WindowsToLambdaKeyTable[0x70] = EKey::KEY_F1;
			g_WindowsToLambdaKeyTable[0x71] = EKey::KEY_F2;
			g_WindowsToLambdaKeyTable[0x72] = EKey::KEY_F3;
			g_WindowsToLambdaKeyTable[0x73] = EKey::KEY_F4;
			g_WindowsToLambdaKeyTable[0x74] = EKey::KEY_F5;
			g_WindowsToLambdaKeyTable[0x75] = EKey::KEY_F6;
			g_WindowsToLambdaKeyTable[0x76] = EKey::KEY_F7;
			g_WindowsToLambdaKeyTable[0x77] = EKey::KEY_F8;
			g_WindowsToLambdaKeyTable[0x78] = EKey::KEY_F9;
			g_WindowsToLambdaKeyTable[0x79] = EKey::KEY_F10;
			g_WindowsToLambdaKeyTable[0x7A] = EKey::KEY_F11;
			g_WindowsToLambdaKeyTable[0x7B] = EKey::KEY_F12;
			g_WindowsToLambdaKeyTable[0x7C] = EKey::KEY_F13;
			g_WindowsToLambdaKeyTable[0x7D] = EKey::KEY_F14;
			g_WindowsToLambdaKeyTable[0x7E] = EKey::KEY_F15;
			g_WindowsToLambdaKeyTable[0x7F] = EKey::KEY_F16;
			g_WindowsToLambdaKeyTable[0x80] = EKey::KEY_F17;
			g_WindowsToLambdaKeyTable[0x81] = EKey::KEY_F18;
			g_WindowsToLambdaKeyTable[0x82] = EKey::KEY_F19;
			g_WindowsToLambdaKeyTable[0x83] = EKey::KEY_F20;
			g_WindowsToLambdaKeyTable[0x84] = EKey::KEY_F21;
			g_WindowsToLambdaKeyTable[0x85] = EKey::KEY_F22;
			g_WindowsToLambdaKeyTable[0x86] = EKey::KEY_F23;
			g_WindowsToLambdaKeyTable[0x87] = EKey::KEY_F24;

			g_WindowsToLambdaKeyTable[0x90] = EKey::KEY_NUM_LOCK;
			g_WindowsToLambdaKeyTable[0x91] = EKey::KEY_SCROLL_LOCK;

			g_WindowsToLambdaKeyTable[0xBA] = EKey::KEY_SEMICOLON;
			g_WindowsToLambdaKeyTable[0xBC] = EKey::KEY_COMMA;
			g_WindowsToLambdaKeyTable[0xBD] = EKey::KEY_MINUS;
			g_WindowsToLambdaKeyTable[0xBE] = EKey::KEY_PERIOD;
			g_WindowsToLambdaKeyTable[0xC0] = EKey::KEY_GRAVE_ACCENT;
			g_WindowsToLambdaKeyTable[0xDB] = EKey::KEY_LEFT_BRACKET;
			g_WindowsToLambdaKeyTable[0xDD] = EKey::KEY_RIGHT_BRACKET;
			g_WindowsToLambdaKeyTable[0xDE] = EKey::KEY_APOSTROPHE;
			g_WindowsToLambdaKeyTable[0xE2] = EKey::KEY_BACKSLASH;

			g_WindowsToLambdaKeyTable[0xA0] = EKey::KEY_LEFT_SHIFT;
			g_WindowsToLambdaKeyTable[0xA2] = EKey::KEY_LEFT_CONTROL;
			g_WindowsToLambdaKeyTable[0xA4] = EKey::KEY_LEFT_ALT;
			g_WindowsToLambdaKeyTable[0x5B] = EKey::KEY_LEFT_SUPER;
			g_WindowsToLambdaKeyTable[0xA1] = EKey::KEY_RIGHT_SHIFT;
			g_WindowsToLambdaKeyTable[0xA3] = EKey::KEY_RIGHT_CONTROL;
			g_WindowsToLambdaKeyTable[0xA5] = EKey::KEY_RIGHT_ALT;
			g_WindowsToLambdaKeyTable[0x5C] = EKey::KEY_RIGHT_SUPER;
			g_WindowsToLambdaKeyTable[0x5C] = EKey::KEY_MENU;

			///////////////////////////////////////////////////

			g_LambdaToWindowsKeyTable[uint32(EKey::KEY_BACKSPACE)]		= 0x08;
			g_LambdaToWindowsKeyTable[uint32(EKey::KEY_TAB)]			= 0x09;
			g_LambdaToWindowsKeyTable[uint32(EKey::KEY_ENTER)]			= 0x0D;
			g_LambdaToWindowsKeyTable[uint32(EKey::KEY_PAUSE)]			= 0x14;
			g_LambdaToWindowsKeyTable[uint32(EKey::KEY_CAPS_LOCK)]		= 0x14;
			g_LambdaToWindowsKeyTable[uint32(EKey::KEY_SPACE)]			= 0x20;
			g_LambdaToWindowsKeyTable[uint32(EKey::KEY_PAGE_UP)]		= 0x21;
			g_LambdaToWindowsKeyTable[uint32(EKey::KEY_PAGE_DOWN)]		= 0x22;
			g_LambdaToWindowsKeyTable[uint32(EKey::KEY_END)]			= 0x23;
			g_LambdaToWindowsKeyTable[uint32(EKey::KEY_HOME)]			= 0x24;
			g_LambdaToWindowsKeyTable[uint32(EKey::KEY_LEFT)]			= 0x25;
			g_LambdaToWindowsKeyTable[uint32(EKey::KEY_UP)]				= 0x26;
			g_LambdaToWindowsKeyTable[uint32(EKey::KEY_RIGHT)]			= 0x27;
			g_LambdaToWindowsKeyTable[uint32(EKey::KEY_DOWN)]			= 0x28;
			g_LambdaToWindowsKeyTable[uint32(EKey::KEY_PRINT_SCREEN)]	= 0x2C;
			g_LambdaToWindowsKeyTable[uint32(EKey::KEY_INSERT)]			= 0x2D;
			g_LambdaToWindowsKeyTable[uint32(EKey::KEY_DELETE)]			= 0x2E;
			g_LambdaToWindowsKeyTable[uint32(EKey::KEY_ESCAPE)]			= 0x1B;

			g_LambdaToWindowsKeyTable[uint32(EKey::KEY_0)] = 0x30;
			g_LambdaToWindowsKeyTable[uint32(EKey::KEY_1)] = 0x31;
			g_LambdaToWindowsKeyTable[uint32(EKey::KEY_2)] = 0x32;
			g_LambdaToWindowsKeyTable[uint32(EKey::KEY_3)] = 0x33;
			g_LambdaToWindowsKeyTable[uint32(EKey::KEY_4)] = 0x34;
			g_LambdaToWindowsKeyTable[uint32(EKey::KEY_5)] = 0x35;
			g_LambdaToWindowsKeyTable[uint32(EKey::KEY_6)] = 0x36;
			g_LambdaToWindowsKeyTable[uint32(EKey::KEY_7)] = 0x37;
			g_LambdaToWindowsKeyTable[uint32(EKey::KEY_8)] = 0x38;
			g_LambdaToWindowsKeyTable[uint32(EKey::KEY_9)] = 0x39;

			g_LambdaToWindowsKeyTable[uint32(EKey::KEY_A)] = 0x41;
			g_LambdaToWindowsKeyTable[uint32(EKey::KEY_B)] = 0x42;
			g_LambdaToWindowsKeyTable[uint32(EKey::KEY_C)] = 0x43;
			g_LambdaToWindowsKeyTable[uint32(EKey::KEY_D)] = 0x44;
			g_LambdaToWindowsKeyTable[uint32(EKey::KEY_E)] = 0x45;
			g_LambdaToWindowsKeyTable[uint32(EKey::KEY_F)] = 0x46;
			g_LambdaToWindowsKeyTable[uint32(EKey::KEY_G)] = 0x47;
			g_LambdaToWindowsKeyTable[uint32(EKey::KEY_H)] = 0x48;
			g_LambdaToWindowsKeyTable[uint32(EKey::KEY_I)] = 0x49;
			g_LambdaToWindowsKeyTable[uint32(EKey::KEY_J)] = 0x4A;
			g_LambdaToWindowsKeyTable[uint32(EKey::KEY_K)] = 0x4B;
			g_LambdaToWindowsKeyTable[uint32(EKey::KEY_L)] = 0x4C;
			g_LambdaToWindowsKeyTable[uint32(EKey::KEY_M)] = 0x4D;
			g_LambdaToWindowsKeyTable[uint32(EKey::KEY_N)] = 0x4E;
			g_LambdaToWindowsKeyTable[uint32(EKey::KEY_O)] = 0x4F;
			g_LambdaToWindowsKeyTable[uint32(EKey::KEY_P)] = 0x50;
			g_LambdaToWindowsKeyTable[uint32(EKey::KEY_Q)] = 0x51;
			g_LambdaToWindowsKeyTable[uint32(EKey::KEY_R)] = 0x52;
			g_LambdaToWindowsKeyTable[uint32(EKey::KEY_S)] = 0x53;
			g_LambdaToWindowsKeyTable[uint32(EKey::KEY_T)] = 0x54;
			g_LambdaToWindowsKeyTable[uint32(EKey::KEY_U)] = 0x55;
			g_LambdaToWindowsKeyTable[uint32(EKey::KEY_V)] = 0x56;
			g_LambdaToWindowsKeyTable[uint32(EKey::KEY_W)] = 0x57;
			g_LambdaToWindowsKeyTable[uint32(EKey::KEY_X)] = 0x58;
			g_LambdaToWindowsKeyTable[uint32(EKey::KEY_Y)] = 0x59;
			g_LambdaToWindowsKeyTable[uint32(EKey::KEY_Z)] = 0x5A;

			g_LambdaToWindowsKeyTable[uint32(EKey::KEY_KEYPAD_0)]			= 0x60;
			g_LambdaToWindowsKeyTable[uint32(EKey::KEY_KEYPAD_1)]			= 0x61;
			g_LambdaToWindowsKeyTable[uint32(EKey::KEY_KEYPAD_2)]			= 0x62;
			g_LambdaToWindowsKeyTable[uint32(EKey::KEY_KEYPAD_3)]			= 0x63;
			g_LambdaToWindowsKeyTable[uint32(EKey::KEY_KEYPAD_4)]			= 0x64;
			g_LambdaToWindowsKeyTable[uint32(EKey::KEY_KEYPAD_5)]			= 0x65;
			g_LambdaToWindowsKeyTable[uint32(EKey::KEY_KEYPAD_6)]			= 0x66;
			g_LambdaToWindowsKeyTable[uint32(EKey::KEY_KEYPAD_7)]			= 0x67;
			g_LambdaToWindowsKeyTable[uint32(EKey::KEY_KEYPAD_8)]			= 0x68;
			g_LambdaToWindowsKeyTable[uint32(EKey::KEY_KEYPAD_9)]			= 0x69;
			g_LambdaToWindowsKeyTable[uint32(EKey::KEY_KEYPAD_MULTIPLY)]	= 0x6A;
			g_LambdaToWindowsKeyTable[uint32(EKey::KEY_KEYPAD_ADD)]			= 0x6B;
			g_LambdaToWindowsKeyTable[uint32(EKey::KEY_KEYPAD_SUBTRACT)]	= 0x6D;
			g_LambdaToWindowsKeyTable[uint32(EKey::KEY_KEYPAD_DECIMAL)]		= 0x6E;
			g_LambdaToWindowsKeyTable[uint32(EKey::KEY_KEYPAD_DIVIDE)]		= 0x6F;

			g_LambdaToWindowsKeyTable[uint32(EKey::KEY_F1)]		= 0x70;
			g_LambdaToWindowsKeyTable[uint32(EKey::KEY_F2)]		= 0x71;
			g_LambdaToWindowsKeyTable[uint32(EKey::KEY_F3)]		= 0x72;
			g_LambdaToWindowsKeyTable[uint32(EKey::KEY_F4)]		= 0x73;
			g_LambdaToWindowsKeyTable[uint32(EKey::KEY_F5)]		= 0x74;
			g_LambdaToWindowsKeyTable[uint32(EKey::KEY_F6)]		= 0x75;
			g_LambdaToWindowsKeyTable[uint32(EKey::KEY_F7)]		= 0x76;
			g_LambdaToWindowsKeyTable[uint32(EKey::KEY_F8)]		= 0x77;
			g_LambdaToWindowsKeyTable[uint32(EKey::KEY_F9)]		= 0x78;
			g_LambdaToWindowsKeyTable[uint32(EKey::KEY_F10)]	= 0x79;
			g_LambdaToWindowsKeyTable[uint32(EKey::KEY_F11)]	= 0x7A;
			g_LambdaToWindowsKeyTable[uint32(EKey::KEY_F12)]	= 0x7B;
			g_LambdaToWindowsKeyTable[uint32(EKey::KEY_F13)]	= 0x7C;
			g_LambdaToWindowsKeyTable[uint32(EKey::KEY_F14)]	= 0x7D;
			g_LambdaToWindowsKeyTable[uint32(EKey::KEY_F15)]	= 0x7E;
			g_LambdaToWindowsKeyTable[uint32(EKey::KEY_F16)]	= 0x7F;
			g_LambdaToWindowsKeyTable[uint32(EKey::KEY_F17)]	= 0x80;
			g_LambdaToWindowsKeyTable[uint32(EKey::KEY_F18)]	= 0x81;
			g_LambdaToWindowsKeyTable[uint32(EKey::KEY_F19)]	= 0x82;
			g_LambdaToWindowsKeyTable[uint32(EKey::KEY_F20)]	= 0x83;
			g_LambdaToWindowsKeyTable[uint32(EKey::KEY_F21)]	= 0x84;
			g_LambdaToWindowsKeyTable[uint32(EKey::KEY_F22)]	= 0x85;
			g_LambdaToWindowsKeyTable[uint32(EKey::KEY_F23)]	= 0x86;
			g_LambdaToWindowsKeyTable[uint32(EKey::KEY_F24)]	= 0x87;

			g_LambdaToWindowsKeyTable[uint32(EKey::KEY_NUM_LOCK)]		= 0x90;
			g_LambdaToWindowsKeyTable[uint32(EKey::KEY_SCROLL_LOCK)]	= 0x91;

			g_LambdaToWindowsKeyTable[uint32(EKey::KEY_SEMICOLON)]		= 0xBA;
			g_LambdaToWindowsKeyTable[uint32(EKey::KEY_COMMA)]			= 0xBC;
			g_LambdaToWindowsKeyTable[uint32(EKey::KEY_MINUS)]			= 0xBD;
			g_LambdaToWindowsKeyTable[uint32(EKey::KEY_PERIOD)]			= 0xBE;
			g_LambdaToWindowsKeyTable[uint32(EKey::KEY_GRAVE_ACCENT)]	= 0xC0;
			g_LambdaToWindowsKeyTable[uint32(EKey::KEY_LEFT_BRACKET)]	= 0xDB;
			g_LambdaToWindowsKeyTable[uint32(EKey::KEY_RIGHT_BRACKET)]	= 0xDD;
			g_LambdaToWindowsKeyTable[uint32(EKey::KEY_APOSTROPHE)]		= 0xDE;
			g_LambdaToWindowsKeyTable[uint32(EKey::KEY_BACKSLASH)]		= 0xE2;

			g_LambdaToWindowsKeyTable[uint32(EKey::KEY_LEFT_SHIFT)]		= 0xA0;
			g_LambdaToWindowsKeyTable[uint32(EKey::KEY_LEFT_CONTROL)]	= 0xA2;
			g_LambdaToWindowsKeyTable[uint32(EKey::KEY_LEFT_ALT)]		= 0xA4;
			g_LambdaToWindowsKeyTable[uint32(EKey::KEY_LEFT_SUPER)]		= 0x5B;
			g_LambdaToWindowsKeyTable[uint32(EKey::KEY_RIGHT_SHIFT)]	= 0xA1;
			g_LambdaToWindowsKeyTable[uint32(EKey::KEY_RIGHT_CONTROL)]	= 0xA3;
			g_LambdaToWindowsKeyTable[uint32(EKey::KEY_RIGHT_ALT)]		= 0xA5;
			g_LambdaToWindowsKeyTable[uint32(EKey::KEY_RIGHT_SUPER)]	= 0x5C;
			g_LambdaToWindowsKeyTable[uint32(EKey::KEY_MENU)]			= 0x5C;
		}

		//---------------------------------
		//CWindowPlatform - Mouse Interface
		//---------------------------------

		void ShowCursor(bool bVisible)
		{
			if (bVisible)
			{
				::ShowCursor(TRUE);
			}
			else
			{
				::ShowCursor(FALSE);
			}
		}


		void SetCursorPosition(int32 x, int32 y)
		{
			::SetCursorPos(x, y);
		}


		void GetCursorPosition(int32& outX, int32& outY)
		{
			POINT cursor = {};
			::GetCursorPos(&cursor);

			outX = cursor.x;
			outY = cursor.y;
		}
	}
}
#endif