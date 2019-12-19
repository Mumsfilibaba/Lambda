#include "LambdaPch.h"
#include "Platform.h"

namespace Lambda
{
	//-------------------------------------------------------
	//Functions for converting keycodes from system to engine
	//-------------------------------------------------------
	
	EKey g_WindowsToLambdaKeyTable[256];
	uint32 g_LambdaToWindowsKeyTable[KEY_LAST + 1];


	EKey LConvertKeyFromSystem(uint32 key)
	{
		return g_WindowsToLambdaKeyTable[key];
	}


	uint32 LSystemKeyFromEngine(EKey key)
	{
		return g_LambdaToWindowsKeyTable[key];
	}


	void LInitializeKeysLookupTable()
	{
		//Set to zero
		memset(g_WindowsToLambdaKeyTable, 0, sizeof(g_WindowsToLambdaKeyTable));
		memset(g_LambdaToWindowsKeyTable, 0, sizeof(g_LambdaToWindowsKeyTable));

		g_WindowsToLambdaKeyTable[0x08] = KEY_BACKSPACE;
		g_WindowsToLambdaKeyTable[0x09] = KEY_TAB;
		g_WindowsToLambdaKeyTable[0x0D] = KEY_ENTER;
		g_WindowsToLambdaKeyTable[0x14] = KEY_PAUSE;
		g_WindowsToLambdaKeyTable[0x14] = KEY_CAPS_LOCK;
		g_WindowsToLambdaKeyTable[0x20] = KEY_SPACE;
		g_WindowsToLambdaKeyTable[0x21] = KEY_PAGE_UP;
		g_WindowsToLambdaKeyTable[0x22] = KEY_PAGE_DOWN;
		g_WindowsToLambdaKeyTable[0x23] = KEY_END;
		g_WindowsToLambdaKeyTable[0x24] = KEY_HOME;
		g_WindowsToLambdaKeyTable[0x25] = KEY_LEFT;
		g_WindowsToLambdaKeyTable[0x26] = KEY_UP;
		g_WindowsToLambdaKeyTable[0x27] = KEY_RIGHT;
		g_WindowsToLambdaKeyTable[0x28] = KEY_DOWN;
		g_WindowsToLambdaKeyTable[0x2C] = KEY_PRINT_SCREEN;
		g_WindowsToLambdaKeyTable[0x2D] = KEY_INSERT;
		g_WindowsToLambdaKeyTable[0x2E] = KEY_DELETE;
		g_WindowsToLambdaKeyTable[0x1B] = KEY_ESCAPE;

		g_WindowsToLambdaKeyTable[0x30] = KEY_0;
		g_WindowsToLambdaKeyTable[0x31] = KEY_1;
		g_WindowsToLambdaKeyTable[0x32] = KEY_2;
		g_WindowsToLambdaKeyTable[0x33] = KEY_3;
		g_WindowsToLambdaKeyTable[0x34] = KEY_4;
		g_WindowsToLambdaKeyTable[0x35] = KEY_5;
		g_WindowsToLambdaKeyTable[0x36] = KEY_6;
		g_WindowsToLambdaKeyTable[0x37] = KEY_7;
		g_WindowsToLambdaKeyTable[0x38] = KEY_8;
		g_WindowsToLambdaKeyTable[0x39] = KEY_9;

		g_WindowsToLambdaKeyTable[0x41] = KEY_A;
		g_WindowsToLambdaKeyTable[0x42] = KEY_B;
		g_WindowsToLambdaKeyTable[0x43] = KEY_C;
		g_WindowsToLambdaKeyTable[0x44] = KEY_D;
		g_WindowsToLambdaKeyTable[0x45] = KEY_E;
		g_WindowsToLambdaKeyTable[0x46] = KEY_F;
		g_WindowsToLambdaKeyTable[0x47] = KEY_G;
		g_WindowsToLambdaKeyTable[0x48] = KEY_H;
		g_WindowsToLambdaKeyTable[0x49] = KEY_I;
		g_WindowsToLambdaKeyTable[0x4A] = KEY_J;
		g_WindowsToLambdaKeyTable[0x4B] = KEY_K;
		g_WindowsToLambdaKeyTable[0x4C] = KEY_L;
		g_WindowsToLambdaKeyTable[0x4D] = KEY_M;
		g_WindowsToLambdaKeyTable[0x4E] = KEY_N;
		g_WindowsToLambdaKeyTable[0x4F] = KEY_O;
		g_WindowsToLambdaKeyTable[0x50] = KEY_P;
		g_WindowsToLambdaKeyTable[0x51] = KEY_Q;
		g_WindowsToLambdaKeyTable[0x52] = KEY_R;
		g_WindowsToLambdaKeyTable[0x53] = KEY_S;
		g_WindowsToLambdaKeyTable[0x54] = KEY_T;
		g_WindowsToLambdaKeyTable[0x55] = KEY_U;
		g_WindowsToLambdaKeyTable[0x56] = KEY_V;
		g_WindowsToLambdaKeyTable[0x57] = KEY_W;
		g_WindowsToLambdaKeyTable[0x58] = KEY_X;
		g_WindowsToLambdaKeyTable[0x59] = KEY_Y;
		g_WindowsToLambdaKeyTable[0x5A] = KEY_Z;

		g_WindowsToLambdaKeyTable[0x60] = KEY_KEYPAD_0;
		g_WindowsToLambdaKeyTable[0x61] = KEY_KEYPAD_1;
		g_WindowsToLambdaKeyTable[0x62] = KEY_KEYPAD_2;
		g_WindowsToLambdaKeyTable[0x63] = KEY_KEYPAD_3;
		g_WindowsToLambdaKeyTable[0x64] = KEY_KEYPAD_4;
		g_WindowsToLambdaKeyTable[0x65] = KEY_KEYPAD_5;
		g_WindowsToLambdaKeyTable[0x66] = KEY_KEYPAD_6;
		g_WindowsToLambdaKeyTable[0x67] = KEY_KEYPAD_7;
		g_WindowsToLambdaKeyTable[0x68] = KEY_KEYPAD_8;
		g_WindowsToLambdaKeyTable[0x69] = KEY_KEYPAD_9;
		g_WindowsToLambdaKeyTable[0x6A] = KEY_KEYPAD_MULTIPLY;
		g_WindowsToLambdaKeyTable[0x6B] = KEY_KEYPAD_ADD;
		g_WindowsToLambdaKeyTable[0x6D] = KEY_KEYPAD_SUBTRACT;
		g_WindowsToLambdaKeyTable[0x6E] = KEY_KEYPAD_DECIMAL;
		g_WindowsToLambdaKeyTable[0x6F] = KEY_KEYPAD_DIVIDE;

		g_WindowsToLambdaKeyTable[0x70] = KEY_F1;
		g_WindowsToLambdaKeyTable[0x71] = KEY_F2;
		g_WindowsToLambdaKeyTable[0x72] = KEY_F3;
		g_WindowsToLambdaKeyTable[0x73] = KEY_F4;
		g_WindowsToLambdaKeyTable[0x74] = KEY_F5;
		g_WindowsToLambdaKeyTable[0x75] = KEY_F6;
		g_WindowsToLambdaKeyTable[0x76] = KEY_F7;
		g_WindowsToLambdaKeyTable[0x77] = KEY_F8;
		g_WindowsToLambdaKeyTable[0x78] = KEY_F9;
		g_WindowsToLambdaKeyTable[0x79] = KEY_F10;
		g_WindowsToLambdaKeyTable[0x7A] = KEY_F11;
		g_WindowsToLambdaKeyTable[0x7B] = KEY_F12;
		g_WindowsToLambdaKeyTable[0x7C] = KEY_F13;
		g_WindowsToLambdaKeyTable[0x7D] = KEY_F14;
		g_WindowsToLambdaKeyTable[0x7E] = KEY_F15;
		g_WindowsToLambdaKeyTable[0x7F] = KEY_F16;
		g_WindowsToLambdaKeyTable[0x80] = KEY_F17;
		g_WindowsToLambdaKeyTable[0x81] = KEY_F18;
		g_WindowsToLambdaKeyTable[0x82] = KEY_F19;
		g_WindowsToLambdaKeyTable[0x83] = KEY_F20;
		g_WindowsToLambdaKeyTable[0x84] = KEY_F21;
		g_WindowsToLambdaKeyTable[0x85] = KEY_F22;
		g_WindowsToLambdaKeyTable[0x86] = KEY_F23;
		g_WindowsToLambdaKeyTable[0x87] = KEY_F24;

		g_WindowsToLambdaKeyTable[0x90] = KEY_NUM_LOCK;
		g_WindowsToLambdaKeyTable[0x91] = KEY_SCROLL_LOCK;

		g_WindowsToLambdaKeyTable[0xBA] = KEY_SEMICOLON;
		g_WindowsToLambdaKeyTable[0xBC] = KEY_COMMA;
		g_WindowsToLambdaKeyTable[0xBD] = KEY_MINUS;
		g_WindowsToLambdaKeyTable[0xBE] = KEY_PERIOD;
		g_WindowsToLambdaKeyTable[0xC0] = KEY_GRAVE_ACCENT;
		g_WindowsToLambdaKeyTable[0xDB] = KEY_LEFT_BRACKET;
		g_WindowsToLambdaKeyTable[0xDD] = KEY_RIGHT_BRACKET;
		g_WindowsToLambdaKeyTable[0xDE] = KEY_APOSTROPHE;
		g_WindowsToLambdaKeyTable[0xE2] = KEY_BACKSLASH;

		g_WindowsToLambdaKeyTable[0xA0] = KEY_LEFT_SHIFT;
		g_WindowsToLambdaKeyTable[0xA2] = KEY_LEFT_CONTROL;
		g_WindowsToLambdaKeyTable[0xA4] = KEY_LEFT_ALT;
		g_WindowsToLambdaKeyTable[0x5B] = KEY_LEFT_SUPER;
		g_WindowsToLambdaKeyTable[0xA1] = KEY_RIGHT_SHIFT;
		g_WindowsToLambdaKeyTable[0xA3] = KEY_RIGHT_CONTROL;
		g_WindowsToLambdaKeyTable[0xA5] = KEY_RIGHT_ALT;
		g_WindowsToLambdaKeyTable[0x5C] = KEY_RIGHT_SUPER;
		g_WindowsToLambdaKeyTable[0x5C] = KEY_MENU;

		///////////////////////////////////////////////////

		g_LambdaToWindowsKeyTable[KEY_BACKSPACE] = 0x08;
		g_LambdaToWindowsKeyTable[KEY_TAB] = 0x09;
		g_LambdaToWindowsKeyTable[KEY_ENTER] = 0x0D;
		g_LambdaToWindowsKeyTable[KEY_PAUSE] = 0x14;
		g_LambdaToWindowsKeyTable[KEY_CAPS_LOCK] = 0x14;
		g_LambdaToWindowsKeyTable[KEY_SPACE] = 0x20;
		g_LambdaToWindowsKeyTable[KEY_PAGE_UP] = 0x21;
		g_LambdaToWindowsKeyTable[KEY_PAGE_DOWN] = 0x22;
		g_LambdaToWindowsKeyTable[KEY_END] = 0x23;
		g_LambdaToWindowsKeyTable[KEY_HOME] = 0x24;
		g_LambdaToWindowsKeyTable[KEY_LEFT] = 0x25;
		g_LambdaToWindowsKeyTable[KEY_UP] = 0x26;
		g_LambdaToWindowsKeyTable[KEY_RIGHT] = 0x27;
		g_LambdaToWindowsKeyTable[KEY_DOWN] = 0x28;
		g_LambdaToWindowsKeyTable[KEY_PRINT_SCREEN] = 0x2C;
		g_LambdaToWindowsKeyTable[KEY_INSERT] = 0x2D;
		g_LambdaToWindowsKeyTable[KEY_DELETE] = 0x2E;
		g_LambdaToWindowsKeyTable[KEY_ESCAPE] = 0x1B;

		g_LambdaToWindowsKeyTable[KEY_0] = 0x30;
		g_LambdaToWindowsKeyTable[KEY_1] = 0x31;
		g_LambdaToWindowsKeyTable[KEY_2] = 0x32;
		g_LambdaToWindowsKeyTable[KEY_3] = 0x33;
		g_LambdaToWindowsKeyTable[KEY_4] = 0x34;
		g_LambdaToWindowsKeyTable[KEY_5] = 0x35;
		g_LambdaToWindowsKeyTable[KEY_6] = 0x36;
		g_LambdaToWindowsKeyTable[KEY_7] = 0x37;
		g_LambdaToWindowsKeyTable[KEY_8] = 0x38;
		g_LambdaToWindowsKeyTable[KEY_9] = 0x39;

		g_LambdaToWindowsKeyTable[KEY_A] = 0x41;
		g_LambdaToWindowsKeyTable[KEY_B] = 0x42;
		g_LambdaToWindowsKeyTable[KEY_C] = 0x43;
		g_LambdaToWindowsKeyTable[KEY_D] = 0x44;
		g_LambdaToWindowsKeyTable[KEY_E] = 0x45;
		g_LambdaToWindowsKeyTable[KEY_F] = 0x46;
		g_LambdaToWindowsKeyTable[KEY_G] = 0x47;
		g_LambdaToWindowsKeyTable[KEY_H] = 0x48;
		g_LambdaToWindowsKeyTable[KEY_I] = 0x49;
		g_LambdaToWindowsKeyTable[KEY_J] = 0x4A;
		g_LambdaToWindowsKeyTable[KEY_K] = 0x4B;
		g_LambdaToWindowsKeyTable[KEY_L] = 0x4C;
		g_LambdaToWindowsKeyTable[KEY_M] = 0x4D;
		g_LambdaToWindowsKeyTable[KEY_N] = 0x4E;
		g_LambdaToWindowsKeyTable[KEY_O] = 0x4F;
		g_LambdaToWindowsKeyTable[KEY_P] = 0x50;
		g_LambdaToWindowsKeyTable[KEY_Q] = 0x51;
		g_LambdaToWindowsKeyTable[KEY_R] = 0x52;
		g_LambdaToWindowsKeyTable[KEY_S] = 0x53;
		g_LambdaToWindowsKeyTable[KEY_T] = 0x54;
		g_LambdaToWindowsKeyTable[KEY_U] = 0x55;
		g_LambdaToWindowsKeyTable[KEY_V] = 0x56;
		g_LambdaToWindowsKeyTable[KEY_W] = 0x57;
		g_LambdaToWindowsKeyTable[KEY_X] = 0x58;
		g_LambdaToWindowsKeyTable[KEY_Y] = 0x59;
		g_LambdaToWindowsKeyTable[KEY_Z] = 0x5A;

		g_LambdaToWindowsKeyTable[KEY_KEYPAD_0] = 0x60;
		g_LambdaToWindowsKeyTable[KEY_KEYPAD_1] = 0x61;
		g_LambdaToWindowsKeyTable[KEY_KEYPAD_2] = 0x62;
		g_LambdaToWindowsKeyTable[KEY_KEYPAD_3] = 0x63;
		g_LambdaToWindowsKeyTable[KEY_KEYPAD_4] = 0x64;
		g_LambdaToWindowsKeyTable[KEY_KEYPAD_5] = 0x65;
		g_LambdaToWindowsKeyTable[KEY_KEYPAD_6] = 0x66;
		g_LambdaToWindowsKeyTable[KEY_KEYPAD_7] = 0x67;
		g_LambdaToWindowsKeyTable[KEY_KEYPAD_8] = 0x68;
		g_LambdaToWindowsKeyTable[KEY_KEYPAD_9] = 0x69;
		g_LambdaToWindowsKeyTable[KEY_KEYPAD_MULTIPLY] = 0x6A;
		g_LambdaToWindowsKeyTable[KEY_KEYPAD_ADD] = 0x6B;
		g_LambdaToWindowsKeyTable[KEY_KEYPAD_SUBTRACT] = 0x6D;
		g_LambdaToWindowsKeyTable[KEY_KEYPAD_DECIMAL] = 0x6E;
		g_LambdaToWindowsKeyTable[KEY_KEYPAD_DIVIDE] = 0x6F;

		g_LambdaToWindowsKeyTable[KEY_F1] = 0x70;
		g_LambdaToWindowsKeyTable[KEY_F2] = 0x71;
		g_LambdaToWindowsKeyTable[KEY_F3] = 0x72;
		g_LambdaToWindowsKeyTable[KEY_F4] = 0x73;
		g_LambdaToWindowsKeyTable[KEY_F5] = 0x74;
		g_LambdaToWindowsKeyTable[KEY_F6] = 0x75;
		g_LambdaToWindowsKeyTable[KEY_F7] = 0x76;
		g_LambdaToWindowsKeyTable[KEY_F8] = 0x77;
		g_LambdaToWindowsKeyTable[KEY_F9] = 0x78;
		g_LambdaToWindowsKeyTable[KEY_F10] = 0x79;
		g_LambdaToWindowsKeyTable[KEY_F11] = 0x7A;
		g_LambdaToWindowsKeyTable[KEY_F12] = 0x7B;
		g_LambdaToWindowsKeyTable[KEY_F13] = 0x7C;
		g_LambdaToWindowsKeyTable[KEY_F14] = 0x7D;
		g_LambdaToWindowsKeyTable[KEY_F15] = 0x7E;
		g_LambdaToWindowsKeyTable[KEY_F16] = 0x7F;
		g_LambdaToWindowsKeyTable[KEY_F17] = 0x80;
		g_LambdaToWindowsKeyTable[KEY_F18] = 0x81;
		g_LambdaToWindowsKeyTable[KEY_F19] = 0x82;
		g_LambdaToWindowsKeyTable[KEY_F20] = 0x83;
		g_LambdaToWindowsKeyTable[KEY_F21] = 0x84;
		g_LambdaToWindowsKeyTable[KEY_F22] = 0x85;
		g_LambdaToWindowsKeyTable[KEY_F23] = 0x86;
		g_LambdaToWindowsKeyTable[KEY_F24] = 0x87;

		g_LambdaToWindowsKeyTable[KEY_NUM_LOCK] = 0x90;
		g_LambdaToWindowsKeyTable[KEY_SCROLL_LOCK] = 0x91;

		g_LambdaToWindowsKeyTable[KEY_SEMICOLON] = 0xBA;
		g_LambdaToWindowsKeyTable[KEY_COMMA] = 0xBC;
		g_LambdaToWindowsKeyTable[KEY_MINUS] = 0xBD;
		g_LambdaToWindowsKeyTable[KEY_PERIOD] = 0xBE;
		g_LambdaToWindowsKeyTable[KEY_GRAVE_ACCENT] = 0xC0;
		g_LambdaToWindowsKeyTable[KEY_LEFT_BRACKET] = 0xDB;
		g_LambdaToWindowsKeyTable[KEY_RIGHT_BRACKET] = 0xDD;
		g_LambdaToWindowsKeyTable[KEY_APOSTROPHE] = 0xDE;
		g_LambdaToWindowsKeyTable[KEY_BACKSLASH] = 0xE2;

		g_LambdaToWindowsKeyTable[KEY_LEFT_SHIFT] = 0xA0;
		g_LambdaToWindowsKeyTable[KEY_LEFT_CONTROL] = 0xA2;
		g_LambdaToWindowsKeyTable[KEY_LEFT_ALT] = 0xA4;
		g_LambdaToWindowsKeyTable[KEY_LEFT_SUPER] = 0x5B;
		g_LambdaToWindowsKeyTable[KEY_RIGHT_SHIFT] = 0xA1;
		g_LambdaToWindowsKeyTable[KEY_RIGHT_CONTROL] = 0xA3;
		g_LambdaToWindowsKeyTable[KEY_RIGHT_ALT] = 0xA5;
		g_LambdaToWindowsKeyTable[KEY_RIGHT_SUPER] = 0x5C;
		g_LambdaToWindowsKeyTable[KEY_MENU] = 0x5C;
	}
}