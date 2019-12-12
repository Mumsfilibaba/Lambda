#pragma once
#include "EMouseButton.h"
#include <glm/glm.hpp>

namespace Lambda
{
	using Point = glm::ivec2;

    //-----------
    //CMouseState
    //-----------

    class LAMBDA_API CMouseState final
    {
    public:
        CMouseState() noexcept;
        CMouseState(const CMouseState& other) noexcept;
        CMouseState(CMouseState&& other) noexcept;
        ~CMouseState() = default;
        
        CMouseState& operator=(const CMouseState& other) noexcept;
        CMouseState& operator=(CMouseState&& other) noexcept;

		void SetPosition(const Point& position);
		void SetVerticalScollValue(float value);
		void SetHorizontalScollValue(float value);
		void SetButtonUp(EMouseButton button);
		void SetButtonDown(EMouseButton button);

		_forceinline const Point& GetPosition() const
		{
			return m_Position;
		}

		_forceinline float GetVerticalScollValue() const
		{
			return m_ScrollWheel.y;
		}

		_forceinline float GetHorizontalScollValue() const
		{
			return m_ScrollWheel.x;
		}

		_forceinline bool IsButtonUp(EMouseButton button) const
		{
			return m_Buttons[button] == false;
		}

		_forceinline bool IsButtonDown(EMouseButton button) const
		{
			return m_Buttons[button] == true;
		}
    private:
		glm::ivec2 m_Position;
		glm::vec2 m_ScrollWheel;
		bool m_Buttons[MOUSEBUTTON_LAST];
    };
}
