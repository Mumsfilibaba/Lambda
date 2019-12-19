#include "LambdaPch.h"
#include "Core/Input/CKeyboardState.h"

namespace Lambda
{
	//--------------
	//CKeyboardState
	//--------------

	CKeyboardState::CKeyboardState()
		: m_bKeyStates()
	{
		memset(m_bKeyStates, 0, sizeof(m_bKeyStates));
	}


	CKeyboardState::CKeyboardState(const bool bKeyStates[static_cast<int32>(EKey::KEY_LAST)+1])
		: m_bKeyStates()
	{
		memcpy(m_bKeyStates, bKeyStates, sizeof(m_bKeyStates));
	}
	
	
	CKeyboardState::CKeyboardState(const CKeyboardState& other) noexcept
		: m_bKeyStates()
	{
		memcpy(this, &other, sizeof(CKeyboardState));
	}
	
	
	CKeyboardState::CKeyboardState(CKeyboardState&& other) noexcept
		: m_bKeyStates()
	{
		memcpy(this, &other, sizeof(CKeyboardState));
	}
	
	
	CKeyboardState& CKeyboardState::operator=(const CKeyboardState& other) noexcept
	{
		if (this != &other)
			memcpy(this, &other, sizeof(CKeyboardState));

		return *this;
	}
	
	
	CKeyboardState& CKeyboardState::operator=(CKeyboardState&& other) noexcept
	{
		if (this != &other)
			memcpy(this, &other, sizeof(CKeyboardState));

		return *this;
	}
}