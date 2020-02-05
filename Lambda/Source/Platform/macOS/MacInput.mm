#include "LambdaPch.h"

#ifdef LAMBDA_PLAT_MACOS
    #include "Platform/macOS/MacInput.h"
    #include "Platform/macOS/MacKeyboard.h"
    
    #include <AppKit/AppKit.h>

namespace Lambda
{
    void MacInput::Init()
    {
    }
    
    void MacInput::Update()
    {
    }

    void MacInput::SetMousePosition(IWindow* pRelativeWindow, int32 x, int32 y)
    {
        
    }

    void MacInput::GetMousePosition(IWindow* pRelativeWindow, int32& x, int32& y)
    {
        
    }

    uint32 MacInput::GetModiferKeys()
    {
        uint32 flags = [NSEvent modifierFlags];
        return MacKeyboard::GetModifierKeys(flags);
    }
}
#endif
