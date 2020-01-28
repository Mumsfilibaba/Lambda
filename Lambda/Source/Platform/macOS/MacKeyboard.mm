#include "LambdaPch.h"

#if defined(LAMBDA_PLAT_MACOS)
    #include "Platform/macOS/MacKeyboard.h"

    #include <Cocoa/Cocoa.h>

namespace Lambda
{
    EKey   CMacKeyboard::s_MacKeyTable[256];
    uint32 CMacKeyboard::s_KeyTable[EKey::KEY_LAST + 1];
    
    void CMacKeyboard::Init()
    {
        //Init to zero
        memset(s_MacKeyTable, 0, sizeof(s_MacKeyTable));
        memset(s_KeyTable, 0, sizeof(s_KeyTable));
        
        /*////////////////////////////////////////////////////////////////////////////////////////////////*/
        //Convert from macOS-Keycode
        /*////////////////////////////////////////////////////////////////////////////////////////////////*/
        
        s_MacKeyTable[0x33] = EKey::KEY_BACKSPACE;
        s_MacKeyTable[0x30] = EKey::KEY_TAB;
        s_MacKeyTable[0x24] = EKey::KEY_ENTER;
        s_MacKeyTable[0x39] = EKey::KEY_CAPS_LOCK;
        s_MacKeyTable[0x31] = EKey::KEY_SPACE;
        s_MacKeyTable[0x74] = EKey::KEY_PAGE_UP;
        s_MacKeyTable[0x79] = EKey::KEY_PAGE_DOWN;
        s_MacKeyTable[0x77] = EKey::KEY_END;
        s_MacKeyTable[0x73] = EKey::KEY_HOME;
        s_MacKeyTable[0x7B] = EKey::KEY_LEFT;
        s_MacKeyTable[0x7E] = EKey::KEY_UP;
        s_MacKeyTable[0x7C] = EKey::KEY_RIGHT;
        s_MacKeyTable[0x7D] = EKey::KEY_DOWN;
        s_MacKeyTable[0x72] = EKey::KEY_INSERT;
        s_MacKeyTable[0x75] = EKey::KEY_DELETE;
        s_MacKeyTable[0x35] = EKey::KEY_ESCAPE;
        
        s_MacKeyTable[0x1D] = EKey::KEY_0;
        s_MacKeyTable[0x12] = EKey::KEY_1;
        s_MacKeyTable[0x13] = EKey::KEY_2;
        s_MacKeyTable[0x14] = EKey::KEY_3;
        s_MacKeyTable[0x15] = EKey::KEY_4;
        s_MacKeyTable[0x17] = EKey::KEY_5;
        s_MacKeyTable[0x16] = EKey::KEY_6;
        s_MacKeyTable[0x1A] = EKey::KEY_7;
        s_MacKeyTable[0x1C] = EKey::KEY_8;
        s_MacKeyTable[0x19] = EKey::KEY_9;
        
        s_MacKeyTable[0x00] = EKey::KEY_A;
        s_MacKeyTable[0x0B] = EKey::KEY_B;
        s_MacKeyTable[0x08] = EKey::KEY_C;
        s_MacKeyTable[0x02] = EKey::KEY_D;
        s_MacKeyTable[0x0E] = EKey::KEY_E;
        s_MacKeyTable[0x03] = EKey::KEY_F;
        s_MacKeyTable[0x05] = EKey::KEY_G;
        s_MacKeyTable[0x04] = EKey::KEY_H;
        s_MacKeyTable[0x22] = EKey::KEY_I;
        s_MacKeyTable[0x26] = EKey::KEY_J;
        s_MacKeyTable[0x28] = EKey::KEY_K;
        s_MacKeyTable[0x25] = EKey::KEY_L;
        s_MacKeyTable[0x2E] = EKey::KEY_M;
        s_MacKeyTable[0x2D] = EKey::KEY_N;
        s_MacKeyTable[0x1F] = EKey::KEY_O;
        s_MacKeyTable[0x23] = EKey::KEY_P;
        s_MacKeyTable[0x0C] = EKey::KEY_Q;
        s_MacKeyTable[0x0F] = EKey::KEY_R;
        s_MacKeyTable[0x01] = EKey::KEY_S;
        s_MacKeyTable[0x11] = EKey::KEY_T;
        s_MacKeyTable[0x20] = EKey::KEY_U;
        s_MacKeyTable[0x09] = EKey::KEY_V;
        s_MacKeyTable[0x0D] = EKey::KEY_W;
        s_MacKeyTable[0x07] = EKey::KEY_X;
        s_MacKeyTable[0x10] = EKey::KEY_Y;
        s_MacKeyTable[0x06] = EKey::KEY_Z;
        
        s_MacKeyTable[0x52] = EKey::KEY_KEYPAD_0;
        s_MacKeyTable[0x53] = EKey::KEY_KEYPAD_1;
        s_MacKeyTable[0x54] = EKey::KEY_KEYPAD_2;
        s_MacKeyTable[0x55] = EKey::KEY_KEYPAD_3;
        s_MacKeyTable[0x56] = EKey::KEY_KEYPAD_4;
        s_MacKeyTable[0x57] = EKey::KEY_KEYPAD_5;
        s_MacKeyTable[0x58] = EKey::KEY_KEYPAD_6;
        s_MacKeyTable[0x59] = EKey::KEY_KEYPAD_7;
        s_MacKeyTable[0x5B] = EKey::KEY_KEYPAD_8;
        s_MacKeyTable[0x5C] = EKey::KEY_KEYPAD_9;
        s_MacKeyTable[0x45] = EKey::KEY_KEYPAD_ADD;
        s_MacKeyTable[0x41] = EKey::KEY_KEYPAD_DECIMAL;
        s_MacKeyTable[0x4B] = EKey::KEY_KEYPAD_DIVIDE;
        s_MacKeyTable[0x4C] = EKey::KEY_KEYPAD_ENTER;
        s_MacKeyTable[0x51] = EKey::KEY_KEYPAD_EQUAL;
        s_MacKeyTable[0x43] = EKey::KEY_KEYPAD_MULTIPLY;
        s_MacKeyTable[0x4E] = EKey::KEY_KEYPAD_SUBTRACT;

        s_MacKeyTable[0x7A] = EKey::KEY_F1;
        s_MacKeyTable[0x78] = EKey::KEY_F2;
        s_MacKeyTable[0x63] = EKey::KEY_F3;
        s_MacKeyTable[0x76] = EKey::KEY_F4;
        s_MacKeyTable[0x60] = EKey::KEY_F5;
        s_MacKeyTable[0x61] = EKey::KEY_F6;
        s_MacKeyTable[0x62] = EKey::KEY_F7;
        s_MacKeyTable[0x64] = EKey::KEY_F8;
        s_MacKeyTable[0x65] = EKey::KEY_F9;
        s_MacKeyTable[0x6D] = EKey::KEY_F10;
        s_MacKeyTable[0x67] = EKey::KEY_F11;
        s_MacKeyTable[0x6F] = EKey::KEY_F12;
        s_MacKeyTable[0x69] = EKey::KEY_F13;
        s_MacKeyTable[0x6B] = EKey::KEY_F14;
        s_MacKeyTable[0x71] = EKey::KEY_F15;
        s_MacKeyTable[0x6A] = EKey::KEY_F16;
        s_MacKeyTable[0x40] = EKey::KEY_F17;
        s_MacKeyTable[0x4F] = EKey::KEY_F18;
        s_MacKeyTable[0x50] = EKey::KEY_F19;
        s_MacKeyTable[0x5A] = EKey::KEY_F20;

        s_MacKeyTable[0x47] = EKey::KEY_NUM_LOCK;
        
        s_MacKeyTable[0x29] = EKey::KEY_SEMICOLON;
        s_MacKeyTable[0x2B] = EKey::KEY_COMMA;
        s_MacKeyTable[0x1B] = EKey::KEY_MINUS;
        s_MacKeyTable[0x2F] = EKey::KEY_PERIOD;
        s_MacKeyTable[0x32] = EKey::KEY_GRAVE_ACCENT;
        s_MacKeyTable[0x21] = EKey::KEY_LEFT_BRACKET;
        s_MacKeyTable[0x1E] = EKey::KEY_RIGHT_BRACKET;
        s_MacKeyTable[0x27] = EKey::KEY_APOSTROPHE;
        s_MacKeyTable[0x2A] = EKey::KEY_BACKSLASH;
        
        s_MacKeyTable[0x38] = EKey::KEY_LEFT_SHIFT;
        s_MacKeyTable[0x3B] = EKey::KEY_LEFT_CONTROL;
        s_MacKeyTable[0x3A] = EKey::KEY_LEFT_ALT;
        s_MacKeyTable[0x37] = EKey::KEY_LEFT_SUPER;
        s_MacKeyTable[0x3C] = EKey::KEY_RIGHT_SHIFT;
        s_MacKeyTable[0x3E] = EKey::KEY_RIGHT_CONTROL;
        s_MacKeyTable[0x36] = EKey::KEY_RIGHT_SUPER;
        s_MacKeyTable[0x3D] = EKey::KEY_RIGHT_ALT;
        s_MacKeyTable[0x6E] = EKey::KEY_MENU;

        s_MacKeyTable[0x18] = EKey::KEY_EQUAL;
        s_MacKeyTable[0x2C] = EKey::KEY_SLASH;
        s_MacKeyTable[0x0A] = EKey::KEY_WORLD_1;
        
        /*////////////////////////////////////////////////////////////////////////////////////////////////*/
        //Convert from EKey
        /*////////////////////////////////////////////////////////////////////////////////////////////////*/
        
        s_KeyTable[EKey::KEY_BACKSPACE] = 0x33;
        s_KeyTable[EKey::KEY_TAB]       = 0x30;
        s_KeyTable[EKey::KEY_ENTER]     = 0x24;
        s_KeyTable[EKey::KEY_CAPS_LOCK] = 0x39;
        s_KeyTable[EKey::KEY_SPACE]     = 0x31;
        s_KeyTable[EKey::KEY_PAGE_UP]   = 0x74;
        s_KeyTable[EKey::KEY_PAGE_DOWN] = 0x79;
        s_KeyTable[EKey::KEY_END]       = 0x77;
        s_KeyTable[EKey::KEY_HOME]      = 0x73;
        s_KeyTable[EKey::KEY_LEFT]      = 0x7B;
        s_KeyTable[EKey::KEY_UP]        = 0x7E;
        s_KeyTable[EKey::KEY_RIGHT]     = 0x7C;
        s_KeyTable[EKey::KEY_DOWN]      = 0x7D;
        s_KeyTable[EKey::KEY_INSERT]    = 0x72;
        s_KeyTable[EKey::KEY_DELETE]    = 0x75;
        s_KeyTable[EKey::KEY_ESCAPE]    = 0x35;

        s_KeyTable[EKey::KEY_0] = 0x1D;
        s_KeyTable[EKey::KEY_1] = 0x12;
        s_KeyTable[EKey::KEY_2] = 0x13;
        s_KeyTable[EKey::KEY_3] = 0x14;
        s_KeyTable[EKey::KEY_4] = 0x15;
        s_KeyTable[EKey::KEY_5] = 0x17;
        s_KeyTable[EKey::KEY_6] = 0x16;
        s_KeyTable[EKey::KEY_7] = 0x1A;
        s_KeyTable[EKey::KEY_8] = 0x1C;
        s_KeyTable[EKey::KEY_9] = 0x19;

        s_KeyTable[EKey::KEY_A] = 0x00;
        s_KeyTable[EKey::KEY_B] = 0x0B;
        s_KeyTable[EKey::KEY_C] = 0x08;
        s_KeyTable[EKey::KEY_D] = 0x02;
        s_KeyTable[EKey::KEY_E] = 0x0E;
        s_KeyTable[EKey::KEY_F] = 0x03;
        s_KeyTable[EKey::KEY_G] = 0x05;
        s_KeyTable[EKey::KEY_H] = 0x04;
        s_KeyTable[EKey::KEY_I] = 0x22;
        s_KeyTable[EKey::KEY_J] = 0x26;
        s_KeyTable[EKey::KEY_K] = 0x28;
        s_KeyTable[EKey::KEY_L] = 0x25;
        s_KeyTable[EKey::KEY_M] = 0x2E;
        s_KeyTable[EKey::KEY_N] = 0x2D;
        s_KeyTable[EKey::KEY_O] = 0x1F;
        s_KeyTable[EKey::KEY_P] = 0x23;
        s_KeyTable[EKey::KEY_Q] = 0x0C;
        s_KeyTable[EKey::KEY_R] = 0x0F;
        s_KeyTable[EKey::KEY_S] = 0x01;
        s_KeyTable[EKey::KEY_T] = 0x11;
        s_KeyTable[EKey::KEY_U] = 0x20;
        s_KeyTable[EKey::KEY_V] = 0x09;
        s_KeyTable[EKey::KEY_W] = 0x0D;
        s_KeyTable[EKey::KEY_X] = 0x07;
        s_KeyTable[EKey::KEY_Y] = 0x10;
        s_KeyTable[EKey::KEY_Z] = 0x06;

        s_KeyTable[EKey::KEY_KEYPAD_0]        = 0x52;
        s_KeyTable[EKey::KEY_KEYPAD_1]        = 0x53;
        s_KeyTable[EKey::KEY_KEYPAD_2]        = 0x54;
        s_KeyTable[EKey::KEY_KEYPAD_3]        = 0x55;
        s_KeyTable[EKey::KEY_KEYPAD_4]        = 0x56;
        s_KeyTable[EKey::KEY_KEYPAD_5]        = 0x57;
        s_KeyTable[EKey::KEY_KEYPAD_6]        = 0x58;
        s_KeyTable[EKey::KEY_KEYPAD_7]        = 0x59;
        s_KeyTable[EKey::KEY_KEYPAD_8]        = 0x5B;
        s_KeyTable[EKey::KEY_KEYPAD_9]        = 0x5C;
        s_KeyTable[EKey::KEY_KEYPAD_ADD]      = 0x45;
        s_KeyTable[EKey::KEY_KEYPAD_DECIMAL]  = 0x41;
        s_KeyTable[EKey::KEY_KEYPAD_DIVIDE]   = 0x4B;
        s_KeyTable[EKey::KEY_KEYPAD_ENTER]    = 0x4C;
        s_KeyTable[EKey::KEY_KEYPAD_EQUAL]    = 0x51;
        s_KeyTable[EKey::KEY_KEYPAD_MULTIPLY] = 0x43;
        s_KeyTable[EKey::KEY_KEYPAD_SUBTRACT] = 0x4E;

        s_KeyTable[EKey::KEY_F1]  = 0x7A;
        s_KeyTable[EKey::KEY_F2]  = 0x78;
        s_KeyTable[EKey::KEY_F3]  = 0x63;
        s_KeyTable[EKey::KEY_F4]  = 0x76;
        s_KeyTable[EKey::KEY_F5]  = 0x60;
        s_KeyTable[EKey::KEY_F6]  = 0x61;
        s_KeyTable[EKey::KEY_F7]  = 0x62;
        s_KeyTable[EKey::KEY_F8]  = 0x64;
        s_KeyTable[EKey::KEY_F9]  = 0x65;
        s_KeyTable[EKey::KEY_F10] = 0x6D;
        s_KeyTable[EKey::KEY_F11] = 0x67;
        s_KeyTable[EKey::KEY_F12] = 0x6F;
        s_KeyTable[EKey::KEY_F13] = 0x69;
        s_KeyTable[EKey::KEY_F14] = 0x6B;
        s_KeyTable[EKey::KEY_F15] = 0x71;
        s_KeyTable[EKey::KEY_F16] = 0x6A;
        s_KeyTable[EKey::KEY_F17] = 0x40;
        s_KeyTable[EKey::KEY_F18] = 0x4F;
        s_KeyTable[EKey::KEY_F19] = 0x50;
        s_KeyTable[EKey::KEY_F20] = 0x5A;

        s_KeyTable[EKey::KEY_NUM_LOCK] = 0x47;

        s_KeyTable[EKey::KEY_SEMICOLON]     = 0x29;
        s_KeyTable[EKey::KEY_COMMA]         = 0x2B;
        s_KeyTable[EKey::KEY_MINUS]         = 0x1B;
        s_KeyTable[EKey::KEY_PERIOD]        = 0x2F;
        s_KeyTable[EKey::KEY_GRAVE_ACCENT]  = 0x32;
        s_KeyTable[EKey::KEY_LEFT_BRACKET]  = 0x21;
        s_KeyTable[EKey::KEY_RIGHT_BRACKET] = 0x1E;
        s_KeyTable[EKey::KEY_APOSTROPHE]    = 0x27;
        s_KeyTable[EKey::KEY_BACKSLASH]     = 0x2A;

        s_KeyTable[EKey::KEY_LEFT_SHIFT]    = 0x38;
        s_KeyTable[EKey::KEY_LEFT_CONTROL]  = 0x3B;
        s_KeyTable[EKey::KEY_LEFT_ALT]      = 0x3A;
        s_KeyTable[EKey::KEY_LEFT_SUPER]    = 0x37;
        s_KeyTable[EKey::KEY_RIGHT_SHIFT]   = 0x3C;
        s_KeyTable[EKey::KEY_RIGHT_CONTROL] = 0x3E;
        s_KeyTable[EKey::KEY_RIGHT_ALT]     = 0x36;
        s_KeyTable[EKey::KEY_RIGHT_SUPER]   = 0x3D;
        s_KeyTable[EKey::KEY_MENU]          = 0x6E;
        
        s_KeyTable[EKey::KEY_EQUAL]   = 0x18;
        s_KeyTable[EKey::KEY_SLASH]   = 0x2C;
        s_KeyTable[EKey::KEY_WORLD_1] = 0x0A;
    }
    
    EKey CMacKeyboard::ConvertVirtualKey(uint32 keycode)
    {
        return s_MacKeyTable[keycode];
    }
    
    uint32 CMacKeyboard::ConvertVirtualKey(EKey keycode)
    {
        return s_KeyTable[keycode];
    }
    
    uint32 CMacKeyboard::GetModifierKeys(uint32 flags)
    {
        int32 modifiers = 0;
        if (flags & NSEventModifierFlagShift)
            modifiers |= EKeyModifier::KEY_MODIFIER_SHIFT;
        if (flags & NSEventModifierFlagControl)
            modifiers |= EKeyModifier::KEY_MODIFIER_CONTROL;
        if (flags & NSEventModifierFlagOption)
            modifiers |= EKeyModifier::KEY_MODIFIER_ALT;
        if (flags & NSEventModifierFlagCommand)
            modifiers |= EKeyModifier::KEY_MODIFIER_SUPER;
        if (flags & NSEventModifierFlagCapsLock)
            modifiers |= EKeyModifier::KEY_MODIFIER_CAPS_LOCK;
        return modifiers;
    }
}
#endif
