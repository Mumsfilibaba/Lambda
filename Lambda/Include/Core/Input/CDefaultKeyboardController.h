#pragma once
#include "IKeyboardController.h"
#include "../Event/IEventListener.h"

namespace Lambda
{
	class CKeyPressedEvent;
	class CKeyReleasedEvent;

	//--------------------------
	//CDefaultKeyboardController 
	//--------------------------

	class CDefaultKeyboardController : public IKeyboardController, public IEventListener
	{
	public:
		CDefaultKeyboardController();
		~CDefaultKeyboardController() = default;

		virtual CKeyboardState GetKeyboardState() const override final;
		virtual bool OnEvent(const CEvent& event) override final;
	private:
		bool OnKeyPressed(const CKeyPressedEvent& event);
		bool OnKeyReleased(const CKeyReleasedEvent& event);
	private:
		CKeyboardState m_KeyboardState;
	};
}