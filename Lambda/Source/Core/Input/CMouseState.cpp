#include "LambdaPch.h"
#include "Core/Input/CMouseState.h"

namespace Lambda
{
	CMouseState::CMouseState() noexcept
		: m_Position(0, 0),
		m_ScrollWheel(0.0f, 0.0f),
		m_Buttons()
	{
	}


	CMouseState::CMouseState(const CMouseState& other) noexcept
		: m_Position(0, 0),
		m_ScrollWheel(0.0f, 0.0f),
		m_Buttons()
	{
		m_Position		= other.m_Position;
		m_ScrollWheel	= other.m_ScrollWheel;
		memcpy(m_Buttons, other.m_Buttons, sizeof(m_Buttons));
	}


	CMouseState::CMouseState(CMouseState&& other) noexcept
		: m_Position(0, 0),
		m_ScrollWheel(0.0f, 0.0f),
		m_Buttons()
	{
		m_Position		= other.m_Position;
		m_ScrollWheel	= other.m_ScrollWheel;
		memcpy(m_Buttons, other.m_Buttons, sizeof(m_Buttons));
	}


	CMouseState& CMouseState::operator=(const CMouseState& other) noexcept
	{
		if (this != &other)
		{
			m_Position = other.m_Position;
			m_ScrollWheel = other.m_ScrollWheel;
			memcpy(this, &other, sizeof(CMouseState));
		}

		return *this;
	}


	CMouseState& CMouseState::operator=(CMouseState&& other) noexcept
	{
		if (this != &other)
		{
			m_Position		= other.m_Position;
			m_ScrollWheel	= other.m_ScrollWheel;
			memcpy(this, &other, sizeof(CMouseState));
		}

		return *this;
	}


	void CMouseState::SetPosition(const glm::ivec2& position)
	{
		m_Position = position;
	}


	void CMouseState::SetVerticalScollValue(float value)
	{
		m_ScrollWheel.y = value;
	}


	void CMouseState::SetHorizontalScollValue(float value)
	{
		m_ScrollWheel.x = value;
	}


	void CMouseState::SetButtonUp(EMouseButton button)
	{
		m_Buttons[button] = false;
	}


	void CMouseState::SetButtonDown(EMouseButton button)
	{
		m_Buttons[button] = true;
	}
}