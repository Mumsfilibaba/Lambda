#pragma once
#include "MathCommon.h"

namespace Math
{
	struct Vector2
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

		float& operator[](uint32 index);
		const float& operator[](uint32 index) const;

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
		union
		{
			struct
			{
				float x;
				float y;
			};

			struct
			{
				float r;
				float g;
			};

			struct
			{
				float u;
				float v;
			};
		};

	public:
		static Vector2 Nan();
	};

	inline Vector2::Vector2(float x, float y)
		: x(x),
		y(y)
	{
	}

	inline Vector2::Vector2(const Vector2& other)
		: x(0.0f),
		y(0.0f)
	{
		*this = other;
	}

	inline Vector2& Vector2::Add(const Vector2& other)
	{
		x += other.x;
		y += other.y;

		return *this;
	}

	inline Vector2& Vector2::Add(float scalar)
	{
		x += scalar;
		y += scalar;

		return *this;
	}

	inline Vector2& Vector2::Subtract(const Vector2& other)
	{
		x -= other.x;
		y -= other.y;

		return *this;
	}

	inline Vector2& Vector2::Subtract(float scalar)
	{
		x -= scalar;
		y -= scalar;

		return *this;
	}

	inline Vector2& Vector2::Multiply(float scalar)
	{
		x *= scalar;
		y *= scalar;

		return *this;
	}

	inline Vector2& Vector2::Divide(float scalar)
	{
		x /= scalar;
		y /= scalar;

		return *this;
	}

	//Returns true if inside the interval [0.999999, 1.000001]
	inline bool Vector2::IsUnitVector() const
	{
		float length = Length();
		return (length > 0.999999f && length < 1.000001f);
	}

	inline bool Vector2::Equals(const Vector2& other) const
	{
		return (x == other.x) && (y == other.y);
	}

	inline float Vector2::Dot(const Vector2& other) const
	{
		return (x * other.x) + (y * other.y);
	}

	inline float Vector2::LengthSqrd() const
	{
		return (x * x) + (y * y);
	}

	inline float Vector2::Length() const
	{
		return sqrt(LengthSqrd());
	}

	inline Vector2& Vector2::Normalize()
	{
		float length = Length();

		if (length > 0.0f)
		{
			x /= length;
			y /= length;
		}
		else
		{
			x = 0.0f;
			y = 0.0f;
		}

		return *this;
	}

	inline Vector2 Vector2::UnitVector() const
	{
		return Vector2(*this).Normalize();
	}

	inline Vector2 Vector2::Normal() const
	{
		return Vector2(-y, x);
	}

	inline Vector2 Vector2::Project(const Vector2& other) const
	{
		Vector2 n(other);
		n.Normalize();

		return Dot(n) * n;
	}

	inline Vector2 Vector2::Reflect(const Vector2& normal) const
	{
		return *this - ((2 * Dot(normal)) * normal);
	}

	inline std::string Vector2::ToString() const
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

	inline bool Vector2::operator==(const Vector2& other) const
	{
		return Equals(other);
	}

	inline bool Vector2::operator!=(const Vector2& other) const
	{
		return !Equals(other);
	}

	inline float& Vector2::operator[](uint32 index)
	{
		return reinterpret_cast<float*>(this)[index];
	}

	inline const float& Vector2::operator[](uint32 index) const
	{
		return reinterpret_cast<const float*>(this)[index];
	}

	inline Vector2 Vector2::operator-() const
	{
		return Vector2(-x, -y);
	}

	inline Vector2& Vector2::operator=(const Vector2& other)
	{
		if (this != &other)
		{
			x = other.x;
			y = other.y;
		}

		return *this;
	}

	inline Vector2 operator-(Vector2 left, const Vector2& right)
	{
		return left.Subtract(right);
	}

	inline Vector2 operator+(Vector2 left, const Vector2& right)
	{
		return left.Add(right);
	}

	inline Vector2 operator-(Vector2 left, float right)
	{
		return left.Subtract(right);
	}

	inline Vector2 operator+(Vector2 left, float right)
	{
		return left.Add(right);
	}

	inline Vector2 operator*(Vector2 left, float right)
	{
		return left.Multiply(right);
	}

	inline Vector2 operator/(Vector2 left, float right)
	{
		return left.Divide(right);
	}

	inline Vector2 operator*(float left, Vector2 right)
	{
		return right.Multiply(left);
	}

	inline Vector2& Vector2::operator-=(const Vector2& other)
	{
		return Subtract(other);
	}

	inline Vector2& Vector2::operator+=(const Vector2& other)
	{
		return Add(other);
	}

	inline Vector2& Vector2::operator-=(float scalar)
	{
		return Subtract(scalar);
	}

	inline Vector2& Vector2::operator+=(float scalar)
	{
		return Add(scalar);;
	}

	inline Vector2& Vector2::operator*=(float scalar)
	{
		return Multiply(scalar);;
	}

	inline Vector2& Vector2::operator/=(float scalar)
	{
		return Divide(scalar);
	}

	inline Vector2 Vector2::Nan()
	{
		return Vector2(NAN, NAN);
	}
}