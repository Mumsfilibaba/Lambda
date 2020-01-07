#import "LambdaPch.h"

#if defined(LAMBDA_PLAT_MACOS)
    #import "Platform/macOS/MacPlatform.h"
    
    #import <Cocoa/Cocoa.h>
    #import <Foundation/Foundation.h>
    #import <CoreServices/CoreServices.h>
    
    #import <mach/mach.h>
    #import <mach/mach_time.h>

namespace Lambda
{
    //-----------
    //MacPlatform
    //-----------
    
    namespace MacPlatform
    {
        /*////////////////////////////////////////////////////////////////////////////////////////////////*/
        void MessageBox(const char* pCaption, const char* pText, EMessageBoxType type)
        {
            CFStringRef text    = CFStringCreateWithCString(0, pText, strlen(pText));
            CFStringRef caption = CFStringCreateWithCString(0, pCaption, strlen(pCaption));

            CFOptionFlags result;
            CFOptionFlags uType = kCFUserNotificationNoteAlertLevel;
            if (type == EMessageBoxType::MESSAGE_BOX_TYPE_ERROR)
            {
                uType = kCFUserNotificationStopAlertLevel;
            }
            
            CFUserNotificationDisplayAlert(0, uType, 0, 0, 0, caption, text, 0, 0, 0, &result);

            CFRelease(caption);
            CFRelease(text);
        }
        
        /*////////////////////////////////////////////////////////////////////////////////////////////////*/
        uint64 QueryPerformanceFrequency()
        {
            mach_timebase_info_data_t timeData = {};
            mach_timebase_info(&timeData);
            return timeData.denom / timeData.numer;
        }

        /*////////////////////////////////////////////////////////////////////////////////////////////////*/
        uint64 QueryPerformanceCounter()
        {
            return mach_absolute_time();
        }
    
        /*////////////////////////////////////////////////////////////////////////////////////////////////*/
        EKey   g_MacKeyTable[256];
        uint32 g_KeyTable[EKey::KEY_LAST + 1];
    
