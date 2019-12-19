#include "LambdaPch.h"
#include "Core/Input/CMouseState.h"

namespace Lambda
{
	//-----------
	//CMouseState
	//-----------

	CMouseState::CMouseState() noexcept
		: m_Position(0, 0),
		m_ScrollWheel(0.0f, 0.0f),
		m_bButtonStates()
	{
		memset(m_bButtonStates, 0, sizeof(m_bButtonStates));
	}

	
	CMouseState::CMouseState(const Vec2& scrollWheel, const Point& position, const bool bButtonStates[static_cast<int32>(EMouseButton::MOUSEBUTTON_LAST)+1]) noexcept
		: m_Position(position),
		m_ScrollWheel(scrollWheel),
		m_bButtonStates()
	{
		memcpy(m_bButtonStates, bButtonStates, sizeof(m_bButtonStates));
	}


	CMouseState::CMouseState(const CMouseState& other) noexcept
		: m_Position(0, 0),
		m_ScrollWheel(0.0f, 0.0f),
		m_bButtonStates()
	{
		memcpy(this, &other, sizeof(CMouseState));
	}


	CMouseState::CMouseState(CMouseState&& other) noexcept
		: m_Position(0, 0),
		m_ScrollWheel(0.0f, 0.0f),
		m_bButtonStates()
	{
		memcpy(this, &other, sizeof(CMouseState));
	}


	CMouseState& CMouseState::operator=(const CMouseState& other) noexcept
	{
		if (this != &other)
		{
			memcpy(this, &other, sizeof(CMouseState));
		}

		return *this;
	}


	CMouseState& CMouseState::operator=(CMouseState&& other) noexcept
	{
		if (this != &other)
		{
			memcpy(this, &other, sizeof(CMouseState));
		}

		return *this;
	}
}