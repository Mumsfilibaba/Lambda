#pragma once
#include "MathCommon.h"
#include "Float4.h"

namespace Math
{
	struct Color
	{
	public:
		explicit Color(uint8 rgba = 255);
		explicit Color(uint8 r, uint8 g, uint8 b, uint8 a);
		explicit Color(const Vec4f& vector4);
		Color(const Color& other);

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

		Color& operator=(const Vec4f& other);
		Color& operator=(const Color& other);
		Color& operator+=(const Color& other);
		Color& operator-=(const Color& other);
		Color& operator*=(const Color& other);
		Color& operator/=(const Color& other);
		Color& operator+=(uint8 scalar);
		Color& operator-=(uint8 scalar);
		Color& operator*=(uint8 scalar);
		Color& operator/=(uint8 scalar);

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

			uint32 Col;
		};
	};

	namespace Colors
	{
		const Color BLACK = Color(0, 0, 0, 255);
		const Color WHITE = Color(255, 255, 255, 255);
		const Color WARMWHITE = Color(255, 240, 224, 255);
		const Color GRAY = Color(128, 128, 128, 255);
		const Color DARKGRAY = Color(64, 64, 64, 255);
		const Color SOFTGRAY = Color(192, 192, 192, 255);
		const Color RED = Color(255, 0, 0, 255);
		const Color DARKRED = Color(128, 0, 0, 255);
		const Color SOFTRED = Color(255, 64, 64, 255);
		const Color GREEN = Color(0, 255, 0, 255);
		const Color SOFTGREEN = Color(64, 255, 64, 255);
		const Color DARKGREEN = Color(0, 128, 0, 255);
		const Color BLUE = Color(0, 0, 255, 255);
		const Color DARKBLUE = Color(0, 0, 128, 255);
		const Color SOFTBLUE = Color(83, 83, 255, 255);
		const Color CORNFLOWERBLUE = Color(100, 149, 237, 255);
		const Color YELLOW = Color(255, 255, 0, 255);
		const Color SOFTYELLOW = Color(255, 255, 64, 255);
		const Color DARKYELLOW = Color(128, 128, 0, 255);
		const Color MAGENTA = Color(255, 0, 255, 255);
		const Color PURPLE = Color(128, 0, 128, 255);
		const Color PINK = Color(255, 192, 203, 255);
	}

	inline Color::Color(uint8 rgba)
		: r(rgba),
		g(rgba),
		b(rgba),
		a(rgba)
	{
	}

	inline Color::Color(uint8 r, uint8 g, uint8 b, uint8 a)
		: r(r),
		g(g),
		b(b),
		a(a)
	{
	}

	inline Color::Color(const Color& other)
	{
		*this = other;
	}

	inline Color::Color(const Vec4f& vector4)
	{
		*this = vector4;
	}

	inline Color& Color::Add(const Color& other)
	{
		r += other.r;
		g += other.g;
		b += other.b;
		a += other.a;

		return *this;
	}

	inline Color& Color::Add(uint8 scalar)
	{
		r += scalar;
		g += scalar;
		b += scalar;
		a += scalar;

		return *this;
	}

	inline Color& Color::Subtract(const Color& other)
	{
		r -= other.r;
		g -= other.g;
		b -= other.b;
		a -= other.a;

		return *this;
	}

	inline Color& Color::Subtract(uint8 scalar)
	{
		r -= scalar;
		g -= scalar;
		b -= scalar;
		a -= scalar;

		return *this;
	}

	inline Color& Color::Multiply(const Color& other)
	{
		r *= other.r;
		g *= other.g;
		b *= other.b;
		a *= other.a;

		return *this;
	}

	inline Color& Color::Multiply(uint8 scalar)
	{
		r *= scalar;
		g *= scalar;
		b *= scalar;
		a *= scalar;

		return *this;
	}

	inline Color& Color::Divide(const Color& other)
	{
		r /= other.r;
		g /= other.g;
		b /= other.b;
		a /= other.a;

		return *this;
	}

	inline Color& Color::Divide(uint8 scalar)
	{
		r /= scalar;
		g /= scalar;
		b /= scalar;
		a /= scalar;

		return *this;
	}

	inline bool Color::Equals(const Color& other) const
	{
		return (Col == other.Col);
	}

	inline std::string Color::ToString() const
	{
		using namespace std;
		return string("[Color][" + to_string(r) + ", " + to_string(g) + ", " + to_string(b) + ", " + to_string(a) + ']');
	}

	inline bool Color::operator==(const Color& other) const
	{
		return Equals(other);
	}

	inline bool Color::operator!=(const Color& other) const
	{
		return !Equals(other);
	}
	
	inline Color& Color::operator=(const Math::Vec4f& other)
	{
		r = (uint8)(other.r * 255);
		g = (uint8)(other.g * 255);
		b = (uint8)(other.b * 255);
		a = (uint8)(other.a * 255);

		return *this;
	}

	inline Color& Color::operator=(const Color& other)
	{
		if (this != &other)
			Col = other.Col;

		return *this;
	}

	inline Color& Color::operator+=(const Color& other)
	{
		return Add(other);
	}

	inline Color& Color::operator-=(const Color& other)
	{
		return Subtract(other);
	}
	
	inline Color& Color::operator*=(const Color& other)
	{
		return Multiply(other);
	}

	inline Color& Color::operator/=(const Color& other)
	{
		return Divide(other);
	}

	inline Color& Color::operator+=(uint8 scalar)
	{
		return Add(scalar);
	}

	inline Color& Color::operator-=(uint8 scalar)
	{
		return Subtract(scalar);
	}
	
	inline Color& Color::operator*=(uint8 scalar)
	{
		return Multiply(scalar);
	}
	
	inline Color& Color::operator/=(uint8 scalar)
	{
		return Divide(scalar);
	}

	inline Color operator-(Color left, const Color& right)
	{
		return left.Subtract(right);
	}

	inline Color operator+(Color left, const Color& right)
	{
		return left.Add(right);
	}

	inline Color operator*(Color left, const Color& right)
	{
		return left.Multiply(right);
	}

	inline Color operator/(Color left, const Color& right)
	{
		return left.Divide(right);
	}

	inline Color operator-(Color left, uint8 right)
	{
		return left.Subtract(right);
	}

	inline Color operator+(Color left, uint8 right)
	{
		return left.Add(right);
	}
	
	inline Color operator*(Color left, uint8 right)
	{
		return left.Multiply(right);
	}

	inline Color operator/(Color left, uint8 right)
	{
		return left.Divide(right);
	}
	
	inline Color operator*(uint8 left, Color right)
	{
		return right.Multiply(left);
	}
}