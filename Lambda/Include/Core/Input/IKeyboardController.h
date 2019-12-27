#pragma once
#include "CKeyboardState.h"

namespace Lambda
{
    //-------------------
    //IKeyboardController
    //-------------------
    class IKeyboardController
    {
    public:
        LAMBDA_INTERFACE(IKeyboardController);

		virtual bool IsKeyUp(EKey key) const = 0;
		virtual bool IsKeyDown(EKey key) const = 0;
        virtual CKeyboardState GetKeyboardState() const = 0;
    };

	//--------------------------------------------------------------------
	//CDummyKeyboardController - Used when no real controller is available
	//--------------------------------------------------------------------
	class CDummyKeyboardController final : public IKeyboardController
	{
	public:
		CDummyKeyboardController() = default;
		~CDummyKeyboardController() = default;

		virtual bool IsKeyUp(EKey) const override final
		{
			return false;
		}

		virtual bool IsKeyDown(EKey) const override final
		{
			return false;
		}

		virtual CKeyboardState GetKeyboardState() const override final
		{
			return CKeyboardState();
		}
	};
}
