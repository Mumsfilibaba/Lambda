#include "LambdaPch.h"

#if defined(LAMBDA_PLAT_WINDOWS)
#include "WindowsInput.h"
#include "WindowsWindow.h"

namespace Lambda
{
	//-----
	//Input
	//-----

	Input* Input::Create()
	{
		return DBG_NEW WindowsInput();
	}

	//------------
	//WindowsInput
	//------------

	WindowsInput::WindowsInput()
	{
		//Set to zero
		memset(m_WindowsToLambdaKeyTable, 0, sizeof(m_WindowsToLambdaKeyTable));
		memset(m_LambdaToWindowsKeyTable, 0, sizeof(m_LambdaToWindowsKeyTable));

		m_WindowsToLambdaKeyTable[0x08]					= KEY_BACKSPACE;
		m_WindowsToLambdaKeyTable[0x09]					= KEY_TAB;
		m_WindowsToLambdaKeyTable[0x0D]					= KEY_ENTER;
		m_WindowsToLambdaKeyTable[0x14]					= KEY_PAUSE;
		m_WindowsToLambdaKeyTable[0x14]					= KEY_CAPS_LOCK;
		m_WindowsToLambdaKeyTable[0x20]					= KEY_SPACE;
		m_WindowsToLambdaKeyTable[0x21]					= KEY_PAGE_UP;
		m_WindowsToLambdaKeyTable[0x22]					= KEY_PAGE_DOWN;
		m_WindowsToLambdaKeyTable[0x23]					= KEY_END;
		m_WindowsToLambdaKeyTable[0x24]					= KEY_HOME;
		m_WindowsToLambdaKeyTable[0x25]					= KEY_LEFT;
		m_WindowsToLambdaKeyTable[0x26]					= KEY_UP;
		m_WindowsToLambdaKeyTable[0x27]					= KEY_RIGHT;
		m_WindowsToLambdaKeyTable[0x28]					= KEY_DOWN;
		m_WindowsToLambdaKeyTable[0x2C]					= KEY_PRINT_SCREEN;
		m_WindowsToLambdaKeyTable[0x2D]					= KEY_INSERT;
		m_WindowsToLambdaKeyTable[0x2E]					= KEY_DELETE;
		m_WindowsToLambdaKeyTable[0x1B]					= KEY_ESCAPE;

		m_WindowsToLambdaKeyTable[0x30]					= KEY_0;
		m_WindowsToLambdaKeyTable[0x31]					= KEY_1;
		m_WindowsToLambdaKeyTable[0x32]					= KEY_2;
		m_WindowsToLambdaKeyTable[0x33]					= KEY_3;
		m_WindowsToLambdaKeyTable[0x34]					= KEY_4;
		m_WindowsToLambdaKeyTable[0x35]					= KEY_5;
		m_WindowsToLambdaKeyTable[0x36]					= KEY_6;
		m_WindowsToLambdaKeyTable[0x37]					= KEY_7;
		m_WindowsToLambdaKeyTable[0x38]					= KEY_8;
		m_WindowsToLambdaKeyTable[0x39]					= KEY_9;

		m_WindowsToLambdaKeyTable[0x41]					= KEY_A;
		m_WindowsToLambdaKeyTable[0x42]					= KEY_B;
		m_WindowsToLambdaKeyTable[0x43]					= KEY_C;
		m_WindowsToLambdaKeyTable[0x44]					= KEY_D;
		m_WindowsToLambdaKeyTable[0x45]					= KEY_E;
		m_WindowsToLambdaKeyTable[0x46]					= KEY_F;
		m_WindowsToLambdaKeyTable[0x47]					= KEY_G;
		m_WindowsToLambdaKeyTable[0x48]					= KEY_H;
		m_WindowsToLambdaKeyTable[0x49]					= KEY_I;
		m_WindowsToLambdaKeyTable[0x4A]					= KEY_J;
		m_WindowsToLambdaKeyTable[0x4B]					= KEY_K;
		m_WindowsToLambdaKeyTable[0x4C]					= KEY_L;
		m_WindowsToLambdaKeyTable[0x4D]					= KEY_M;
		m_WindowsToLambdaKeyTable[0x4E]					= KEY_N;
		m_WindowsToLambdaKeyTable[0x4F]					= KEY_O;
		m_WindowsToLambdaKeyTable[0x50]					= KEY_P;
		m_WindowsToLambdaKeyTable[0x51]					= KEY_Q;
		m_WindowsToLambdaKeyTable[0x52]					= KEY_R;
		m_WindowsToLambdaKeyTable[0x53]					= KEY_S;
		m_WindowsToLambdaKeyTable[0x54]					= KEY_T;
		m_WindowsToLambdaKeyTable[0x55]					= KEY_U;
		m_WindowsToLambdaKeyTable[0x56]					= KEY_V;
		m_WindowsToLambdaKeyTable[0x57]					= KEY_W;
		m_WindowsToLambdaKeyTable[0x58]					= KEY_X;
		m_WindowsToLambdaKeyTable[0x59]					= KEY_Y;
		m_WindowsToLambdaKeyTable[0x5A]					= KEY_Z;

		m_WindowsToLambdaKeyTable[0x60]					= KEY_KEYPAD_0;
		m_WindowsToLambdaKeyTable[0x61]					= KEY_KEYPAD_1;
		m_WindowsToLambdaKeyTable[0x62]					= KEY_KEYPAD_2;
		m_WindowsToLambdaKeyTable[0x63]					= KEY_KEYPAD_3;
		m_WindowsToLambdaKeyTable[0x64]					= KEY_KEYPAD_4;
		m_WindowsToLambdaKeyTable[0x65]					= KEY_KEYPAD_5;
		m_WindowsToLambdaKeyTable[0x66]					= KEY_KEYPAD_6;
		m_WindowsToLambdaKeyTable[0x67]					= KEY_KEYPAD_7;
		m_WindowsToLambdaKeyTable[0x68]					= KEY_KEYPAD_8;
		m_WindowsToLambdaKeyTable[0x69]					= KEY_KEYPAD_9;
		m_WindowsToLambdaKeyTable[0x6A]					= KEY_KEYPAD_MULTIPLY;
		m_WindowsToLambdaKeyTable[0x6B]					= KEY_KEYPAD_ADD;
		m_WindowsToLambdaKeyTable[0x6D]					= KEY_KEYPAD_SUBTRACT;
		m_WindowsToLambdaKeyTable[0x6E]					= KEY_KEYPAD_DECIMAL;
		m_WindowsToLambdaKeyTable[0x6F]					= KEY_KEYPAD_DIVIDE;

		m_WindowsToLambdaKeyTable[0x70]					= KEY_F1;
		m_WindowsToLambdaKeyTable[0x71]					= KEY_F2;
		m_WindowsToLambdaKeyTable[0x72]					= KEY_F3;
		m_WindowsToLambdaKeyTable[0x73]					= KEY_F4;
		m_WindowsToLambdaKeyTable[0x74]					= KEY_F5;
		m_WindowsToLambdaKeyTable[0x75]					= KEY_F6;
		m_WindowsToLambdaKeyTable[0x76]					= KEY_F7;
		m_WindowsToLambdaKeyTable[0x77]					= KEY_F8;
		m_WindowsToLambdaKeyTable[0x78]					= KEY_F9;
		m_WindowsToLambdaKeyTable[0x79]					= KEY_F10;
		m_WindowsToLambdaKeyTable[0x7A]					= KEY_F11;
		m_WindowsToLambdaKeyTable[0x7B]					= KEY_F12;
		m_WindowsToLambdaKeyTable[0x7C]					= KEY_F13;
		m_WindowsToLambdaKeyTable[0x7D]					= KEY_F14;
		m_WindowsToLambdaKeyTable[0x7E]					= KEY_F15;
		m_WindowsToLambdaKeyTable[0x7F]					= KEY_F16;
		m_WindowsToLambdaKeyTable[0x80]					= KEY_F17;
		m_WindowsToLambdaKeyTable[0x81]					= KEY_F18;
		m_WindowsToLambdaKeyTable[0x82]					= KEY_F19;
		m_WindowsToLambdaKeyTable[0x83]					= KEY_F20;
		m_WindowsToLambdaKeyTable[0x84]					= KEY_F21;
		m_WindowsToLambdaKeyTable[0x85]					= KEY_F22;
		m_WindowsToLambdaKeyTable[0x86]					= KEY_F23;
		m_WindowsToLambdaKeyTable[0x87]					= KEY_F24;

		m_WindowsToLambdaKeyTable[0x90]					= KEY_NUM_LOCK;
		m_WindowsToLambdaKeyTable[0x91]					= KEY_SCROLL_LOCK;

		m_WindowsToLambdaKeyTable[0xBA]					= KEY_SEMICOLON;
		m_WindowsToLambdaKeyTable[0xBC]					= KEY_COMMA;
		m_WindowsToLambdaKeyTable[0xBD]					= KEY_MINUS;
		m_WindowsToLambdaKeyTable[0xBE]					= KEY_PERIOD;
		m_WindowsToLambdaKeyTable[0xC0]					= KEY_GRAVE_ACCENT;
		m_WindowsToLambdaKeyTable[0xDB]					= KEY_LEFT_BRACKET;
		m_WindowsToLambdaKeyTable[0xDD]					= KEY_RIGHT_BRACKET;
		m_WindowsToLambdaKeyTable[0xDE]					= KEY_APOSTROPHE;
		m_WindowsToLambdaKeyTable[0xE2]					= KEY_BACKSLASH;

		m_WindowsToLambdaKeyTable[0xA0]					= KEY_LEFT_SHIFT;
		m_WindowsToLambdaKeyTable[0xA2]					= KEY_LEFT_CONTROL;
		m_WindowsToLambdaKeyTable[0xA4]					= KEY_LEFT_ALT;
		m_WindowsToLambdaKeyTable[0x5B]					= KEY_LEFT_SUPER;
		m_WindowsToLambdaKeyTable[0xA1]					= KEY_RIGHT_SHIFT;
		m_WindowsToLambdaKeyTable[0xA3]					= KEY_RIGHT_CONTROL;
		m_WindowsToLambdaKeyTable[0xA5]					= KEY_RIGHT_ALT;
		m_WindowsToLambdaKeyTable[0x5C]					= KEY_RIGHT_SUPER;
		m_WindowsToLambdaKeyTable[0x5C]					= KEY_MENU;

		///////////////////////////////////////////////////

		m_LambdaToWindowsKeyTable[KEY_BACKSPACE]		= 0x08;
		m_LambdaToWindowsKeyTable[KEY_TAB]				= 0x09;
		m_LambdaToWindowsKeyTable[KEY_ENTER]			= 0x0D;
		m_LambdaToWindowsKeyTable[KEY_PAUSE]			= 0x14;
		m_LambdaToWindowsKeyTable[KEY_CAPS_LOCK]		= 0x14;
		m_LambdaToWindowsKeyTable[KEY_SPACE]			= 0x20;
		m_LambdaToWindowsKeyTable[KEY_PAGE_UP]			= 0x21;
		m_LambdaToWindowsKeyTable[KEY_PAGE_DOWN]		= 0x22;
		m_LambdaToWindowsKeyTable[KEY_END]				= 0x23;
		m_LambdaToWindowsKeyTable[KEY_HOME]				= 0x24;
		m_LambdaToWindowsKeyTable[KEY_LEFT]				= 0x25;
		m_LambdaToWindowsKeyTable[KEY_UP]				= 0x26;
		m_LambdaToWindowsKeyTable[KEY_RIGHT]			= 0x27;
		m_LambdaToWindowsKeyTable[KEY_DOWN]				= 0x28;
		m_LambdaToWindowsKeyTable[KEY_PRINT_SCREEN]		= 0x2C;
		m_LambdaToWindowsKeyTable[KEY_INSERT]			= 0x2D;
		m_LambdaToWindowsKeyTable[KEY_DELETE]			= 0x2E;
		m_LambdaToWindowsKeyTable[KEY_ESCAPE]			= 0x1B;

		m_LambdaToWindowsKeyTable[KEY_0]				= 0x30;
		m_LambdaToWindowsKeyTable[KEY_1]				= 0x31;
		m_LambdaToWindowsKeyTable[KEY_2]				= 0x32;
		m_LambdaToWindowsKeyTable[KEY_3]				= 0x33;
		m_LambdaToWindowsKeyTable[KEY_4]				= 0x34;
		m_LambdaToWindowsKeyTable[KEY_5]				= 0x35;
		m_LambdaToWindowsKeyTable[KEY_6]				= 0x36;
		m_LambdaToWindowsKeyTable[KEY_7]				= 0x37;
		m_LambdaToWindowsKeyTable[KEY_8]				= 0x38;
		m_LambdaToWindowsKeyTable[KEY_9]				= 0x39;

		m_LambdaToWindowsKeyTable[KEY_A]				= 0x41;
		m_LambdaToWindowsKeyTable[KEY_B]				= 0x42;
		m_LambdaToWindowsKeyTable[KEY_C]				= 0x43;
		m_LambdaToWindowsKeyTable[KEY_D]				= 0x44;
		m_LambdaToWindowsKeyTable[KEY_E]				= 0x45;
		m_LambdaToWindowsKeyTable[KEY_F]				= 0x46;
		m_LambdaToWindowsKeyTable[KEY_G]				= 0x47;
		m_LambdaToWindowsKeyTable[KEY_H]				= 0x48;
		m_LambdaToWindowsKeyTable[KEY_I]				= 0x49;
		m_LambdaToWindowsKeyTable[KEY_J]				= 0x4A;
		m_LambdaToWindowsKeyTable[KEY_K]				= 0x4B;
		m_LambdaToWindowsKeyTable[KEY_L]				= 0x4C;
		m_LambdaToWindowsKeyTable[KEY_M]				= 0x4D;
		m_LambdaToWindowsKeyTable[KEY_N]				= 0x4E;
		m_LambdaToWindowsKeyTable[KEY_O]				= 0x4F;
		m_LambdaToWindowsKeyTable[KEY_P]				= 0x50;
		m_LambdaToWindowsKeyTable[KEY_Q]				= 0x51;
		m_LambdaToWindowsKeyTable[KEY_R]				= 0x52;
		m_LambdaToWindowsKeyTable[KEY_S]				= 0x53;
		m_LambdaToWindowsKeyTable[KEY_T]				= 0x54;
		m_LambdaToWindowsKeyTable[KEY_U]				= 0x55;
		m_LambdaToWindowsKeyTable[KEY_V]				= 0x56;
		m_LambdaToWindowsKeyTable[KEY_W]				= 0x57;
		m_LambdaToWindowsKeyTable[KEY_X]				= 0x58;
		m_LambdaToWindowsKeyTable[KEY_Y]				= 0x59;
		m_LambdaToWindowsKeyTable[KEY_Z]				= 0x5A;

		m_LambdaToWindowsKeyTable[KEY_KEYPAD_0]			= 0x60;
		m_LambdaToWindowsKeyTable[KEY_KEYPAD_1]			= 0x61;
		m_LambdaToWindowsKeyTable[KEY_KEYPAD_2]			= 0x62;
		m_LambdaToWindowsKeyTable[KEY_KEYPAD_3]			= 0x63;
		m_LambdaToWindowsKeyTable[KEY_KEYPAD_4]			= 0x64;
		m_LambdaToWindowsKeyTable[KEY_KEYPAD_5]			= 0x65;
		m_LambdaToWindowsKeyTable[KEY_KEYPAD_6]			= 0x66;
		m_LambdaToWindowsKeyTable[KEY_KEYPAD_7]			= 0x67;
		m_LambdaToWindowsKeyTable[KEY_KEYPAD_8]			= 0x68;
		m_LambdaToWindowsKeyTable[KEY_KEYPAD_9]			= 0x69;
		m_LambdaToWindowsKeyTable[KEY_KEYPAD_MULTIPLY]	= 0x6A;
		m_LambdaToWindowsKeyTable[KEY_KEYPAD_ADD]		= 0x6B;
		m_LambdaToWindowsKeyTable[KEY_KEYPAD_SUBTRACT]	= 0x6D;
		m_LambdaToWindowsKeyTable[KEY_KEYPAD_DECIMAL]	= 0x6E;
		m_LambdaToWindowsKeyTable[KEY_KEYPAD_DIVIDE]	= 0x6F;

		m_LambdaToWindowsKeyTable[KEY_F1]				= 0x70;
		m_LambdaToWindowsKeyTable[KEY_F2]				= 0x71;
		m_LambdaToWindowsKeyTable[KEY_F3]				= 0x72;
		m_LambdaToWindowsKeyTable[KEY_F4]				= 0x73;
		m_LambdaToWindowsKeyTable[KEY_F5]				= 0x74;
		m_LambdaToWindowsKeyTable[KEY_F6]				= 0x75;
		m_LambdaToWindowsKeyTable[KEY_F7]				= 0x76;
		m_LambdaToWindowsKeyTable[KEY_F8]				= 0x77;
		m_LambdaToWindowsKeyTable[KEY_F9]				= 0x78;
		m_LambdaToWindowsKeyTable[KEY_F10]				= 0x79;
		m_LambdaToWindowsKeyTable[KEY_F11]				= 0x7A;
		m_LambdaToWindowsKeyTable[KEY_F12]				= 0x7B;
		m_LambdaToWindowsKeyTable[KEY_F13]				= 0x7C;
		m_LambdaToWindowsKeyTable[KEY_F14]				= 0x7D;
		m_LambdaToWindowsKeyTable[KEY_F15]				= 0x7E;
		m_LambdaToWindowsKeyTable[KEY_F16]				= 0x7F;
		m_LambdaToWindowsKeyTable[KEY_F17]				= 0x80;
		m_LambdaToWindowsKeyTable[KEY_F18]				= 0x81;
		m_LambdaToWindowsKeyTable[KEY_F19]				= 0x82;
		m_LambdaToWindowsKeyTable[KEY_F20]				= 0x83;
		m_LambdaToWindowsKeyTable[KEY_F21]				= 0x84;
		m_LambdaToWindowsKeyTable[KEY_F22]				= 0x85;
		m_LambdaToWindowsKeyTable[KEY_F23]				= 0x86;
		m_LambdaToWindowsKeyTable[KEY_F24]				= 0x87;

		m_LambdaToWindowsKeyTable[KEY_NUM_LOCK]			= 0x90;
		m_LambdaToWindowsKeyTable[KEY_SCROLL_LOCK]		= 0x91;

		m_LambdaToWindowsKeyTable[KEY_SEMICOLON]		= 0xBA;
		m_LambdaToWindowsKeyTable[KEY_COMMA]			= 0xBC;
		m_LambdaToWindowsKeyTable[KEY_MINUS]			= 0xBD;
		m_LambdaToWindowsKeyTable[KEY_PERIOD]			= 0xBE;
		m_LambdaToWindowsKeyTable[KEY_GRAVE_ACCENT]		= 0xC0;
		m_LambdaToWindowsKeyTable[KEY_LEFT_BRACKET]		= 0xDB;
		m_LambdaToWindowsKeyTable[KEY_RIGHT_BRACKET]	= 0xDD;
		m_LambdaToWindowsKeyTable[KEY_APOSTROPHE]		= 0xDE;
		m_LambdaToWindowsKeyTable[KEY_BACKSLASH]		= 0xE2;

		m_LambdaToWindowsKeyTable[KEY_LEFT_SHIFT]		= 0xA0;
		m_LambdaToWindowsKeyTable[KEY_LEFT_CONTROL]		= 0xA2;
		m_LambdaToWindowsKeyTable[KEY_LEFT_ALT]			= 0xA4;
		m_LambdaToWindowsKeyTable[KEY_LEFT_SUPER]		= 0x5B;
		m_LambdaToWindowsKeyTable[KEY_RIGHT_SHIFT]		= 0xA1;
		m_LambdaToWindowsKeyTable[KEY_RIGHT_CONTROL]	= 0xA3;
		m_LambdaToWindowsKeyTable[KEY_RIGHT_ALT]		= 0xA5;
		m_LambdaToWindowsKeyTable[KEY_RIGHT_SUPER]		= 0x5C;
		m_LambdaToWindowsKeyTable[KEY_MENU]				= 0x5C;
	}

    
	Key WindowsInput::WindowsKeyToLambda(uint32 keycode) const
	{
		return m_WindowsToLambdaKeyTable[keycode];
	}

    
	uint32 WindowsInput::LambdaKeyToWindows(Key keycode) const
	{
		return m_LambdaToWindowsKeyTable[keycode];
	}

    
	MouseButton WindowsInput::WindowsButtonToLambda(uint32 keycode) const
	{
		switch(keycode)
		{
		case MK_LBUTTON: return MOUSEBUTTON_LEFT;
		case MK_MBUTTON: return MOUSEBUTTON_MIDDLE;
		case MK_RBUTTON: return MOUSEBUTTON_RIGHT;
		case MK_XBUTTON1: return MOUSEBUTTON_FORWARD;
		case MK_XBUTTON2: return MOUSEBUTTON_BACKWARD;
		default: return MOUSEBUTTON_UNKNOWN;
		}
	}

    
	uint32 WindowsInput::LambdaButtonToWindows(MouseButton keycode) const
	{
		switch (keycode)
		{
		case MOUSEBUTTON_LEFT: return MK_LBUTTON;
		case MOUSEBUTTON_MIDDLE: return MK_MBUTTON;
		case MOUSEBUTTON_RIGHT: return MK_RBUTTON;
		case MOUSEBUTTON_FORWARD: return MK_XBUTTON1;
		case MOUSEBUTTON_BACKWARD: return MK_XBUTTON2;
		default: return 0;
		}
	}

    
	bool WindowsInput::InternalIsKeyUp(Key keycode) const
	{
		return !(GetAsyncKeyState(LambdaKeyToWindows(keycode)) & 0x8000);
	}

    
	bool WindowsInput::InternalIsKeyDown(Key keycode) const
	{
		return GetAsyncKeyState(LambdaKeyToWindows(keycode)) & 0x8000;
	}

    
	void WindowsInput::InternalSetMousePosition(float x, float y) const
	{
		SetCursorPos(int(x), int(y));
	}

    
	void WindowsInput::InternalGetMousePosition(float& x, float& y) const
	{
		POINT p = {};
		GetCursorPos(&p);

		x = float(p.x);
		y = float(p.y);
	}
    

	void WindowsInput::InternalSetMousePosition(IWindow* pRelativeTo, float x, float y) const
	{
		if (!pRelativeTo)
		{
			return;
		}

		POINT p = { LONG(x), LONG(y) };
		ClientToScreen((HWND)pRelativeTo->GetNativeHandle(), &p);

		SetCursorPos(p.x, p.y);
	}
    

	void WindowsInput::InternalGetMousePosition(IWindow* pRelativeTo, float& x, float& y) const
	{
		LAMBDA_ASSERT(pRelativeTo != nullptr);

		POINT p = {};
		GetCursorPos(&p);

		ScreenToClient((HWND)pRelativeTo->GetNativeHandle(), &p);
		x = float(p.x);
		y = float(p.y);
	}
}
#endif
