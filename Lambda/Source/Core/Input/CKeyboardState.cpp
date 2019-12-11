#include "LambdaPch.h"
#include "Core/Input/CKeyboardState.h"

namespace Lambda
{
	CKeyboardState::CKeyboardState()
		: m_Keys()
	{
		memset(m_Keys, 0, sizeof(m_Keys));
	}
	
	
	CKeyboardState::CKeyboardState(const CKeyboardState& other) noexcept
		: m_Keys()
	{
		memcpy(m_Keys, other.m_Keys, sizeof(m_Keys));
	}
	
	
	CKeyboardState::CKeyboardState(CKeyboardState&& other) noexcept
		: m_Keys()
	{
		memcpy(m_Keys, other.m_Keys, sizeof(m_Keys));
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
	
	
	void CKeyboardState::SetKeyStateDown(EKey key)
	{
		m_Keys[key] = true;
	}
	
	
	void CKeyboardState::SetKeyStateUp(EKey key)
	{
		m_Keys[key] = false;
	}
}