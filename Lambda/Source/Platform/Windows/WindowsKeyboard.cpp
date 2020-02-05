#include "LambdaPch.h"

#if defined(LAMBDA_PLAT_WINDOWS)
	#include "Platform/Windows/WindowsKeyboard.h"

namespace Lambda
{
	uint32 WindowsKeyboard::s_KeyTable[EKey::KEY_LAST + 1];
	EKey   WindowsKeyboard::s_WindowsKeyTable[256];

	uint32 WindowsKeyboard::GetActiveModifierKeys()
	{
		uint32 modifiers = 0;
		if (GetKeyState(VK_SHIFT) & 0x8000)
			modifiers |= EKeyModifier::KEY_MODIFIER_SHIFT;
		if (GetKeyState(VK_CONTROL) & 0x8000)
			modifiers |= EKeyModifier::KEY_MODIFIER_CONTROL;
		if (GetKeyState(VK_MENU) & 0x8000)
			modifiers |= EKeyModifier::KEY_MODIFIER_ALT;
		if ((GetKeyState(VK_LWIN) | GetKeyState(VK_RWIN)) & 0x8000)
			modifiers |= EKeyModifier::KEY_MODIFIER_SUPER;
		if (GetKeyState(VK_CAPITAL) & 1)
			modifiers |= EKeyModifier::KEY_MODIFIER_CAPS_LOCK;
		if (GetKeyState(VK_NUMLOCK) & 1)
			modifiers |= EKeyModifier::KEY_MODIFIER_NUM_LOCK;

		return modifiers;
	}

