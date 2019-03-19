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

#include "RayEngine.h"
#include "../../Include/Math/Vector4.h"

#if defined(__ANDROID__)
#include <sstream>
#endif

namespace Math
{
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	Vector4::Vector4(float x, float y, float z, float w)
		: x(x),
		y(y),
		z(z),
		w(w)
	{
	}


	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	Vector4::Vector4(const Vector4& other)
	{
		*this = other;
	}


	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	Vector4::Vector4(const Vector3& vector3)
	{
		*this = vector3;
	}


	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	Vector4& Vector4::Add(const Vector4& other)
	{
#if defined(SSE_INTRIN)
		sse128 = _mm_add_ps(sse128, other.sse128);
#else
		x += other.x;
		y += other.y;
		z += other.z;
		w += other.w;
#endif
		return *this;
	}


	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	Vector4& Vector4::Add(float scalar)
	{
#if defined(SSE_INTRIN)
		sse128 = _mm_add_ps(sse128, _mm_set_ps1(scalar));
#else
		x += scalar;
		y += scalar;
		z += scalar;
		w += scalar;
#endif
		return *this;
	}


	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	Vector4& Vector4::Subtract(const Vector4& other)
	{
#if defined(SSE_INTRIN)
		sse128 = _mm_sub_ps(sse128, other.sse128);
#else
		x -= other.x;
		y -= other.y;
		z -= other.z;
		w -= other.w;
#endif

		return *this;
	}


	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	Vector4& Vector4::Subtract(float scalar)
	{
#if defined(SSE_INTRIN)
		sse128 = _mm_sub_ps(sse128, _mm_set_ps1(scalar));
#else
		x -= scalar;
		y -= scalar;
		z -= scalar;
		w -= scalar;
#endif
		return *this;
	}


	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	Vector4& Vector4::Multiply(float scalar)
	{
#if defined(SSE_INTRIN)
		sse128 = _mm_mul_ps(sse128, _mm_set_ps1(scalar));
#else
		x *= scalar;
		y *= scalar;
		z *= scalar;
		w *= scalar;
#endif
		return *this;
	}


	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	Vector4& Vector4::Divide(float scalar)
	{
#if defined(SSE_INTRIN)
		sse128 = _mm_div_ps(sse128, _mm_set_ps1(scalar));
#else
		x /= scalar;
		y /= scalar;
		z /= scalar;
		w /= scalar;
#endif
		return *this;
	}


	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	bool Vector4::IsUnitVector() const
	{
		float length = Length();
		//To fix rounding errors
		return length > 0.999999f && length < 1.000001;
	}


	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	bool Vector4::Equals(const Vector4& other) const
	{
		return (x == other.x && y == other.y && z == other.z && w == other.w);
	}


	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	float Vector4::Dot(const Vector4& other) const
	{
#if defined(SSE_INTRIN)
		__m128 summed = _mm_mul_ps(sse128, other.sse128);
		__m128 shuffled = _mm_shuffle_ps(summed, summed, _MM_SHUFFLE(2, 3, 0, 1));
		summed = _mm_add_ps(summed, shuffled);
		shuffled = _mm_movehl_ps(shuffled, summed);
		summed = _mm_add_ss(summed, shuffled);
		return _mm_cvtss_f32(summed);
#else
		return (x * other.x) + (y * other.y) + (z * other.z) + (w * other.w);
#endif
	}


	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	float Vector4::LengthSqrd() const
	{
#if defined(SSE_INTRIN)
		__m128 summed = _mm_mul_ps(sse128, sse128);
		__m128 shuffled = _mm_shuffle_ps(summed, summed, _MM_SHUFFLE(2, 3, 0, 1));
		summed = _mm_add_ps(summed, shuffled);
		shuffled = _mm_movehl_ps(shuffled, summed);
		return _mm_cvtss_f32(_mm_add_ss(summed, shuffled));
#else
		return (x * x) + (y * y) + (z * z) + (w * w);
#endif
	}


	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	float Vector4::Length() const
	{
#if defined(SSE_INTRIN)
		__m128 summed = _mm_mul_ps(sse128, sse128);
		__m128 shuffled = _mm_shuffle_ps(summed, summed, _MM_SHUFFLE(2, 3, 0, 1));
		summed = _mm_add_ps(summed, shuffled);
		shuffled = _mm_movehl_ps(shuffled, summed);
		return _mm_cvtss_f32(_mm_sqrt_ss(_mm_add_ss(summed, shuffled)));
#else
		return sqrt(LengthSqrd());
#endif
	}


	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	Vector4& Vector4::Normalize()
	{
#if defined(SSE_INTRIN)
		__m128 summed = _mm_mul_ps(sse128, sse128);
		__m128 shuffled = _mm_shuffle_ps(summed, summed, _MM_SHUFFLE(2, 3, 0, 1));
		summed = _mm_add_ps(summed, shuffled);
		shuffled = _mm_movehl_ps(shuffled, summed);
		summed = _mm_add_ss(summed, shuffled);
		sse128 = _mm_mul_ps(sse128, _mm_rsqrt_ps(_mm_shuffle_ps(summed, summed, 0)));
#else
		float length = Length();

		if (length > 0)
			Divide(length);
		else
			memset(this, 0, sizeof(Vector4));
#endif
		return *this;
	}


	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	Vector4 Vector4::UnitVector() const
	{
#if defined(SSE_INTRIN)
		__m128 summed = _mm_mul_ps(sse128, sse128);
		__m128 shuffled = _mm_shuffle_ps(summed, summed, _MM_SHUFFLE(2, 3, 0, 1));
		summed = _mm_add_ps(summed, shuffled);
		shuffled = _mm_movehl_ps(shuffled, summed);
		summed = _mm_add_ss(summed, shuffled);
		return _mm_mul_ps(sse128, _mm_rsqrt_ps(_mm_shuffle_ps(summed, summed, 0)));
#else
		return Vector4(*this).Normalize();
#endif
	}


	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	Vector4 Vector4::Project(const Vector4& other) const
	{
#if defined(SSE_INTRIN)
		__m128 summed = _mm_mul_ps(other.sse128, other.sse128);
		__m128 shuffled = _mm_shuffle_ps(summed, summed, _MM_SHUFFLE(2, 3, 0, 1));
		summed = _mm_add_ps(summed, shuffled);
		shuffled = _mm_movehl_ps(shuffled, summed);
		summed = _mm_add_ss(summed, shuffled);
		__m128 norm = _mm_mul_ps(other.sse128, _mm_rsqrt_ps(_mm_shuffle_ps(summed, summed, 0)));
		summed = _mm_mul_ps(sse128, norm);
		shuffled = _mm_shuffle_ps(summed, summed, _MM_SHUFFLE(2, 3, 0, 1));
		summed = _mm_add_ps(summed, shuffled);
		shuffled = _mm_movehl_ps(shuffled, summed);
		summed = _mm_add_ss(summed, shuffled);
		return _mm_mul_ps(norm, _mm_shuffle_ps(summed, summed, 0));
#else
		Vector4 n(other);
		n.Normalize();
		return Dot(n) * n;
#endif
	}


	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	Vector4 Vector4::Reflect(const Vector4& normal) const
	{
#if defined(SSE_INTRIN)
		__m128 summed = _mm_mul_ps(sse128, normal.sse128);
		__m128 shuffled = _mm_shuffle_ps(summed, summed, _MM_SHUFFLE(2, 3, 0, 1));
		summed = _mm_add_ps(summed, shuffled);
		shuffled = _mm_movehl_ps(shuffled, summed);
		summed = _mm_add_ss(summed, shuffled);
		summed = _mm_mul_ps(normal.sse128, _mm_mul_ps(_mm_shuffle_ps(summed, summed, 0), _mm_set_ps1(2.0f)));
		return _mm_sub_ps(sse128, summed);
#else
		return *this - ((2 * Dot(normal)) * normal);
#endif
	}


	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	std::string Vector4::ToString() const
	{
#if defined(_ANDROID)
		std::stringstream stream;
		stream << '[' << x << ", " << y << ", " << z << ", " << w << ']';
		return stream.str();
#else
		using namespace std;
		return string('[' + to_string(x) + ", " + to_string(y) + ", " + to_string(z) + ", " + to_string(w) + ']');
#endif
	}


	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	bool Vector4::operator==(const Vector4& other) const
	{
		return Equals(other);
	}


	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	bool Vector4::operator!=(const Vector4& other) const
	{
		return !Equals(other);
	}


	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	Vector4 operator-(Vector4 left, const Vector4& right)
	{
		return left.Subtract(right);
	}


	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	Vector4 operator+(Vector4 left, const Vector4& right)
	{
		return left.Add(right);
	}


	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	Vector4 operator-(Vector4 left, float right)
	{
		return left.Subtract(right);
	}


	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	Vector4 operator+(Vector4 left, float right)
	{
		return left.Add(right);
	}


	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	Vector4 operator*(Vector4 left, float right)
	{
		return left.Multiply(right);
	}


	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	Vector4 operator/(Vector4 left, float right)
	{
		return left.Divide(right);
	}


	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	Vector4 operator*(float left, Vector4 right)
	{
		return right.Multiply(left);
	}


	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	Vector4& Vector4::operator=(const Vector3& vector3)
	{
		*reinterpret_cast<Vector3*>(&x) = vector3;
		w = 1.0f;

		return *this;
	}


	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	Vector4 Vector4::operator-() const
	{
		return Vector4(-x, -y, -z, -w);
	}


	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	Vector4& Vector4::operator=(const Vector4& other)
	{
		if (this != &other)
		{
			x = other.x;
			y = other.y;
			z = other.z;
			w = other.w;
		}

		return *this;
	}


	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	Vector4& Vector4::operator-=(const Vector4& other)
	{
		return Subtract(other);
	}


	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	Vector4& Vector4::operator+=(const Vector4& other)
	{
		return Add(other);
	}


	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	Vector4& Vector4::operator-=(float scalar)
	{
		return Subtract(scalar);
	}


	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	Vector4& Vector4::operator+=(float scalar)
	{
		return Add(scalar);
	}


	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	Vector4& Vector4::operator*=(float scalar)
	{
		return Multiply(scalar);
	}


	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	Vector4& Vector4::operator/=(float scalar)
	{
		return Divide(scalar);
	}


	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	Vector4::operator Vector3&()
	{
		return *reinterpret_cast<Vector3*>(&x);
	}


	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	Vector4::operator const Vector3&() const
	{
		return *reinterpret_cast<const Vector3*>(&x);
	}


	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	Vector4 Vector4::Nan()
	{
		return Vector4(NAN, NAN, NAN, NAN);
	}
}