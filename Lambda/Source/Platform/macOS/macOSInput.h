#pragma once
#include "System/Input.h"
#if defined(LAMBDA_PLAT_MACOS)
    #include "macOSWindow.h"

namespace Lambda
{
    //----------
    //macOSInput
    //----------
    
    class macOSInput final : public Input
    {
        friend class macOSWindow;
        
    public:
        LAMBDA_NO_COPY(macOSInput);
        
        macOSInput();
        ~macOSInput() = default;
        
        Key GLFWKeyToLambda(uint32 keycode) const;
        uint32  LambdaKeyToGLFW(Key keycode) const;
        
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
        static Key      ConvertGLFWKey(uint32 keycode);
        static uint32   ConvertLambdaKey(Key keycode);
    };
    
    
    inline Key macOSInput::ConvertGLFWKey(uint32 keycode)
    {
        return ((macOSInput&)Input::Get()).GLFWKeyToLambda(keycode);
    }
    
    
    inline uint32 macOSInput::ConvertLambdaKey(Key keycode)
    {
        return ((macOSInput&)Input::Get()).LambdaKeyToGLFW(keycode);
    }
}

#endif
