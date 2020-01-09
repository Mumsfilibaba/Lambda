#include "LambdaPch.h"

#if defined(LAMBDA_PLAT_MACOS)
    #include "Platform/macOS/MacPlatform.h"
    
    #include <Cocoa/Cocoa.h>
    #include <Foundation/Foundation.h>
    #include <CoreServices/CoreServices.h>
    
    #include <mach/mach.h>
    #include <mach/mach_time.h>

namespace Lambda
{
    //-----------
    //MacPlatform
    //-----------
    
    namespace MacPlatform
    {
        //----
        //Misc
        //----
    
        namespace Misc
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
        }
        
        //----
        //Time
        //----
        namespace Time
        {
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
