/*////////////////////////////////////////////////////////////

Copyright 2018 Alexander Dahlin

Licensed under the Apache License, Version 2.0 (the
"License"); you may not use this file except in
compliance with the License. You may obtain a copy of
the License at

http ://www.apache.org/licenses/LICENSE-2.0

THIS SOFTWARE IS PROVIDED "AS IS". MEANING NO WARRANTY
OR SUPPORT IS PROVIDED OF ANY KIND.

In event of any damages, direct or indirect that can
be traced back to the use of this software, shall no
contributor be held liable. This includes computer
failure and or malfunction of any kind.

////////////////////////////////////////////////////////////*/

#pragma once
#include "MathUtilities.h"

namespace Math
{
	class RAYENGINE_API Point
	{
	public:
		Point(int x = 0, int y = 0);
		Point(const Point& other);

		Point& Add(const Point& other);
		Point& Add(int scalar);
		Point& Subtract(const Point& other);
		Point& Subtract(int scalar);
		Point& Multiply(int scalar);
		Point& Divide(int scalar);

		bool Equals(const Point& other) const;

		std::string ToString() const;

		Point operator-() const;
		Point& operator=(const Point& right);
		Point& operator+=(const Point& right);
		Point& operator-=(const Point& right);
		Point& operator+=(int right);
		Point& operator-=(int right);
		Point& operator*=(int right);
		Point& operator/=(int right);

		bool operator==(const Point& other) const;
		bool operator!=(const Point& other) const;


		friend Point operator+(Point left, const Point& right);
		friend Point operator-(Point left, const Point& right);
		friend Point operator+(Point left, int right);
		friend Point operator-(Point left, int right);
		friend Point operator*(Point left, int right);
		friend Point operator/(Point left, int right);
		friend Point operator*(int left, Point right);

	public:
		int x;
		int y;
	};
}