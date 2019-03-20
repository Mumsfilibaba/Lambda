#pragma once
#include "Vector4.h"

namespace Math
{
	class ColorF
	{
	public:
		ColorF(float r = 0.0f, float g = 0.0f, float b = 0.0f, float a = 1.0f);
		ColorF(const ColorF& other);
		explicit ColorF(const Vector4& vector4);

		ColorF& Add(const ColorF& other);
		ColorF& Add(float scalar);
		ColorF& Subtract(const ColorF& other);
		ColorF& Subtract(float scalar);
		ColorF& Multiply(const ColorF& other);
		ColorF& Multiply(float scalar);
		ColorF& Divide(const ColorF& other);
		ColorF& Divide(float scalar);

		ColorF& Lerp(const ColorF& src, float delta);
		ColorF Lerp(const ColorF& src, float delta) const;

		bool Equals(const ColorF& other) const;

		std::string ToString() const;

		bool operator==(const ColorF& other) const;
		bool operator!=(const ColorF& other) const;

		ColorF& operator=(const Vector4& other);
		ColorF& operator=(const ColorF& other);
		ColorF& operator+=(const ColorF& other);
		ColorF& operator-=(const ColorF& other);
		ColorF& operator*=(const ColorF& other);
		ColorF& operator/=(const ColorF& other);
		ColorF& operator+=(float scalar);
		ColorF& operator-=(float scalar);
		ColorF& operator*=(float scalar);
		ColorF& operator/=(float scalar);

		operator float*();
		operator const float*() const;
		explicit operator Vector4&();
		explicit operator const Vector4&() const;

		friend ColorF operator-(ColorF left, const ColorF& right);
		friend ColorF operator+(ColorF left, const ColorF& right);
		friend ColorF operator*(ColorF left, const ColorF& right);
		friend ColorF operator/(ColorF left, const ColorF& right);
		friend ColorF operator-(ColorF left, float right);
		friend ColorF operator+(ColorF left, float right);
		friend ColorF operator*(ColorF left, float right);
		friend ColorF operator/(ColorF left, float right);
		friend ColorF operator*(float left, ColorF right);

	public:
		union
		{
			struct
			{
				float r;
				float g;
				float b;
				float a;
			};

			float rgba[4];
			Vector4 vector;
		};

	public:
		const static ColorF BLACK;
		const static ColorF WHITE;
		const static ColorF WARMWHITE;
		const static ColorF GRAY;
		const static ColorF DARKGRAY;
		const static ColorF SOFTGRAY;
		const static ColorF RED;
		const static ColorF DARKRED;
		const static ColorF SOFTRED;
		const static ColorF GREEN;
		const static ColorF DARKGREEN;
		const static ColorF SOFTGREEN;
		const static ColorF BLUE;
		const static ColorF DARKBLUE;
		const static ColorF SOFTBLUE;
		const static ColorF CORNFLOWERBLUE;
		const static ColorF YELLOW;
		const static ColorF DARKYELLOW;
		const static ColorF SOFTYELLOW;
		const static ColorF MAGENTA;
		const static ColorF PURPLE;
		const static ColorF PINK;
	};

	const ColorF ColorF::BLACK(0.0f, 0.0f, 0.0f, 1.0f);
	const ColorF ColorF::WHITE(1.0f, 1.0f, 1.0f, 1.0f);
	const ColorF ColorF::WARMWHITE(1.0f, 0.945f, 0.88f, 1.0f);
	const ColorF ColorF::GRAY(0.502f, 0.502f, 0.502f, 1.0f);
	const ColorF ColorF::DARKGRAY(0.251f, 0.251f, 0.251f, 1.0f);
	const ColorF ColorF::SOFTGRAY(0.753f, 0.753f, 0.753f, 1.0f);
	const ColorF ColorF::RED(1.0f, 0.0f, 0.0f, 1.0f);
	const ColorF ColorF::DARKRED(0.502f, 0.0f, 0.0f, 1.0f);
	const ColorF ColorF::SOFTRED(1.0f, 0.251f, 0.251f, 1.0f);
	const ColorF ColorF::GREEN(0.0f, 1.0f, 0.0f, 1.0f);
	const ColorF ColorF::SOFTGREEN(0.251f, 1.0f, 0.251f, 1.0f);
	const ColorF ColorF::DARKGREEN(0.0f, 0.502f, 0.0f, 1.0f);
	const ColorF ColorF::BLUE(0.0f, 0.0f, 1.0f, 1.0f);
	const ColorF ColorF::DARKBLUE(0.0f, 0.0f, 0.502f, 1.0f);
	const ColorF ColorF::SOFTBLUE(0.325f, 0.325f, 1.0f, 1.0f);
	const ColorF ColorF::CORNFLOWERBLUE(0.392f, 0.584f, 0.929f, 1.0f);
	const ColorF ColorF::YELLOW(1.0f, 1.0f, 0.0f, 1.0f);
	const ColorF ColorF::SOFTYELLOW(1.0f, 1.0f, 0.251f, 1.0f);
	const ColorF ColorF::DARKYELLOW(0.502f, 0.502f, 0.0f, 1.0f);
	const ColorF ColorF::MAGENTA(1.0f, 0.0f, 1.0f, 1.0f);
	const ColorF ColorF::PURPLE(0.502f, 0.0f, 0.502f, 1.0f);
	const ColorF ColorF::PINK(1.0f, 0.753f, 0.796f, 1.0f);

