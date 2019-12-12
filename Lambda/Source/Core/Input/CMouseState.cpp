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
		memcpy(this, &other, sizeof(CMouseState));
	}


	CMouseState::CMouseState(CMouseState&& other) noexcept
		: m_Position(0, 0),
		m_ScrollWheel(0.0f, 0.0f),
		m_Buttons()
	{
		memcpy(this, &other, sizeof(CMouseState));
	}


	CMouseState& CMouseState::operator=(const CMouseState& other) noexcept
	{
		if (this != &other)
			memcpy(this, &other, sizeof(CMouseState));

		return *this;
	}


	CMouseState& CMouseState::operator=(CMouseState&& other) noexcept
	{
		if (this != &other)
			memcpy(this, &other, sizeof(CMouseState));

		return *this;
	}


	void CMouseState::SetPosition(const Point& position)
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