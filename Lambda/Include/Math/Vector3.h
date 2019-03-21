#pragma once
#include "MathCommon.h"

namespace Math
{
	struct Vector3
	{
	public:
		Vector3(float x = 0.0f, float y = 0.0f, float z = 0.0f);
		Vector3(const Vector3& other);

		Vector3& Add(const Vector3& other);
		Vector3& Add(float scalar);
		Vector3& Subtract(const Vector3& other);
		Vector3& Subtract(float scalar);
		Vector3& Multiply(float scalar);
		Vector3& Divide(float scalar);

		bool IsUnitVector() const;
		bool Equals(const Vector3& other) const;

		float Dot(const Vector3& other) const;
		float LengthSqrd() const;
		float Length() const;
		Vector3& Normalize();
		Vector3 UnitVector() const;
		Vector3 Cross(const Vector3& other) const;
		Vector3 Project(const Vector3& other) const;
		Vector3 Reflect(const Vector3& normal) const;

		std::string ToString() const;

		Vector3 operator-() const;
		Vector3& operator=(const Vector3& other);
		Vector3& operator-=(const Vector3& other);
		Vector3& operator+=(const Vector3& other);
		Vector3& operator-=(float other);
		Vector3& operator+=(float other);
		Vector3& operator*=(float other);
		Vector3& operator/=(float other);

		bool operator==(const Vector3& other) const;
		bool operator!=(const Vector3& other) const;

		float& operator[](uint32 index);
		const float& operator[](uint32 index) const;

		friend Vector3 operator-(Vector3 left, const Vector3& right);
		friend Vector3 operator+(Vector3 left, const Vector3& right);
		friend Vector3 operator-(Vector3 left, float right);
		friend Vector3 operator+(Vector3 left, float right);
		friend Vector3 operator*(Vector3 left, float right);
		friend Vector3 operator/(Vector3 left, float right);
		friend Vector3 operator*(float left, Vector3 right);

	public:
		union
		{
			struct
			{
				float x;
				float y;
				float z;
			};

			struct
			{
				float r;
				float g;
				float b;
			};
		};

	public:
		static Vector3 Nan();

	public:
		static const Vector3 UP;
		static const Vector3 FORWARD;
		static const Vector3 RIGHT;
	};

	static const Vector3 UP = Vector3(0.0f, 1.0f, 0.0f);
	static const Vector3 FORWARD = Vector3(0.0f, 0.0f, 1.0f);
	static const Vector3 RIGHT = Vector3(1.0f, 0.0f, 0.0f);

	inline Vector3::Vector3(float x, float y, float z)
		: x(x),
		y(y),
		z(z)
	{
	}

	inline Vector3::Vector3(const Vector3& other)
		: x(0.0f),
		y(0.0f),
		z(0.0f)
	{
		*this = other;
	}

	inline Vector3& Vector3::Add(const Vector3& other)
	{
		x += other.x;
		y += other.y;
		z += other.z;

		return *this;
	}

	inline Vector3& Vector3::Add(float scalar)
	{
		x += scalar;
		y += scalar;
		z += scalar;

		return *this;
	}

	inline Vector3& Vector3::Subtract(const Vector3& other)
	{
		x -= other.x;
		y -= other.y;
		z -= other.z;

		return *this;
	}

	inline Vector3& Vector3::Subtract(float scalar)
	{
		x -= scalar;
		y -= scalar;
		z -= scalar;

		return *this;
	}

	inline Vector3& Vector3::Multiply(float scalar)
	{
		x *= scalar;
		y *= scalar;
		z *= scalar;

		return *this;
	}

	inline Vector3& Vector3::Divide(float scalar)
	{
		x /= scalar;
		y /= scalar;
		z /= scalar;

		return *this;
	}

	//Returns true if inside the interval [0.999999, 1.000001]
	inline bool Vector3::IsUnitVector() const
	{
		float length = Length();
		return (length > 0.999999f && length < 1.000001f);
	}

