#pragma once
#include "Vector2.h"

namespace Math
{
	struct Rectangle
	{
	public:
		Rectangle();
		Rectangle(const Math::Vector2& topLeft, const Math::Vector2& bottomRight);

	public:
		Math::Vector2 TopLeft;
		Math::Vector2 BottomRight;
	};

	inline Rectangle::Rectangle()
		: TopLeft(),
		BottomRight()
	{
	}

	inline Rectangle::Rectangle(const Math::Vector2& topLeft, const Math::Vector2& bottomRight)
		: TopLeft(topLeft),
		BottomRight(bottomRight)
	{
	}
}