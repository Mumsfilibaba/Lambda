#pragma once
#include "IKeyboardController.h"
#include "../Event/IEventListener.h"

namespace Lambda
{
	class CKeyPressedEvent;
	class CKeyReleasedEvent;

	//-------------------
	//CKeyboardController 
	//-------------------

	class CKeyboardController : public IKeyboardController, public IEventListener
	{
	public:
		CKeyboardController();
		~CKeyboardController();

		virtual bool OnEvent(const CEvent& event) override final;
		virtual bool IsKeyUp(EKey key) const override final;
		virtual bool IsKeyDown(EKey key) const override final;
		virtual CKeyboardState GetKeyboardState() const override final;
	private:
		bool OnKeyPressed(const CKeyPressedEvent& event);
		bool OnKeyReleased(const CKeyReleasedEvent& event);
	private:
		CKeyboardState m_KeyboardState;
	};
}