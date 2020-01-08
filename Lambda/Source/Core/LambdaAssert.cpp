#include "LambdaPch.h"

#include "Core/LambdaAssert.h"
#include "Core/Platform.h"

#include "Core/Engine/Engine.h"
#include "Core/Engine/Console.h"

#include "AssertManager.h"

//For other platforms we just use assert(false) for now
#if !defined(LAMBDA_VISUAL_STUDIO)
	#include <assert.h>
#endif
#include <stdio.h>

namespace Lambda
{
	namespace Assert
	{
		//------
		//Assert
		//------

        /*////////////////////////////////////////////////////////////////////////////////////////////////*/
        static CAssertManager& GetManager()
        {
            static CAssertManager s_Instance;
            return s_Instance;
        }
    
        /*////////////////////////////////////////////////////////////////////////////////////////////////*/
        void SetShowDialogOnAssert(bool bShowDialogOnAssert)
        {
            GetManager().SetShowDialogOnAssert(bShowDialogOnAssert);
        }
    
        /*////////////////////////////////////////////////////////////////////////////////////////////////*/
        void SetWriteConsoleOnAssert(bool bWriteConsoleOnAssert)
        {
            GetManager().SetWriteConsoleOnAssert(bWriteConsoleOnAssert);
        }
    
        /*////////////////////////////////////////////////////////////////////////////////////////////////*/
        bool ShowDialogOnAssert()
        {
            return GetManager().ShowDialogOnAssert();
        }
        
        /*////////////////////////////////////////////////////////////////////////////////////////////////*/
        bool WriteConsoleOnAssert()
        {
            return GetManager().WriteConsoleOnAssert();
        }
        
        /*////////////////////////////////////////////////////////////////////////////////////////////////*/
		void AssertWithMessage(int nLine, const char* pFile, const char* pFormat, ...)
		{
			//Clear messagebuffer
			static char message[MAX_MESSAGE_LENGTH];
			memset(message, 0, sizeof(message));
			
			//Print message to buffer
			va_list args;
			va_start(args, pFormat);
			vsnprintf(message, MAX_MESSAGE_LENGTH, pFormat, args);
			va_end(args);
            
            //Get assertion message
            static char assertionMessage[MAX_MESSAGE_LENGTH];
            memset(assertionMessage, 0, sizeof(assertionMessage));
            snprintf(assertionMessage, MAX_MESSAGE_LENGTH, "Assert Failed\nLine: %d\nFile: %s\n%s\n", nLine, pFile, message);
            
			//Print to console
            if (WriteConsoleOnAssert())
            {
                IConsole* pConsole = CConsole::Get();
                if (pConsole)
                {
                    pConsole->SetTextColor(EConsoleColor::CONSOLE_COLOR_RED);
                    pConsole->Print("%s", assertionMessage);
                    pConsole->Reset();
                }
            }

			//Print messagebox
            if (ShowDialogOnAssert())
            {
                Platform::Misc::MessageBox("Assertion Failed", assertionMessage, EMessageBoxType::MESSAGE_BOX_TYPE_ERROR);
            }
		}
	}
}