	void WindowsKeyboard::Init()
	{
		//Set to zero
		memset(s_KeyTable, 0, sizeof(s_KeyTable));
		memset(s_WindowsKeyTable, 0, sizeof(s_WindowsKeyTable));

		s_WindowsKeyTable[0x08] = EKey::KEY_BACKSPACE;
		s_WindowsKeyTable[0x09] = EKey::KEY_TAB;
		s_WindowsKeyTable[0x0D] = EKey::KEY_ENTER;
		s_WindowsKeyTable[0x14] = EKey::KEY_PAUSE;
		s_WindowsKeyTable[0x14] = EKey::KEY_CAPS_LOCK;
		s_WindowsKeyTable[0x20] = EKey::KEY_SPACE;
		s_WindowsKeyTable[0x21] = EKey::KEY_PAGE_UP;
		s_WindowsKeyTable[0x22] = EKey::KEY_PAGE_DOWN;
		s_WindowsKeyTable[0x23] = EKey::KEY_END;
		s_WindowsKeyTable[0x24] = EKey::KEY_HOME;
		s_WindowsKeyTable[0x25] = EKey::KEY_LEFT;
		s_WindowsKeyTable[0x26] = EKey::KEY_UP;
		s_WindowsKeyTable[0x27] = EKey::KEY_RIGHT;
		s_WindowsKeyTable[0x28] = EKey::KEY_DOWN;
		s_WindowsKeyTable[0x2C] = EKey::KEY_PRINT_SCREEN;
		s_WindowsKeyTable[0x2D] = EKey::KEY_INSERT;
		s_WindowsKeyTable[0x2E] = EKey::KEY_DELETE;
		s_WindowsKeyTable[0x1B] = EKey::KEY_ESCAPE;

		s_WindowsKeyTable[0x30] = EKey::KEY_0;
		s_WindowsKeyTable[0x31] = EKey::KEY_1;
		s_WindowsKeyTable[0x32] = EKey::KEY_2;
		s_WindowsKeyTable[0x33] = EKey::KEY_3;
		s_WindowsKeyTable[0x34] = EKey::KEY_4;
		s_WindowsKeyTable[0x35] = EKey::KEY_5;
		s_WindowsKeyTable[0x36] = EKey::KEY_6;
		s_WindowsKeyTable[0x37] = EKey::KEY_7;
		s_WindowsKeyTable[0x38] = EKey::KEY_8;
		s_WindowsKeyTable[0x39] = EKey::KEY_9;

		s_WindowsKeyTable[0x41] = EKey::KEY_A;
		s_WindowsKeyTable[0x42] = EKey::KEY_B;
		s_WindowsKeyTable[0x43] = EKey::KEY_C;
		s_WindowsKeyTable[0x44] = EKey::KEY_D;
		s_WindowsKeyTable[0x45] = EKey::KEY_E;
		s_WindowsKeyTable[0x46] = EKey::KEY_F;
		s_WindowsKeyTable[0x47] = EKey::KEY_G;
		s_WindowsKeyTable[0x48] = EKey::KEY_H;
		s_WindowsKeyTable[0x49] = EKey::KEY_I;
		s_WindowsKeyTable[0x4A] = EKey::KEY_J;
		s_WindowsKeyTable[0x4B] = EKey::KEY_K;
		s_WindowsKeyTable[0x4C] = EKey::KEY_L;
		s_WindowsKeyTable[0x4D] = EKey::KEY_M;
		s_WindowsKeyTable[0x4E] = EKey::KEY_N;
		s_WindowsKeyTable[0x4F] = EKey::KEY_O;
		s_WindowsKeyTable[0x50] = EKey::KEY_P;
		s_WindowsKeyTable[0x51] = EKey::KEY_Q;
		s_WindowsKeyTable[0x52] = EKey::KEY_R;
		s_WindowsKeyTable[0x53] = EKey::KEY_S;
		s_WindowsKeyTable[0x54] = EKey::KEY_T;
		s_WindowsKeyTable[0x55] = EKey::KEY_U;
		s_WindowsKeyTable[0x56] = EKey::KEY_V;
		s_WindowsKeyTable[0x57] = EKey::KEY_W;
		s_WindowsKeyTable[0x58] = EKey::KEY_X;
		s_WindowsKeyTable[0x59] = EKey::KEY_Y;
		s_WindowsKeyTable[0x5A] = EKey::KEY_Z;

		s_WindowsKeyTable[0x60] = EKey::KEY_KEYPAD_0;
		s_WindowsKeyTable[0x61] = EKey::KEY_KEYPAD_1;
		s_WindowsKeyTable[0x62] = EKey::KEY_KEYPAD_2;
		s_WindowsKeyTable[0x63] = EKey::KEY_KEYPAD_3;
		s_WindowsKeyTable[0x64] = EKey::KEY_KEYPAD_4;
		s_WindowsKeyTable[0x65] = EKey::KEY_KEYPAD_5;
		s_WindowsKeyTable[0x66] = EKey::KEY_KEYPAD_6;
		s_WindowsKeyTable[0x67] = EKey::KEY_KEYPAD_7;
		s_WindowsKeyTable[0x68] = EKey::KEY_KEYPAD_8;
		s_WindowsKeyTable[0x69] = EKey::KEY_KEYPAD_9;
		s_WindowsKeyTable[0x6A] = EKey::KEY_KEYPAD_MULTIPLY;
		s_WindowsKeyTable[0x6B] = EKey::KEY_KEYPAD_ADD;
		s_WindowsKeyTable[0x6D] = EKey::KEY_KEYPAD_SUBTRACT;
		s_WindowsKeyTable[0x6E] = EKey::KEY_KEYPAD_DECIMAL;
		s_WindowsKeyTable[0x6F] = EKey::KEY_KEYPAD_DIVIDE;

		s_WindowsKeyTable[0x70] = EKey::KEY_F1;
		s_WindowsKeyTable[0x71] = EKey::KEY_F2;
		s_WindowsKeyTable[0x72] = EKey::KEY_F3;
		s_WindowsKeyTable[0x73] = EKey::KEY_F4;
		s_WindowsKeyTable[0x74] = EKey::KEY_F5;
		s_WindowsKeyTable[0x75] = EKey::KEY_F6;
		s_WindowsKeyTable[0x76] = EKey::KEY_F7;
		s_WindowsKeyTable[0x77] = EKey::KEY_F8;
		s_WindowsKeyTable[0x78] = EKey::KEY_F9;
		s_WindowsKeyTable[0x79] = EKey::KEY_F10;
		s_WindowsKeyTable[0x7A] = EKey::KEY_F11;
		s_WindowsKeyTable[0x7B] = EKey::KEY_F12;
		s_WindowsKeyTable[0x7C] = EKey::KEY_F13;
		s_WindowsKeyTable[0x7D] = EKey::KEY_F14;
		s_WindowsKeyTable[0x7E] = EKey::KEY_F15;
		s_WindowsKeyTable[0x7F] = EKey::KEY_F16;
		s_WindowsKeyTable[0x80] = EKey::KEY_F17;
		s_WindowsKeyTable[0x81] = EKey::KEY_F18;
		s_WindowsKeyTable[0x82] = EKey::KEY_F19;
		s_WindowsKeyTable[0x83] = EKey::KEY_F20;
		s_WindowsKeyTable[0x84] = EKey::KEY_F21;
		s_WindowsKeyTable[0x85] = EKey::KEY_F22;
		s_WindowsKeyTable[0x86] = EKey::KEY_F23;
		s_WindowsKeyTable[0x87] = EKey::KEY_F24;

		s_WindowsKeyTable[0x90] = EKey::KEY_NUM_LOCK;
		s_WindowsKeyTable[0x91] = EKey::KEY_SCROLL_LOCK;

		s_WindowsKeyTable[0xBA] = EKey::KEY_SEMICOLON;
		s_WindowsKeyTable[0xBC] = EKey::KEY_COMMA;
		s_WindowsKeyTable[0xBD] = EKey::KEY_MINUS;
		s_WindowsKeyTable[0xBE] = EKey::KEY_PERIOD;
		s_WindowsKeyTable[0xC0] = EKey::KEY_GRAVE_ACCENT;
		s_WindowsKeyTable[0xDB] = EKey::KEY_LEFT_BRACKET;
		s_WindowsKeyTable[0xDD] = EKey::KEY_RIGHT_BRACKET;
		s_WindowsKeyTable[0xDE] = EKey::KEY_APOSTROPHE;
		s_WindowsKeyTable[0xE2] = EKey::KEY_BACKSLASH;

		s_WindowsKeyTable[0xA0] = EKey::KEY_LEFT_SHIFT;
		s_WindowsKeyTable[0xA2] = EKey::KEY_LEFT_CONTROL;
		s_WindowsKeyTable[0xA4] = EKey::KEY_LEFT_ALT;
		s_WindowsKeyTable[0x5B] = EKey::KEY_LEFT_SUPER;
		s_WindowsKeyTable[0xA1] = EKey::KEY_RIGHT_SHIFT;
		s_WindowsKeyTable[0xA3] = EKey::KEY_RIGHT_CONTROL;
		s_WindowsKeyTable[0xA5] = EKey::KEY_RIGHT_ALT;
		s_WindowsKeyTable[0x5C] = EKey::KEY_RIGHT_SUPER;
		s_WindowsKeyTable[0x5C] = EKey::KEY_MENU;

		/*///////////////////////////////////////////////*/

		s_KeyTable[EKey::KEY_BACKSPACE] = 0x08;
		s_KeyTable[EKey::KEY_TAB] = 0x09;
		s_KeyTable[EKey::KEY_ENTER] = 0x0D;
		s_KeyTable[EKey::KEY_PAUSE] = 0x14;
		s_KeyTable[EKey::KEY_CAPS_LOCK] = 0x14;
		s_KeyTable[EKey::KEY_SPACE] = 0x20;
		s_KeyTable[EKey::KEY_PAGE_UP] = 0x21;
		s_KeyTable[EKey::KEY_PAGE_DOWN] = 0x22;
		s_KeyTable[EKey::KEY_END] = 0x23;
		s_KeyTable[EKey::KEY_HOME] = 0x24;
		s_KeyTable[EKey::KEY_LEFT] = 0x25;
		s_KeyTable[EKey::KEY_UP] = 0x26;
		s_KeyTable[EKey::KEY_RIGHT] = 0x27;
		s_KeyTable[EKey::KEY_DOWN] = 0x28;
		s_KeyTable[EKey::KEY_PRINT_SCREEN] = 0x2C;
		s_KeyTable[EKey::KEY_INSERT] = 0x2D;
		s_KeyTable[EKey::KEY_DELETE] = 0x2E;
		s_KeyTable[EKey::KEY_ESCAPE] = 0x1B;

		s_KeyTable[EKey::KEY_0] = 0x30;
		s_KeyTable[EKey::KEY_1] = 0x31;
		s_KeyTable[EKey::KEY_2] = 0x32;
		s_KeyTable[EKey::KEY_3] = 0x33;
		s_KeyTable[EKey::KEY_4] = 0x34;
		s_KeyTable[EKey::KEY_5] = 0x35;
		s_KeyTable[EKey::KEY_6] = 0x36;
		s_KeyTable[EKey::KEY_7] = 0x37;
		s_KeyTable[EKey::KEY_8] = 0x38;
		s_KeyTable[EKey::KEY_9] = 0x39;

		s_KeyTable[EKey::KEY_A] = 0x41;
		s_KeyTable[EKey::KEY_B] = 0x42;
		s_KeyTable[EKey::KEY_C] = 0x43;
		s_KeyTable[EKey::KEY_D] = 0x44;
		s_KeyTable[EKey::KEY_E] = 0x45;
		s_KeyTable[EKey::KEY_F] = 0x46;
		s_KeyTable[EKey::KEY_G] = 0x47;
		s_KeyTable[EKey::KEY_H] = 0x48;
		s_KeyTable[EKey::KEY_I] = 0x49;
		s_KeyTable[EKey::KEY_J] = 0x4A;
		s_KeyTable[EKey::KEY_K] = 0x4B;
		s_KeyTable[EKey::KEY_L] = 0x4C;
		s_KeyTable[EKey::KEY_M] = 0x4D;
		s_KeyTable[EKey::KEY_N] = 0x4E;
		s_KeyTable[EKey::KEY_O] = 0x4F;
		s_KeyTable[EKey::KEY_P] = 0x50;
		s_KeyTable[EKey::KEY_Q] = 0x51;
		s_KeyTable[EKey::KEY_R] = 0x52;
		s_KeyTable[EKey::KEY_S] = 0x53;
		s_KeyTable[EKey::KEY_T] = 0x54;
		s_KeyTable[EKey::KEY_U] = 0x55;
		s_KeyTable[EKey::KEY_V] = 0x56;
		s_KeyTable[EKey::KEY_W] = 0x57;
		s_KeyTable[EKey::KEY_X] = 0x58;
		s_KeyTable[EKey::KEY_Y] = 0x59;
		s_KeyTable[EKey::KEY_Z] = 0x5A;

		s_KeyTable[EKey::KEY_KEYPAD_0] = 0x60;
		s_KeyTable[EKey::KEY_KEYPAD_1] = 0x61;
		s_KeyTable[EKey::KEY_KEYPAD_2] = 0x62;
		s_KeyTable[EKey::KEY_KEYPAD_3] = 0x63;
		s_KeyTable[EKey::KEY_KEYPAD_4] = 0x64;
		s_KeyTable[EKey::KEY_KEYPAD_5] = 0x65;
		s_KeyTable[EKey::KEY_KEYPAD_6] = 0x66;
		s_KeyTable[EKey::KEY_KEYPAD_7] = 0x67;
		s_KeyTable[EKey::KEY_KEYPAD_8] = 0x68;
		s_KeyTable[EKey::KEY_KEYPAD_9] = 0x69;
		s_KeyTable[EKey::KEY_KEYPAD_MULTIPLY] = 0x6A;
		s_KeyTable[EKey::KEY_KEYPAD_ADD] = 0x6B;
		s_KeyTable[EKey::KEY_KEYPAD_SUBTRACT] = 0x6D;
		s_KeyTable[EKey::KEY_KEYPAD_DECIMAL] = 0x6E;
		s_KeyTable[EKey::KEY_KEYPAD_DIVIDE] = 0x6F;

		s_KeyTable[EKey::KEY_F1] = 0x70;
		s_KeyTable[EKey::KEY_F2] = 0x71;
		s_KeyTable[EKey::KEY_F3] = 0x72;
		s_KeyTable[EKey::KEY_F4] = 0x73;
		s_KeyTable[EKey::KEY_F5] = 0x74;
		s_KeyTable[EKey::KEY_F6] = 0x75;
		s_KeyTable[EKey::KEY_F7] = 0x76;
		s_KeyTable[EKey::KEY_F8] = 0x77;
		s_KeyTable[EKey::KEY_F9] = 0x78;
		s_KeyTable[EKey::KEY_F10] = 0x79;
		s_KeyTable[EKey::KEY_F11] = 0x7A;
		s_KeyTable[EKey::KEY_F12] = 0x7B;
		s_KeyTable[EKey::KEY_F13] = 0x7C;
		s_KeyTable[EKey::KEY_F14] = 0x7D;
		s_KeyTable[EKey::KEY_F15] = 0x7E;
		s_KeyTable[EKey::KEY_F16] = 0x7F;
		s_KeyTable[EKey::KEY_F17] = 0x80;
		s_KeyTable[EKey::KEY_F18] = 0x81;
		s_KeyTable[EKey::KEY_F19] = 0x82;
		s_KeyTable[EKey::KEY_F20] = 0x83;
		s_KeyTable[EKey::KEY_F21] = 0x84;
		s_KeyTable[EKey::KEY_F22] = 0x85;
		s_KeyTable[EKey::KEY_F23] = 0x86;
		s_KeyTable[EKey::KEY_F24] = 0x87;

		s_KeyTable[EKey::KEY_NUM_LOCK] = 0x90;
		s_KeyTable[EKey::KEY_SCROLL_LOCK] = 0x91;

		s_KeyTable[EKey::KEY_SEMICOLON] = 0xBA;
		s_KeyTable[EKey::KEY_COMMA] = 0xBC;
		s_KeyTable[EKey::KEY_MINUS] = 0xBD;
		s_KeyTable[EKey::KEY_PERIOD] = 0xBE;
		s_KeyTable[EKey::KEY_GRAVE_ACCENT] = 0xC0;
		s_KeyTable[EKey::KEY_LEFT_BRACKET] = 0xDB;
		s_KeyTable[EKey::KEY_RIGHT_BRACKET] = 0xDD;
		s_KeyTable[EKey::KEY_APOSTROPHE] = 0xDE;
		s_KeyTable[EKey::KEY_BACKSLASH] = 0xE2;

		s_KeyTable[EKey::KEY_LEFT_SHIFT] = 0xA0;
		s_KeyTable[EKey::KEY_LEFT_CONTROL] = 0xA2;
		s_KeyTable[EKey::KEY_LEFT_ALT] = 0xA4;
		s_KeyTable[EKey::KEY_LEFT_SUPER] = 0x5B;
		s_KeyTable[EKey::KEY_RIGHT_SHIFT] = 0xA1;
		s_KeyTable[EKey::KEY_RIGHT_CONTROL] = 0xA3;
		s_KeyTable[EKey::KEY_RIGHT_ALT] = 0xA5;
		s_KeyTable[EKey::KEY_RIGHT_SUPER] = 0x5C;
		s_KeyTable[EKey::KEY_MENU] = 0x5C;
	}
}
#endif
