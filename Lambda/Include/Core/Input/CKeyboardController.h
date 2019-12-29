#pragma once
#include "IKeyboardController.h"
#include "Core/Event/IEventListener.h"

namespace Lambda
{
	//-------------------
	//CKeyboardController
	//-------------------
	class CKeyboardController : public IKeyboardController, public IEventListener
	{
	public:
		CKeyboardController();
		~CKeyboardController();

		virtual bool OnEvent(const SSystemEvent& event) override final;

		virtual bool IsKeyUp(EKey key) const override final;
		virtual bool IsKeyDown(EKey key) const override final;
		virtual CKeyboardState GetKeyboardState() const override final;
	private:
		bool m_bKeyStates[static_cast<int32>(EKey::KEY_LAST) + 1];
	};
}