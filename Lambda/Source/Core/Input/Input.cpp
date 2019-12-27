#include "LambdaPch.h"
#include "Core/Input/Input.h"

namespace Lambda
{
    namespace Input
    {
        //-----------------
        //SInputControllers
        //-----------------
        struct SInputControllers
        {
            IKeyboardController* pKeyboardController = nullptr;
            IMouseController* pMouseController = nullptr;
            IGamepadController* pGamepadController = nullptr; 
        } g_InputControllers;

        //-----
        //Input
        //-----
        bool Initialize()
        {
            //Create controller for keyboard
            IKeyboardController* pKeyboardController = DBG_NEW CKeyboardController();
            SetKeyboardController(pKeyboardController);

            //Create controller for mouse
            IMouseController* pMouseController = DBG_NEW CMouseController();
            SetMouseController(pMouseController);

            //Create controller for gamepad
    #if defined(LAMBDA_PLAT_WINDOWS)
            IGamepadController* pGamepadController = DBG_NEW CXInputGamepadController();
            SetGamepadController(pGamepadController);
    #else
            CDummyGamepadController* pGamepadController = DBG_NEW CDummyGamepadController();
            SetGamepadController(pGamepadController);
    #endif
            return true;
        }


        void Update()
        {
        }


        void Release()
        {
            SafeDelete(g_InputControllers.pKeyboardController);
            SafeDelete(g_InputControllers.pMouseController);
            SafeDelete(g_InputControllers.pGamepadController);
        }


        void SetKeyboardController(IKeyboardController* pKeyboardController)
        {
            LAMBDA_ASSERT_PRINT(pKeyboardController != nullptr, "pKeyboardController cannot be nullptr\n");

            SafeDelete(g_InputControllers.pKeyboardController);
            g_InputControllers.pKeyboardController = pKeyboardController;
        }


        void SetMouseController(IMouseController* pMouseController)
        {
            LAMBDA_ASSERT_PRINT(pMouseController != nullptr, "pMouseController cannot be nullptr\n");

            SafeDelete(g_InputControllers.pMouseController);
            g_InputControllers.pMouseController = pMouseController;
        }


        void SetGamepadController(IGamepadController* pGamepadController)
        {
            LAMBDA_ASSERT_PRINT(pGamepadController != nullptr, "pGamepadController cannot be nullptr\n");

            SafeDelete(g_InputControllers.pGamepadController);
            g_InputControllers.pGamepadController = pGamepadController;
        }

        
        void GetMouseState(CMouseState* pMouseState)
        {
        }


        void GetGamepadState(CGamepadState* pGamepadState)
        {
        }


        void GetKeyboardState(CKeyboardState* pKeyboardState)
        {
        }


        IKeyboardController* GetKeyboardController()
        {
            LAMBDA_ASSERT_PRINT(g_InputControllers.pKeyboardController != nullptr, "KeyboardController not initialized properly\n");
            return g_InputControllers.pKeyboardController;
        }


        IMouseController* GetMouseController()
        {
            LAMBDA_ASSERT_PRINT(g_InputControllers.pMouseController != nullptr, "MouseController not initialized properly\n");
            return g_InputControllers.pMouseController;
        }


        IGamepadController* GetGamepadController()
        {
            LAMBDA_ASSERT_PRINT(g_InputControllers.pGamepadController != nullptr, "GamepadController not initialized properly\n");
            return g_InputControllers.pGamepadController;
        }
    }
}