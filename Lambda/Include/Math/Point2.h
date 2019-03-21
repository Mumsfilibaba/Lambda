#pragma once
#include "MathCommon.h"

namespace Math
{
	struct Point2
	{
	public:
		Point2(int32 x = 0, int32 y = 0);
		Point2(const Point2& other);

		Point2& Add(const Point2& other);
		Point2& Add(int32 scalar);
		Point2& Subtract(const Point2& other);
		Point2& Subtract(int32 scalar);
		Point2& Multiply(int32 scalar);
		Point2& Divide(int32 scalar);

		bool Equals(const Point2& other) const;

		std::string ToString() const;

		Point2 operator-() const;
		Point2& operator=(const Point2& right);
		Point2& operator+=(const Point2& right);
		Point2& operator-=(const Point2& right);
		Point2& operator+=(int32 right);
		Point2& operator-=(int32 right);
		Point2& operator*=(int32 right);
		Point2& operator/=(int32 right);

		bool operator==(const Point2& other) const;
		bool operator!=(const Point2& other) const;


		friend Point2 operator+(Point2 left, const Point2& right);
		friend Point2 operator-(Point2 left, const Point2& right);
		friend Point2 operator+(Point2 left, int32 right);
		friend Point2 operator-(Point2 left, int32 right);
		friend Point2 operator*(Point2 left, int32 right);
		friend Point2 operator/(Point2 left, int32 right);
		friend Point2 operator*(int32 int32, Point2 right);

	public:
		int32 x;
		int32 y;
	};

	inline Point2::Point2(int32 x, int32 y)
		: x(x),
		y(y)
	{
	}

	inline Point2::Point2(const Point2& other)
		: x(0),
		y(0)
	{
		*this = other;
	}

	inline Point2& Point2::Add(const Point2& other)
	{
		x += other.x;
		y += other.y;

		return *this;
	}

	inline Point2& Point2::Add(int32 scalar)
	{
		x += scalar;
		y += scalar;

		return *this;
	}

	inline Point2& Point2::Subtract(const Point2& other)
	{
		x -= other.x;
		y -= other.y;

		return *this;
	}

	inline Point2& Point2::Subtract(int32 scalar)
	{
		x -= scalar;
		y -= scalar;

		return *this;
	}

	inline Point2& Point2::Multiply(int32 scalar)
	{
		x *= scalar;
		y *= scalar;

		return *this;
	}

	inline Point2& Point2::Divide(int32 scalar)
	{
		x /= scalar;
		y /= scalar;

		return *this;
	}

	inline bool Point2::Equals(const Point2 & other) const
	{
		return (x == other.x) && (y == other.y);
	}

	inline std::string Point2::ToString() const
	{
#if defined(_ANDROID)
		std::stringstream stream;
		stream << '[' << x << ", " << y << ']';
		return stream.str();
#else
		using namespace std;
		return string('[' + to_string(x) + ", " + to_string(y) + ']');
#endif
	}

	inline Point2 Point2::operator-() const
	{
		return Point2(-x, -y);
	}

	inline Point2& Point2::operator=(const Point2& other)
	{
		if (this != &other)
		{
			x = other.x;
			y = other.y;
		}

		return *this;
	}

	inline Point2& Point2::operator+=(const Point2& right)
	{
		return Add(right);
	}

	inline Point2& Point2::operator-=(const Point2& right)
	{
		return Subtract(right);
	}

	inline Point2& Point2::operator+=(int32 right)
	{
		return Add(right);
	}

	inline Point2& Point2::operator-=(int32 right)
	{
		return Subtract(right);
	}
	
	inline Point2& Point2::operator*=(int32 right)
	{
		return Multiply(right);
	}
	
	inline Point2& Point2::operator/=(int32 right)
	{
		return Divide(right);
	}
	
	inline bool Point2::operator==(const Point2& other) const
	{
		return Equals(other);
	}

	inline bool Point2::operator!=(const Point2& other) const
	{
		return !Equals(other);
	}
	
	inline Point2 operator+(Point2 left, const Point2& right)
	{
		return left.Add(right);
	}
	
	inline Point2 operator-(Point2 left, const Point2& right)
	{
		return left.Subtract(right);
	}
	
	inline Point2 operator+(Point2 left, int32 right)
	{
		return left.Add(right);
	}
	
	inline Point2 operator-(Point2 left, int32 right)
	{
		return left.Subtract(right);
	}
	
	inline Point2 operator*(Point2 left, int32 right)
	{
		return left.Multiply(right);
	}

	inline Point2 operator*(int32 left, Point2 right)
	{
		return right.Multiply(left);
	}

	inline Point2 operator/(Point2 left, int32 right)
	{
		return left.Divide(right);
	}
}