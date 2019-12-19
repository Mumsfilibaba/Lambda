#pragma once
#include "EMouseButton.h"
#include <glm/glm.hpp>

namespace Lambda
{
	using Vec2  = glm::vec2;
	using Point = glm::ivec2;

    //-----------
    //CMouseState
    //-----------

    class LAMBDA_API CMouseState final
    {
    public:
        CMouseState() noexcept;
		CMouseState(const Vec2& ScrollWheel, const Point& position, const bool bButtonStates[static_cast<int32>(EMouseButton::MOUSEBUTTON_LAST) + 1]) noexcept;
        CMouseState(const CMouseState& other) noexcept;
        CMouseState(CMouseState&& other) noexcept;
        ~CMouseState() = default;
        
        CMouseState& operator=(const CMouseState& other) noexcept;
        CMouseState& operator=(CMouseState&& other) noexcept;

		_forceinline const Point& GetPosition() const
		{
			return m_Position;
		}

		_forceinline const Vec2& GetScollValue() const
		{
			return m_ScrollWheel;
		}

		_forceinline bool IsButtonUp(EMouseButton button) const
		{
			return m_bButtonStates[static_cast<int32>(button)] == false;
		}

		_forceinline bool IsButtonDown(EMouseButton button) const
		{
			return m_bButtonStates[static_cast<int32>(button)] == true;
		}
    private:
		Point m_Position;
		Vec2 m_ScrollWheel;
		bool m_bButtonStates[static_cast<int32>(EMouseButton::MOUSEBUTTON_LAST)+1];
    };
}