        void CreateKeyLookupTable()
        {
            //Init to zero
            memset(g_MacKeyTable, 0, sizeof(g_MacKeyTable));
            memset(g_KeyTable, 0, sizeof(g_KeyTable));
            
            /*////////////////////////////////////////////////////////////////////////////////////////////////*/
            //Convert from macOS-Keycode
            /*////////////////////////////////////////////////////////////////////////////////////////////////*/
            
            g_MacKeyTable[0x33] = EKey::KEY_BACKSPACE;
            g_MacKeyTable[0x30] = EKey::KEY_TAB;
            g_MacKeyTable[0x24] = EKey::KEY_ENTER;
            g_MacKeyTable[0x39] = EKey::KEY_CAPS_LOCK;
            g_MacKeyTable[0x31] = EKey::KEY_SPACE;
            g_MacKeyTable[0x74] = EKey::KEY_PAGE_UP;
            g_MacKeyTable[0x79] = EKey::KEY_PAGE_DOWN;
            g_MacKeyTable[0x77] = EKey::KEY_END;
            g_MacKeyTable[0x73] = EKey::KEY_HOME;
            g_MacKeyTable[0x7B] = EKey::KEY_LEFT;
            g_MacKeyTable[0x7E] = EKey::KEY_UP;
            g_MacKeyTable[0x7C] = EKey::KEY_RIGHT;
            g_MacKeyTable[0x7D] = EKey::KEY_DOWN;
            g_MacKeyTable[0x72] = EKey::KEY_INSERT;
            g_MacKeyTable[0x75] = EKey::KEY_DELETE;
            g_MacKeyTable[0x35] = EKey::KEY_ESCAPE;
            
            g_MacKeyTable[0x1D] = EKey::KEY_0;
            g_MacKeyTable[0x12] = EKey::KEY_1;
            g_MacKeyTable[0x13] = EKey::KEY_2;
            g_MacKeyTable[0x14] = EKey::KEY_3;
            g_MacKeyTable[0x15] = EKey::KEY_4;
            g_MacKeyTable[0x17] = EKey::KEY_5;
            g_MacKeyTable[0x16] = EKey::KEY_6;
            g_MacKeyTable[0x1A] = EKey::KEY_7;
            g_MacKeyTable[0x1C] = EKey::KEY_8;
            g_MacKeyTable[0x19] = EKey::KEY_9;
            
            g_MacKeyTable[0x00] = EKey::KEY_A;
            g_MacKeyTable[0x0B] = EKey::KEY_B;
            g_MacKeyTable[0x08] = EKey::KEY_C;
            g_MacKeyTable[0x02] = EKey::KEY_D;
            g_MacKeyTable[0x0E] = EKey::KEY_E;
            g_MacKeyTable[0x03] = EKey::KEY_F;
            g_MacKeyTable[0x05] = EKey::KEY_G;
            g_MacKeyTable[0x04] = EKey::KEY_H;
            g_MacKeyTable[0x22] = EKey::KEY_I;
            g_MacKeyTable[0x26] = EKey::KEY_J;
            g_MacKeyTable[0x28] = EKey::KEY_K;
            g_MacKeyTable[0x25] = EKey::KEY_L;
            g_MacKeyTable[0x2E] = EKey::KEY_M;
            g_MacKeyTable[0x2D] = EKey::KEY_N;
            g_MacKeyTable[0x1F] = EKey::KEY_O;
            g_MacKeyTable[0x23] = EKey::KEY_P;
            g_MacKeyTable[0x0C] = EKey::KEY_Q;
            g_MacKeyTable[0x0F] = EKey::KEY_R;
            g_MacKeyTable[0x01] = EKey::KEY_S;
            g_MacKeyTable[0x11] = EKey::KEY_T;
            g_MacKeyTable[0x20] = EKey::KEY_U;
            g_MacKeyTable[0x09] = EKey::KEY_V;
            g_MacKeyTable[0x0D] = EKey::KEY_W;
            g_MacKeyTable[0x07] = EKey::KEY_X;
            g_MacKeyTable[0x10] = EKey::KEY_Y;
            g_MacKeyTable[0x06] = EKey::KEY_Z;
            
            g_MacKeyTable[0x52] = EKey::KEY_KEYPAD_0;
            g_MacKeyTable[0x53] = EKey::KEY_KEYPAD_1;
            g_MacKeyTable[0x54] = EKey::KEY_KEYPAD_2;
            g_MacKeyTable[0x55] = EKey::KEY_KEYPAD_3;
            g_MacKeyTable[0x56] = EKey::KEY_KEYPAD_4;
            g_MacKeyTable[0x57] = EKey::KEY_KEYPAD_5;
            g_MacKeyTable[0x58] = EKey::KEY_KEYPAD_6;
            g_MacKeyTable[0x59] = EKey::KEY_KEYPAD_7;
            g_MacKeyTable[0x5B] = EKey::KEY_KEYPAD_8;
            g_MacKeyTable[0x5C] = EKey::KEY_KEYPAD_9;
            g_MacKeyTable[0x45] = EKey::KEY_KEYPAD_ADD;
            g_MacKeyTable[0x41] = EKey::KEY_KEYPAD_DECIMAL;
            g_MacKeyTable[0x4B] = EKey::KEY_KEYPAD_DIVIDE;
            g_MacKeyTable[0x4C] = EKey::KEY_KEYPAD_ENTER;
            g_MacKeyTable[0x51] = EKey::KEY_KEYPAD_EQUAL;
            g_MacKeyTable[0x43] = EKey::KEY_KEYPAD_MULTIPLY;
            g_MacKeyTable[0x4E] = EKey::KEY_KEYPAD_SUBTRACT;

            g_MacKeyTable[0x7A] = EKey::KEY_F1;
            g_MacKeyTable[0x78] = EKey::KEY_F2;
            g_MacKeyTable[0x63] = EKey::KEY_F3;
            g_MacKeyTable[0x76] = EKey::KEY_F4;
            g_MacKeyTable[0x60] = EKey::KEY_F5;
            g_MacKeyTable[0x61] = EKey::KEY_F6;
            g_MacKeyTable[0x62] = EKey::KEY_F7;
            g_MacKeyTable[0x64] = EKey::KEY_F8;
            g_MacKeyTable[0x65] = EKey::KEY_F9;
            g_MacKeyTable[0x6D] = EKey::KEY_F10;
            g_MacKeyTable[0x67] = EKey::KEY_F11;
            g_MacKeyTable[0x6F] = EKey::KEY_F12;
            g_MacKeyTable[0x69] = EKey::KEY_F13;
            g_MacKeyTable[0x6B] = EKey::KEY_F14;
            g_MacKeyTable[0x71] = EKey::KEY_F15;
            g_MacKeyTable[0x6A] = EKey::KEY_F16;
            g_MacKeyTable[0x40] = EKey::KEY_F17;
            g_MacKeyTable[0x4F] = EKey::KEY_F18;
            g_MacKeyTable[0x50] = EKey::KEY_F19;
            g_MacKeyTable[0x5A] = EKey::KEY_F20;

            g_MacKeyTable[0x47] = EKey::KEY_NUM_LOCK;
            
            g_MacKeyTable[0x29] = EKey::KEY_SEMICOLON;
            g_MacKeyTable[0x2B] = EKey::KEY_COMMA;
            g_MacKeyTable[0x1B] = EKey::KEY_MINUS;
            g_MacKeyTable[0x2F] = EKey::KEY_PERIOD;
            g_MacKeyTable[0x32] = EKey::KEY_GRAVE_ACCENT;
            g_MacKeyTable[0x21] = EKey::KEY_LEFT_BRACKET;
            g_MacKeyTable[0x1E] = EKey::KEY_RIGHT_BRACKET;
            g_MacKeyTable[0x27] = EKey::KEY_APOSTROPHE;
            g_MacKeyTable[0x2A] = EKey::KEY_BACKSLASH;
            
            g_MacKeyTable[0x38] = EKey::KEY_LEFT_SHIFT;
            g_MacKeyTable[0x3B] = EKey::KEY_LEFT_CONTROL;
            g_MacKeyTable[0x3A] = EKey::KEY_LEFT_ALT;
            g_MacKeyTable[0x37] = EKey::KEY_LEFT_SUPER;
            g_MacKeyTable[0x3C] = EKey::KEY_RIGHT_SHIFT;
            g_MacKeyTable[0x3E] = EKey::KEY_RIGHT_CONTROL;
            g_MacKeyTable[0x36] = EKey::KEY_RIGHT_SUPER;
            g_MacKeyTable[0x3D] = EKey::KEY_RIGHT_ALT;
            g_MacKeyTable[0x6E] = EKey::KEY_MENU;

            g_MacKeyTable[0x18] = EKey::KEY_EQUAL;
            g_MacKeyTable[0x2C] = EKey::KEY_SLASH;
            g_MacKeyTable[0x0A] = EKey::KEY_WORLD_1;
            
            /*////////////////////////////////////////////////////////////////////////////////////////////////*/
            //Convert from EKey
            /*////////////////////////////////////////////////////////////////////////////////////////////////*/
            
            g_KeyTable[EKey::KEY_BACKSPACE] = 0x33;
            g_KeyTable[EKey::KEY_TAB]       = 0x30;
            g_KeyTable[EKey::KEY_ENTER]     = 0x24;
            g_KeyTable[EKey::KEY_CAPS_LOCK] = 0x39;
            g_KeyTable[EKey::KEY_SPACE]     = 0x31;
            g_KeyTable[EKey::KEY_PAGE_UP]   = 0x74;
            g_KeyTable[EKey::KEY_PAGE_DOWN] = 0x79;
            g_KeyTable[EKey::KEY_END]       = 0x77;
            g_KeyTable[EKey::KEY_HOME]      = 0x73;
            g_KeyTable[EKey::KEY_LEFT]      = 0x7B;
            g_KeyTable[EKey::KEY_UP]        = 0x7E;
            g_KeyTable[EKey::KEY_RIGHT]     = 0x7C;
            g_KeyTable[EKey::KEY_DOWN]      = 0x7D;
            g_KeyTable[EKey::KEY_INSERT]    = 0x72;
            g_KeyTable[EKey::KEY_DELETE]    = 0x75;
            g_KeyTable[EKey::KEY_ESCAPE]    = 0x35;

            g_KeyTable[EKey::KEY_0] = 0x1D;
            g_KeyTable[EKey::KEY_1] = 0x12;
            g_KeyTable[EKey::KEY_2] = 0x13;
            g_KeyTable[EKey::KEY_3] = 0x14;
            g_KeyTable[EKey::KEY_4] = 0x15;
            g_KeyTable[EKey::KEY_5] = 0x17;
            g_KeyTable[EKey::KEY_6] = 0x16;
            g_KeyTable[EKey::KEY_7] = 0x1A;
            g_KeyTable[EKey::KEY_8] = 0x1C;
            g_KeyTable[EKey::KEY_9] = 0x19;

            g_KeyTable[EKey::KEY_A] = 0x00;
            g_KeyTable[EKey::KEY_B] = 0x0B;
            g_KeyTable[EKey::KEY_C] = 0x08;
            g_KeyTable[EKey::KEY_D] = 0x02;
            g_KeyTable[EKey::KEY_E] = 0x0E;
            g_KeyTable[EKey::KEY_F] = 0x03;
            g_KeyTable[EKey::KEY_G] = 0x05;
            g_KeyTable[EKey::KEY_H] = 0x04;
            g_KeyTable[EKey::KEY_I] = 0x22;
            g_KeyTable[EKey::KEY_J] = 0x26;
            g_KeyTable[EKey::KEY_K] = 0x28;
            g_KeyTable[EKey::KEY_L] = 0x25;
            g_KeyTable[EKey::KEY_M] = 0x2E;
            g_KeyTable[EKey::KEY_N] = 0x2D;
            g_KeyTable[EKey::KEY_O] = 0x1F;
            g_KeyTable[EKey::KEY_P] = 0x23;
            g_KeyTable[EKey::KEY_Q] = 0x0C;
            g_KeyTable[EKey::KEY_R] = 0x0F;
            g_KeyTable[EKey::KEY_S] = 0x01;
            g_KeyTable[EKey::KEY_T] = 0x11;
            g_KeyTable[EKey::KEY_U] = 0x20;
            g_KeyTable[EKey::KEY_V] = 0x09;
            g_KeyTable[EKey::KEY_W] = 0x0D;
            g_KeyTable[EKey::KEY_X] = 0x07;
            g_KeyTable[EKey::KEY_Y] = 0x10;
            g_KeyTable[EKey::KEY_Z] = 0x06;

            g_KeyTable[EKey::KEY_KEYPAD_0]        = 0x52;
            g_KeyTable[EKey::KEY_KEYPAD_1]        = 0x53;
            g_KeyTable[EKey::KEY_KEYPAD_2]        = 0x54;
            g_KeyTable[EKey::KEY_KEYPAD_3]        = 0x55;
            g_KeyTable[EKey::KEY_KEYPAD_4]        = 0x56;
            g_KeyTable[EKey::KEY_KEYPAD_5]        = 0x57;
            g_KeyTable[EKey::KEY_KEYPAD_6]        = 0x58;
            g_KeyTable[EKey::KEY_KEYPAD_7]        = 0x59;
            g_KeyTable[EKey::KEY_KEYPAD_8]        = 0x5B;
            g_KeyTable[EKey::KEY_KEYPAD_9]        = 0x5C;
            g_KeyTable[EKey::KEY_KEYPAD_ADD]      = 0x45;
            g_KeyTable[EKey::KEY_KEYPAD_DECIMAL]  = 0x41;
            g_KeyTable[EKey::KEY_KEYPAD_DIVIDE]   = 0x4B;
            g_KeyTable[EKey::KEY_KEYPAD_ENTER]    = 0x4C;
            g_KeyTable[EKey::KEY_KEYPAD_EQUAL]    = 0x51;
            g_KeyTable[EKey::KEY_KEYPAD_MULTIPLY] = 0x43;
            g_KeyTable[EKey::KEY_KEYPAD_SUBTRACT] = 0x4E;

            g_KeyTable[EKey::KEY_F1]  = 0x7A;
            g_KeyTable[EKey::KEY_F2]  = 0x78;
            g_KeyTable[EKey::KEY_F3]  = 0x63;
            g_KeyTable[EKey::KEY_F4]  = 0x76;
            g_KeyTable[EKey::KEY_F5]  = 0x60;
            g_KeyTable[EKey::KEY_F6]  = 0x61;
            g_KeyTable[EKey::KEY_F7]  = 0x62;
            g_KeyTable[EKey::KEY_F8]  = 0x64;
            g_KeyTable[EKey::KEY_F9]  = 0x65;
            g_KeyTable[EKey::KEY_F10] = 0x6D;
            g_KeyTable[EKey::KEY_F11] = 0x67;
            g_KeyTable[EKey::KEY_F12] = 0x6F;
            g_KeyTable[EKey::KEY_F13] = 0x69;
            g_KeyTable[EKey::KEY_F14] = 0x6B;
            g_KeyTable[EKey::KEY_F15] = 0x71;
            g_KeyTable[EKey::KEY_F16] = 0x6A;
            g_KeyTable[EKey::KEY_F17] = 0x40;
            g_KeyTable[EKey::KEY_F18] = 0x4F;
            g_KeyTable[EKey::KEY_F19] = 0x50;
            g_KeyTable[EKey::KEY_F20] = 0x5A;

            g_KeyTable[EKey::KEY_NUM_LOCK] = 0x47;

            g_KeyTable[EKey::KEY_SEMICOLON]     = 0x29;
            g_KeyTable[EKey::KEY_COMMA]         = 0x2B;
            g_KeyTable[EKey::KEY_MINUS]         = 0x1B;
            g_KeyTable[EKey::KEY_PERIOD]        = 0x2F;
            g_KeyTable[EKey::KEY_GRAVE_ACCENT]  = 0x32;
            g_KeyTable[EKey::KEY_LEFT_BRACKET]  = 0x21;
            g_KeyTable[EKey::KEY_RIGHT_BRACKET] = 0x1E;
            g_KeyTable[EKey::KEY_APOSTROPHE]    = 0x27;
            g_KeyTable[EKey::KEY_BACKSLASH]     = 0x2A;

            g_KeyTable[EKey::KEY_LEFT_SHIFT]    = 0x38;
            g_KeyTable[EKey::KEY_LEFT_CONTROL]  = 0x3B;
            g_KeyTable[EKey::KEY_LEFT_ALT]      = 0x3A;
            g_KeyTable[EKey::KEY_LEFT_SUPER]    = 0x37;
            g_KeyTable[EKey::KEY_RIGHT_SHIFT]   = 0x3C;
            g_KeyTable[EKey::KEY_RIGHT_CONTROL] = 0x3E;
            g_KeyTable[EKey::KEY_RIGHT_ALT]     = 0x36;
            g_KeyTable[EKey::KEY_RIGHT_SUPER]   = 0x3D;
            g_KeyTable[EKey::KEY_MENU]          = 0x6E;
            
            g_KeyTable[EKey::KEY_EQUAL]   = 0x18;
            g_KeyTable[EKey::KEY_SLASH]   = 0x2C;
            g_KeyTable[EKey::KEY_WORLD_1] = 0x0A;
        }
    
        /*////////////////////////////////////////////////////////////////////////////////////////////////*/
        uint32 ConvertToVirtualKey(EKey keycode)
        {
            return g_KeyTable[keycode];
        }
    
        /*////////////////////////////////////////////////////////////////////////////////////////////////*/
        EKey ConvertFromVirtualKey(uint32 keycode)
        {
            return g_MacKeyTable[keycode];
        }
    
        /*////////////////////////////////////////////////////////////////////////////////////////////////*/
        uint32 ConvertModifierKeyFlags(uint32 flags)
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
    
        /*////////////////////////////////////////////////////////////////////////////////////////////////*/
        EMouseButton ConvertMouseButton(uint32 button)
        {
            if (button == 0)
                return EMouseButton::MOUSEBUTTON_LEFT;
            else if (button == 1)
                return EMouseButton::MOUSEBUTTON_RIGHT;
            else if (button == 2)
                return EMouseButton::MOUSEBUTTON_MIDDLE;
            else
                return EMouseButton::MOUSEBUTTON_UNKNOWN;
        }
    }
}
#endif
