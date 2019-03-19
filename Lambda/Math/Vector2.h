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
#include "MathCommon.h"

namespace Math
{
	class RAYENGINE_API Vector2
	{
	public:
		Vector2(float x = 0.0f, float y = 0.0f);
		Vector2(const Vector2& other);

		Vector2& Add(const Vector2& other);
		Vector2& Add(float scalar);
		Vector2& Subtract(const Vector2& other);
		Vector2& Subtract(float scalar);
		Vector2& Multiply(float scalar);
		Vector2& Divide(float scalar);

		bool IsUnitVector() const;
		bool Equals(const Vector2& other) const;

		float Dot(const Vector2& other) const;
		float LengthSqrd() const;
		float Length() const;
		Vector2& Normalize();
		Vector2 UnitVector() const;
		Vector2 Normal() const;
		Vector2 Project(const Vector2& other) const;
		Vector2 Reflect(const Vector2& normal) const;

		std::string ToString() const;

		bool operator==(const Vector2& other) const;
		bool operator!=(const Vector2& other) const;

		Vector2 operator-() const;
		Vector2& operator=(const Vector2& other);
		Vector2& operator-=(const Vector2& other);
		Vector2& operator+=(const Vector2& other);
		Vector2& operator-=(float scalar);
		Vector2& operator+=(float scalar);
		Vector2& operator*=(float scalar);
		Vector2& operator/=(float scalar);

		friend Vector2 operator-(Vector2 left, const Vector2& right);
		friend Vector2 operator+(Vector2 left, const Vector2& right);
		friend Vector2 operator-(Vector2 left, float right);
		friend Vector2 operator+(Vector2 left, float right);
		friend Vector2 operator*(Vector2 left, float right);
		friend Vector2 operator/(Vector2 left, float right);
		friend Vector2 operator*(float left, Vector2 right);

	public:
		static Vector2 Nan();

	public:
		union
		{
			struct
			{
				float x;
				float y;
			};

			float v[2];
		};
	};
}