#pragma once
#include "Float2.h"

namespace Math
{
	struct Rectangle
	{
	public:
		Rectangle();
		Rectangle(const Math::Vec2f& topLeft, const Math::Vec2f& bottomRight);

	public:
		Math::Vec2f TopLeft;
		Math::Vec2f BottomRight;
	};

	inline Rectangle::Rectangle()
		: TopLeft(),
		BottomRight()
	{
	}

	inline Rectangle::Rectangle(const Math::Vec2f& topLeft, const Math::Vec2f& bottomRight)
		: TopLeft(topLeft),
		BottomRight(bottomRight)
	{
	}
}