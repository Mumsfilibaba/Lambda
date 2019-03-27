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

	public:
		static const Color BLACK;
		static const Color WHITE;
		static const Color WARMWHITE;
		static const Color GRAY;
		static const Color DARKGRAY;
		static const Color SOFTGRAY;
		static const Color RED;
		static const Color DARKRED;
		static const Color SOFTRED;
		static const Color GREEN;
		static const Color DARKGREEN;
		static const Color SOFTGREEN;
		static const Color BLUE;
		static const Color DARKBLUE;
		static const Color SOFTBLUE;
		static const Color CORNFLOWERBLUE;
		static const Color YELLOW;
		static const Color DARKYELLOW;
		static const Color SOFTYELLOW;
		static const Color MAGENTA;
		static const Color PURPLE;
		static const Color PINK;
	};

	const Color Color::BLACK(0, 0, 0, 255);
	const Color Color::WHITE(255, 255, 255, 255);
	const Color Color::WARMWHITE(255, 240, 224, 255);
	const Color Color::GRAY(128, 128, 128, 255);
	const Color Color::DARKGRAY(64, 64, 64, 255);
	const Color Color::SOFTGRAY(192, 192, 192, 255);
	const Color Color::RED(255, 0, 0, 255);
	const Color Color::DARKRED(128, 0, 0, 255);
	const Color Color::SOFTRED(255, 64, 64, 255);
	const Color Color::GREEN(0, 255, 0, 255);
	const Color Color::SOFTGREEN(64, 255, 64, 255);
	const Color Color::DARKGREEN(0, 128, 0, 255);
	const Color Color::BLUE(0, 0, 255, 255);
	const Color Color::DARKBLUE(0, 0, 128, 255);
	const Color Color::SOFTBLUE(83, 83, 255, 255);
	const Color Color::CORNFLOWERBLUE(100, 149, 237, 255);
	const Color Color::YELLOW(255, 255, 0, 255);
	const Color Color::SOFTYELLOW(255, 255, 64, 255);
	const Color Color::DARKYELLOW(128, 128, 0, 255);
	const Color Color::MAGENTA(255, 0, 255, 255);
	const Color Color::PURPLE(128, 0, 128, 255);
	const Color Color::PINK(255, 192, 203, 255);

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