	ColorF::ColorF(float r, float g, float b, float a)
		: r(r),
		g(g),
		b(b),
		a(a)
	{
	}

	ColorF::ColorF(const ColorF& other)
		: r(0.0f),
		g(0.0f),
		b(0.0f),
		a(0.0f)
	{
		*this = other;
	}

	ColorF::ColorF(const Math::Vector4& vector4)
		: r(0.0f),
		g(0.0f),
		b(0.0f),
		a(0.0f)
	{
		*this = vector4;
	}

	ColorF& ColorF::Add(const ColorF& other)
	{
		r = r + other.r;
		g = g + other.g;
		b = b + other.b;
		a = a + other.a;

		return *this;
	}

	ColorF& ColorF::Add(float scalar)
	{
		r = r + scalar;
		g = g + scalar;
		b = b + scalar;
		a = a + scalar;

		return *this;
	}

	ColorF& ColorF::Subtract(const ColorF& other)
	{
		r = r - other.r;
		g = g - other.g;
		b = b - other.b;
		a = a - other.a;

		return *this;
	}

	ColorF& ColorF::Subtract(float scalar)
	{
		r = r - scalar;
		g = g - scalar;
		b = b - scalar;
		a = a - scalar;

		return *this;
	}

	ColorF& ColorF::Multiply(const ColorF& other)
	{
		r = r * other.r;
		g = g * other.g;
		b = b * other.b;
		a = a * other.a;

		return *this;
	}

	ColorF& ColorF::Multiply(float scalar)
	{
		r = r * scalar;
		g = g * scalar;
		b = b * scalar;
		a = a * scalar;

		return *this;
	}

	ColorF& ColorF::Divide(const ColorF& other)
	{
		r = r / other.r;
		g = g / other.g;
		b = b / other.b;
		a = a / other.a;

		return *this;
	}

	ColorF& ColorF::Divide(float scalar)
	{
		r = r / scalar;
		g = g / scalar;
		b = b / scalar;
		a = a / scalar;

		return *this;
	}

	ColorF& ColorF::Lerp(const ColorF& src, float delta)
	{
		r = r + (src.r - r) * delta;
		g = g + (src.g - g) * delta;
		b = b + (src.b - b) * delta;
		a = a + (src.a - a) * delta;

		return *this;
	}

	ColorF ColorF::Lerp(const ColorF& src, float delta) const
	{
		return ColorF(*this).Lerp(src, delta);
	}

	bool ColorF::Equals(const ColorF& other) const
	{
		return (r == other.r && g == other.g && b == other.b && a == other.a);
	}

	std::string ColorF::ToString() const
	{
		using namespace std;
		return string("[ColorF][" + to_string(r) + ", " + to_string(g) + ", " + to_string(b) + ", " + to_string(a) + ']');
	}

	bool ColorF::operator==(const ColorF& other) const
	{
		return Equals(other);
	}

	bool ColorF::operator!=(const ColorF& other) const
	{
		return !Equals(other);
	}

	ColorF& ColorF::operator=(const Math::Vector4& other)
	{
		r = other.x;
		g = other.y;
		b = other.z;
		a = other.w;

		return *this;
	}

	ColorF& ColorF::operator=(const ColorF& other)
	{
		r = other.r;
		g = other.g;
		b = other.b;
		a = other.a;

		return *this;
	}

	ColorF& ColorF::operator+=(const ColorF& other)
	{
		return Add(other);
	}

	ColorF& ColorF::operator-=(const ColorF& other)
	{
		return Subtract(other);
	}

	ColorF& ColorF::operator*=(const ColorF& other)
	{
		return Multiply(other);
	}

	ColorF& ColorF::operator/=(const ColorF& other)
	{
		return Divide(other);
	}

	ColorF& ColorF::operator+=(float scalar)
	{
		return Add(scalar);
	}

	ColorF& ColorF::operator-=(float scalar)
	{
		return Subtract(scalar);
	}

	ColorF& ColorF::operator*=(float scalar)
	{
		return Multiply(scalar);
	}

	ColorF& ColorF::operator/=(float scalar)
	{
		return Divide(scalar);
	}

	ColorF::operator const float*() const
	{
		return rgba;
	}

	ColorF::operator float*()
	{
		return rgba;
	}

	ColorF::operator Math::Vector4&()
	{
		return vector;
	}

	ColorF::operator const Math::Vector4&() const
	{
		return vector;
	}

	ColorF operator-(ColorF left, const ColorF& right)
	{
		return left.Subtract(right);
	}

	ColorF operator+(ColorF left, const ColorF& right)
	{
		return left.Add(right);
	}

	ColorF operator*(ColorF left, const ColorF& right)
	{
		return left.Multiply(right);
	}

	ColorF operator/(ColorF left, const ColorF& right)
	{
		return left.Divide(right);
	}

	ColorF operator-(ColorF left, float right)
	{
		return left.Subtract(right);
	}

	ColorF operator+(ColorF left, float right)
	{
		return left.Add(right);
	}

	ColorF operator*(ColorF left, float right)
	{
		return left.Multiply(right);
	}

	ColorF operator/(ColorF left, float right)
	{
		return left.Divide(right);
	}

	ColorF operator*(float left, ColorF right)
	{
		return right.Multiply(left);
	}
}