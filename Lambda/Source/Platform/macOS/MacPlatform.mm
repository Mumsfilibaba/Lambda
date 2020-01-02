#import "LambdaPch.h"

#if defined(LAMBDA_PLAT_MACOS)
    #import "Platform/macOS/MacPlatform.h"
    
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
        /*//////////////////////////////////////////////////////////////////////////*/
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
            
            CFUserNotificationDisplayAlert(0, uType, 0, 0, 0, text, caption, 0, 0, 0, &result);

            CFRelease(caption);
            CFRelease(text);
        }
        
        /*/////////////////////////////*/
        uint64 QueryPerformanceFrequency()
        {
            mach_timebase_info_data_t timeData = {};
            mach_timebase_info(&timeData);
            return timeData.denom / timeData.numer;
        }

        /*////////////////////////////*/
        uint64 QueryPerformanceCounter()
        {
            return mach_absolute_time();
        }        
    }
}
#endif
