#pragma once
#include "System/Input.h"
#if defined(LAMBDA_PLAT_MACOS)
    #include "macOSWindow.h"

namespace Lambda
{
    //----------
    //MacOSInput
    //----------
    
    class MacOSInput final : public Input
    {
        friend class MacOSWindow;
        
    public:
        LAMBDA_NO_COPY(MacOSInput);
        
        MacOSInput();
        ~MacOSInput() = default;
        
        Key GLFWKeyToLambda(uint32 keycode) const;
        uint32 LambdaKeyToGLFW(Key keycode) const;
        
        virtual bool InternalIsKeyUp(Key keycode) const override final;
        virtual bool InternalIsKeyDown(Key keycode) const override final;
        
        virtual void InternalSetMousePosition(float x, float y) const override final;
        virtual void InternalGetMousePosition(float& x, float& y) const override final;
        virtual void InternalSetMousePosition(IWindow* pRelativeTo, float x, float y) const override final;
        virtual void InternalGetMousePosition(IWindow* pRelativeTo, float& x, float& y) const override final;
        
    private:
        Key      m_LambdaKeyTable[GLFW_KEY_LAST + 1];
        uint32   m_GLFWKeyTable[KEY_LAST + 1];
        
    private:
        static Key ConvertGLFWKey(uint32 keycode);
        static uint32 ConvertLambdaKey(Key keycode);
    };
    
    
    inline Key MacOSInput::ConvertGLFWKey(uint32 keycode)
    {
        return ((MacOSInput&)Input::Get()).GLFWKeyToLambda(keycode);
    }
    
    
    inline uint32 MacOSInput::ConvertLambdaKey(Key keycode)
    {
        return ((MacOSInput&)Input::Get()).LambdaKeyToGLFW(keycode);
    }
}

#endif