	inline bool Vector3::Equals(const Vector3& other) const
	{
		return (x == other.x && y == other.y && z == other.z);
	}

	inline float Vector3::Dot(const Vector3& other) const
	{
		return (x * other.x) + (y * other.y) + (z * other.z);
	}

	inline float Vector3::LengthSqrd() const
	{
		return (x * x) + (y * y) + (z * z);
	}

	inline float Vector3::Length() const
	{
		return sqrt(LengthSqrd());
	}

	inline Vector3& Vector3::Normalize()
	{
		float length = Length();
		if (length > 0.0f)
		{
			x /= length;
			y /= length;
			z /= length;
		}
		else
		{
			x = 0.0f;
			y = 0.0f;
			z = 0.0f;
		}

		return *this;
	}

	inline Vector3 Vector3::UnitVector() const
	{
		return Vector3(*this).Normalize();
	}

	inline Vector3 Vector3::Cross(const Vector3& other) const
	{
		return Vector3(
			(y * other.z) - (other.y * z),
			(z * other.x) - (other.z * x),
			(x * other.y) - (other.x * y));
	}

	inline Vector3 Vector3::Project(const Vector3& other) const
	{
		Vector3 n(other);
		n.Normalize();

		return Dot(n) * n;
	}

	inline Vector3 Vector3::Reflect(const Vector3& normal) const
	{
		return *this - ((2 * Dot(normal)) * normal);;
	}

	inline std::string Vector3::ToString() const
	{
#if defined(_ANDROID)
		std::stringstream stream;
		stream << '[' << x << ", " << y << ", " << z << ']';
		return stream.str();
#else
		using namespace std;
		return string('[' + to_string(x) + ", " + to_string(y) + ", " + to_string(z) + ']');
#endif
	}

	inline Vector3 operator-(Vector3 left, const Vector3& right)
	{
		return left.Subtract(right);
	}

	inline Vector3 operator+(Vector3 left, const Vector3& right)
	{
		return left.Add(right);
	}

	inline Vector3 operator-(Vector3 left, float right)
	{
		return left.Subtract(right);
	}

	inline Vector3 operator+(Vector3 left, float right)
	{
		return left.Add(right);
	}

	inline Vector3 operator*(Vector3 left, float right)
	{
		return left.Multiply(right);
	}

	inline Vector3 operator/(Vector3 left, float right)
	{
		return left.Divide(right);
	}

	inline Vector3 operator*(float left, Vector3 right)
	{
		return right.Multiply(left);
	}

	inline Vector3 Vector3::operator-() const
	{
		return Vector3(-x, -y, -z);
	}

	inline Vector3& Vector3::operator=(const Vector3& other)
	{
		if (this != &other)
		{
			x = other.x;
			y = other.y;
			z = other.z;
		}

		return *this;
	}

	inline Vector3& Vector3::operator-=(const Vector3& other)
	{
		return Subtract(other);
	}

	inline Vector3& Vector3::operator+=(const Vector3& other)
	{
		return Add(other);
	}

	inline Vector3& Vector3::operator-=(float scalar)
	{
		return Subtract(scalar);
	}

	inline Vector3& Vector3::operator+=(float scalar)
	{
		return Add(scalar);
	}

	inline Vector3& Vector3::operator*=(float scalar)
	{
		return Multiply(scalar);
	}

	inline Vector3& Vector3::operator/=(float scalar)
	{
		return Divide(scalar);
	}

	inline bool Vector3::operator==(const Vector3& other) const
	{
		return Equals(other);
	}

	inline bool Vector3::operator!=(const Vector3& other) const
	{
		return !Equals(other);
	}

	inline float& Vector3::operator[](uint32 index)
	{
		return reinterpret_cast<float*>(this)[index];
	}

	inline const float& Vector3::operator[](uint32 index) const
	{
		return reinterpret_cast<const float*>(this)[index];
	}

	inline Vector3 Vector3::Nan()
	{
		return Vector3(NAN, NAN, NAN);
	}
}