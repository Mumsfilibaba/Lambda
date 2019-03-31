#pragma once

namespace Lambda
{
	typedef char int8;
	typedef short int16;
	typedef int int32;
	typedef long long int64;
	typedef unsigned char uint8;
	typedef unsigned short uint16;
	typedef unsigned int uint32;
	typedef unsigned long long uint64;


	enum Key : uint8
	{
		KEY_UNKNOWN = 0,
		KEY_SPACE = 1,
		KEY_APOSTROPHE = 2,	/* ' */
		KEY_COMMA = 3,	/* , */
		KEY_MINUS = 4,	/* - */
		KEY_PERIOD = 5,	/* . */
		KEY_SLASH = 6,	/* / */
		KEY_0 = 7,
		KEY_1 = 8,
		KEY_2 = 9,
		KEY_3 = 10,
		KEY_4 = 11,
		KEY_5 = 12,
		KEY_6 = 13,
		KEY_7 = 14,
		KEY_8 = 15,
		KEY_9 = 16,
		KEY_SEMICOLON = 17,	/* ; */
		KEY_EQUAL = 18,	/* = */
		KEY_A = 19,
		KEY_B = 20,
		KEY_C = 21,
		KEY_D = 22,
		KEY_E = 23,
		KEY_F = 24,
		KEY_G = 25,
		KEY_H = 26,
		KEY_I = 27,
		KEY_J = 28,
		KEY_K = 29,
		KEY_L = 30,
		KEY_M = 31,
		KEY_N = 32,
		KEY_O = 33,
		KEY_P = 34,
		KEY_Q = 35,
		KEY_R = 36,
		KEY_S = 37,
		KEY_T = 38,
		KEY_U = 39,
		KEY_V = 40,
		KEY_W = 41,
		KEY_X = 42,
		KEY_Y = 43,
		KEY_Z = 44,
		KEY_LEFT_BRACKET = 45,	/* [ */
		KEY_BACKSLASH = 46,	/* \ */
		KEY_RIGHT_BRACKET = 47,	/* ] */
		KEY_GRAVE_ACCENT = 48,	/* ` */
		KEY_WORLD_1 = 49,	/* Non-US #1 */
		KEY_WORLD_2 = 50,	/* Non-US #2 */
		KEY_ESCAPE = 51,
		KEY_ENTER = 52,
		KEY_TAB = 53,
		KEY_BACKSPACE = 54,
		KEY_INSERT = 55,
		KEY_DELETE = 56,
		KEY_RIGHT = 57,
		KEY_LEFT = 58,
		KEY_DOWN = 59,
		KEY_UP = 60,
		KEY_PAGE_UP = 70,
		KEY_PAGE_DOWN = 71,
		KEY_HOME = 72,
		KEY_END = 73,
		KEY_CAPS_LOCK = 74,
		KEY_SCROLL_LOCK = 75,
		KEY_NUM_LOCK = 76,
		KEY_PRINT_SCREEN = 77,
		KEY_PAUSE = 78,
		KEY_F1 = 79,
		KEY_F2 = 80,
		KEY_F3 = 81,
		KEY_F4 = 82,
		KEY_F5 = 83,
		KEY_F6 = 84,
		KEY_F7 = 85,
		KEY_F8 = 86,
		KEY_F9 = 87,
		KEY_F10 = 88,
		KEY_F11 = 89,
		KEY_F12 = 90,
		KEY_F13 = 91,
		KEY_F14 = 92,
		KEY_F15 = 93,
		KEY_F16 = 94,
		KEY_F17 = 95,
		KEY_F18 = 96,
		KEY_F19 = 97,
		KEY_F20 = 98,
		KEY_F21 = 99,
		KEY_F22 = 100,
		KEY_F23 = 101,
		KEY_F24 = 102,
		KEY_F25 = 103,
		KEY_KEYPAD_0 = 104,
		KEY_KEYPAD_1 = 105,
		KEY_KEYPAD_2 = 106,
		KEY_KEYPAD_3 = 107,
		KEY_KEYPAD_4 = 108,
		KEY_KEYPAD_5 = 109,
		KEY_KEYPAD_6 = 110,
		KEY_KEYPAD_7 = 111,
		KEY_KEYPAD_8 = 112,
		KEY_KEYPAD_9 = 113,
		KEY_KEYPAD_DECIMAL = 114,
		KEY_KEYPAD_DIVIDE = 115,
		KEY_KEYPAD_MULTIPLY = 116,
		KEY_KEYPAD_SUBTRACT = 117,
		KEY_KEYPAD_ADD = 118,
		KEY_KEYPAD_ENTER = 119,
		KEY_KEYPAD_EQUAL = 120,
		KEY_LEFT_SHIFT = 121,
		KEY_LEFT_CONTROL = 122,
		KEY_LEFT_ALT = 123,
		KEY_LEFT_SUPER = 124,
		KEY_RIGHT_SHIFT = 125,
		KEY_RIGHT_CONTROL = 126,
		KEY_RIGHT_ALT = 127,
		KEY_RIGHT_SUPER = 128,
		KEY_MENU = 129,
	};


	enum MouseButton : uint8
	{
		MOUSEBUTTON_UNKNOWN = 0,
		MOUSEBUTTON_LEFT = 1,
		MOUSEBUTTON_MIDDLE = 2,
		MOUSEBUTTON_RIGHT = 3,
		MOUSEBUTTON_FORWARD = 4,
		MOUSEBUTTON_BACKWARD = 5,
	};


	enum PrimitiveTopology : uint32
	{
		PRIMITIVE_TOPOLOGY_UNKNOWN = 0,
		PRIMITIVE_TOPOLOGY_TRIANGLELIST = 1,
		PRIMITIVE_TOPOLOGY_TRIANGLESTRIP = 2,
	};


	enum ResourceUsage : uint32
	{
		RESOURCE_USAGE_UNKNOWN = 0,
		RESOURCE_USAGE_DEFAULT = 1,
		RESOURCE_USAGE_DYNAMIC = 2,
	};


	enum BufferFlags : uint32
	{
		BUFFER_FLAGS_NONE = 0,
		BUFFER_FLAGS_VERTEX_BUFFER = (1 << 0),
		BUFFER_FLAGS_INDEX_BUFFER = (1 << 2),
		BUFFER_FLAGS_CONSTANT_BUFFER = (1 << 3),
	};


	struct ResourceData
	{
		const void* pData = nullptr;
		uint32 SizeInBytes = 0;
	};
}