#pragma once
#include "MathCommon.h"
#include "ColorF.h"

namespace Math
{
	class Color
	{
	public:
		Color(uint8 r = 0, uint8 g = 0, uint8 b = 0, uint8 a = 0);
		Color(const Color& other);
		explicit Color(const ColorF& colorf);
		explicit Color(const Vector4& vector4);

		Color& Add(const Color& other);
		Color& Add(uint8 scalar);
		Color& Subtract(const Color& other);
		Color& Subtract(uint8 scalar);
		Color& Multiply(const Color& other);
		Color& Multiply(uint8 scalar);
		Color& Divide(const Color& other);
		Color& Divide(uint8 scalar);

		bool Equals(const Color& other) const;

		std::string ToString() const;

		bool operator==(const Color& other) const;
		bool operator!=(const Color& other) const;

		Color& operator=(const Vector4& other);
		Color& operator=(const ColorF& other);
		Color& operator=(const Color& other);
		Color& operator+=(const Color& other);
		Color& operator-=(const Color& other);
		Color& operator*=(const Color& other);
		Color& operator/=(const Color& other);
		Color& operator+=(uint8 scalar);
		Color& operator-=(uint8 scalar);
		Color& operator*=(uint8 scalar);
		Color& operator/=(uint8 scalar);

		explicit operator ColorF();

		friend Color operator-(Color left, const Color& right);
		friend Color operator+(Color left, const Color& right);
		friend Color operator*(Color left, const Color& right);
		friend Color operator/(Color left, const Color& right);
		friend Color operator-(Color left, uint8 right);
		friend Color operator+(Color left, uint8 right);
		friend Color operator*(Color left, uint8 right);
		friend Color operator/(Color left, uint8 right);
		friend Color operator*(uint8 left, Color right);

	public:
		union
		{
			struct
			{
				uint8 r;
				uint8 g;
				uint8 b;
				uint8 a;
			};

			uint8 rgba[4];
			uint32 color;
		};

	public:
		const static Color BLACK;
		const static Color WHITE;
		const static Color WARMWHITE;
		const static Color GRAY;
		const static Color DARKGRAY;
		const static Color SOFTGRAY;
		const static Color RED;
		const static Color DARKRED;
		const static Color SOFTRED;
		const static Color GREEN;
		const static Color DARKGREEN;
		const static Color SOFTGREEN;
		const static Color BLUE;
		const static Color DARKBLUE;
		const static Color SOFTBLUE;
		const static Color CORNFLOWERBLUE;
		const static Color YELLOW;
		const static Color DARKYELLOW;
		const static Color SOFTYELLOW;
		const static Color MAGENTA;
		const static Color PURPLE;
		const static Color PINK;
	};
}