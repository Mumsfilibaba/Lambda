#pragma once
#include "MathCommon.h"
#include "Vector3.h"

namespace Math
{
	struct VECTORALIGN Vector4
	{
	public:
#if defined(SSE_INTRIN)
		Vector4(__m128 xmm);
#endif
		Vector4(const Vector4& other);
		explicit Vector4(const Vector2& xy, float z, float w);
		explicit Vector4(const Vector2& xy, const Vector2& zw);
		explicit Vector4(const Vector3& xyz, float w);
		explicit Vector4(float x = 0.0f, float y = 0.0f, float z = 0.0f, float w = 0.0f);
		explicit Vector4(const Vector3& other);

		Vector4& vectorcall Add(const Vector4& other);
		Vector4& Add(float scalar);
		Vector4& Subtract(const Vector4& other);
		Vector4& Subtract(float scalar);
		Vector4& Multiply(float scalar);
		Vector4& Divide(float scalar);

		bool IsUnitVector() const;
		bool Equals(const Vector4& other) const;

		float Dot(const Vector4& other) const;
		float LengthSqrd() const;
		float Length() const;
		Vector4& Normalize();
		Vector4 UnitVector() const;
		Vector4 Project(const Vector4& other) const;
		Vector4 Reflect(const Vector4& normal) const;

		std::string ToString() const;

		bool operator==(const Vector4& other) const;
		bool operator!=(const Vector4& other) const;

		float& operator[](uint32 index);
		const float& operator[](uint32 index) const;

		Vector4& operator=(const Vector4& other);
		Vector4& operator=(const Vector3& vector3);
		
		forceinline Vector4 vectorcall operator-() const;
		forceinline Vector4& vectorcall operator-=(const Vector4& other);
		forceinline Vector4& vectorcall operator+=(const Vector4& other);
		forceinline Vector4& vectorcall operator-=(float scalar);
		forceinline Vector4& vectorcall operator+=(float scalar);
		forceinline Vector4& vectorcall operator*=(float scalar);
		forceinline Vector4& vectorcall operator/=(float scalar);

		explicit operator Vector3&();
		explicit operator const Vector3&() const;

		friend forceinline Vector4 vectorcall operator-(Vector4 left, const Vector4& right);
		friend forceinline Vector4 vectorcall operator+(Vector4 left, const Vector4& right);
		friend forceinline Vector4 vectorcall operator-(Vector4 left, float right);
		friend forceinline Vector4 vectorcall operator+(Vector4 left, float right);
		friend forceinline Vector4 vectorcall operator*(Vector4 left, float right);
		friend forceinline Vector4 vectorcall operator/(Vector4 left, float right);
		friend forceinline Vector4 vectorcall operator*(float left, Vector4 right);

	public:
		union
		{
			struct
			{
				float x;
				float y;
				float z;
				float w;
			};

			struct
			{
				float r;
				float g;
				float b;
				float a;
			};

			SSE_VARIABLE(Xmm);
		};

	public:
		static Vector4 Nan();
	};

	//ColorF& ColorF::Lerp(const ColorF& src, float delta)
	//{
	//	r = r + (src.r - r) * delta;
	//	g = g + (src.g - g) * delta;
	//	b = b + (src.b - b) * delta;
	//	a = a + (src.a - a) * delta;

	//	return *this;
	//}

	//ColorF ColorF::Lerp(const ColorF& src, float delta) const
	//{
	//	return ColorF(*this).Lerp(src, delta);
	//}

#if defined(SSE_INTRIN)
	inline Vector4::Vector4(__m128 xmm)
		: Xmm(xmm)
	{
	}
#endif

	inline Vector4::Vector4(float x, float y, float z, float w)
		: x(x),
		y(y),
		z(z),
		w(w)
	{
	}

	inline Vector4::Vector4(const Vector4& other)
		: x(0.0f), 
		y(0.0f), 
		z(0.0f), 
		w(0.0f)
	{
		*this = other;
	}

	inline Vector4::Vector4(const Vector2& xy, float z, float w)
		: x(0.0f),
		y(0.0f),
		z(0.0f),
		w(0.0f)
	{
		x = xy.x;
		y = xy.y;
		z = z;
		w = w;
	}

	inline Vector4::Vector4(const Vector2& xy, const Vector2& zw)
		: x(0.0f),
		y(0.0f),
		z(0.0f),
		w(0.0f)
	{
		x = xy.x;
		y = xy.y;
		z = zw.x;
		w = zw.y;
	}

	inline Vector4::Vector4(const Vector3& xyz, float w)
		: x(0.0f),
		y(0.0f),
		z(0.0f),
		w(0.0f)
	{
		x = xyz.x;
		y = xyz.y;
		z = xyz.z;
		w = w;
	}

	inline Vector4::Vector4(const Vector3& vector3)
		: x(0.0f),
		y(0.0f),
		z(0.0f),
		w(0.0f)
	{
		*this = vector3;
	}

	inline Vector4& vectorcall Vector4::Add(const Vector4& other)
	{
#if defined(SSE_INTRIN)
		Xmm = _mm_add_ps(Xmm, other.Xmm);
		return *this;
#else
		x += other.x;
		y += other.y;
		z += other.z;
		w += other.w;

		return *this;
#endif
	}

	inline Vector4& Vector4::Add(float scalar)
	{
#if defined(SSE_INTRIN)
		Xmm = _mm_add_ps(Xmm, _mm_set_ps1(scalar));
		return *this;
#else
		x += scalar;
		y += scalar;
		z += scalar;
		w += scalar;
		return *this;
#endif
	}

	inline Vector4& Vector4::Subtract(const Vector4& other)
	{
#if defined(SSE_INTRIN)
		Xmm = _mm_sub_ps(Xmm, other.Xmm);
		return *this;
#else
		x -= other.x;
		y -= other.y;
		z -= other.z;
		w -= other.w;
		return *this;
#endif
	}

	inline Vector4& Vector4::Subtract(float scalar)
	{
#if defined(SSE_INTRIN)
		Xmm = _mm_sub_ps(Xmm, _mm_set_ps1(scalar));
		return *this;
#else
		x -= scalar;
		y -= scalar;
		z -= scalar;
		w -= scalar;
		return *this;
#endif
	}

	inline Vector4& Vector4::Multiply(float scalar)
	{
#if defined(SSE_INTRIN)
		Xmm = _mm_mul_ps(Xmm, _mm_set_ps1(scalar));
		return *this;
#else
		x *= scalar;
		y *= scalar;
		z *= scalar;
		w *= scalar;
		return *this;
#endif
	}

	inline Vector4& Vector4::Divide(float scalar)
	{
#if defined(SSE_INTRIN)
		Xmm = _mm_div_ps(Xmm, _mm_set_ps1(scalar));
		return *this;
#else
		x /= scalar;
		y /= scalar;
		z /= scalar;
		w /= scalar;
#endif
		return *this;
	}

	//Returns true if inside the interval [0.999999, 1.000001]
	inline bool Vector4::IsUnitVector() const
	{
		float length = Length();
		return length > 0.999999f && length < 1.000001f;
	}

	inline bool Vector4::Equals(const Vector4& other) const
	{
		return (x == other.x && y == other.y && z == other.z && w == other.w);
	}

	inline float Vector4::Dot(const Vector4& other) const
	{
#if defined(SSE_INTRIN)
		__m128 summed = _mm_mul_ps(Xmm, other.Xmm);
		__m128 shuffled = _mm_shuffle_ps(summed, summed, _MM_SHUFFLE(2, 3, 0, 1));
		summed = _mm_add_ps(summed, shuffled);
		shuffled = _mm_movehl_ps(shuffled, summed);
		summed = _mm_add_ss(summed, shuffled);
		return _mm_cvtss_f32(summed);
#else
		return (x * other.x) + (y * other.y) + (z * other.z) + (w * other.w);
#endif
	}

	inline float Vector4::LengthSqrd() const
	{
#if defined(SSE_INTRIN)
		__m128 summed = _mm_mul_ps(Xmm, Xmm);
		__m128 shuffled = _mm_shuffle_ps(summed, summed, _MM_SHUFFLE(2, 3, 0, 1));
		summed = _mm_add_ps(summed, shuffled);
		shuffled = _mm_movehl_ps(shuffled, summed);
		return _mm_cvtss_f32(_mm_add_ss(summed, shuffled));
#else
		return (x * x) + (y * y) + (z * z) + (w * w);
#endif
	}

	inline float Vector4::Length() const
	{
#if defined(SSE_INTRIN)
		__m128 summed = _mm_mul_ps(Xmm, Xmm);
		__m128 shuffled = _mm_shuffle_ps(summed, summed, _MM_SHUFFLE(2, 3, 0, 1));
		summed = _mm_add_ps(summed, shuffled);
		shuffled = _mm_movehl_ps(shuffled, summed);
		return _mm_cvtss_f32(_mm_sqrt_ss(_mm_add_ss(summed, shuffled)));
#else
		return sqrt(LengthSqrd());
#endif
	}

	inline Vector4& Vector4::Normalize()
	{
#if defined(SSE_INTRIN)
		__m128 summed = _mm_mul_ps(Xmm, Xmm);
		__m128 shuffled = _mm_shuffle_ps(summed, summed, _MM_SHUFFLE(2, 3, 0, 1));
		summed = _mm_add_ps(summed, shuffled);
		shuffled = _mm_movehl_ps(shuffled, summed);
		summed = _mm_add_ss(summed, shuffled);
		Xmm = _mm_mul_ps(Xmm, _mm_rsqrt_ps(_mm_shuffle_ps(summed, summed, 0)));
		return *this;
#else
		float length = Length();

		if (length > 0)
			Divide(length);
		else
			memset(this, 0, sizeof(Vector4));
		return *this;
#endif
	}

	inline Vector4 Vector4::UnitVector() const
	{
#if defined(SSE_INTRIN)
		__m128 summed = _mm_mul_ps(Xmm, Xmm);
		__m128 shuffled = _mm_shuffle_ps(summed, summed, _MM_SHUFFLE(2, 3, 0, 1));
		summed = _mm_add_ps(summed, shuffled);
		shuffled = _mm_movehl_ps(shuffled, summed);
		summed = _mm_add_ss(summed, shuffled);
		return _mm_mul_ps(Xmm, _mm_rsqrt_ps(_mm_shuffle_ps(summed, summed, 0)));
#else
		return Vector4(*this).Normalize();
#endif
	}

	inline Vector4 Vector4::Project(const Vector4& other) const
	{
#if defined(SSE_INTRIN)
		__m128 summed = _mm_mul_ps(other.Xmm, other.Xmm);
		__m128 shuffled = _mm_shuffle_ps(summed, summed, _MM_SHUFFLE(2, 3, 0, 1));
		summed = _mm_add_ps(summed, shuffled);
		shuffled = _mm_movehl_ps(shuffled, summed);
		summed = _mm_add_ss(summed, shuffled);
		__m128 norm = _mm_mul_ps(other.Xmm, _mm_rsqrt_ps(_mm_shuffle_ps(summed, summed, 0)));
		summed = _mm_mul_ps(Xmm, norm);
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

	inline Vector4 Vector4::Reflect(const Vector4& normal) const
	{
#if defined(SSE_INTRIN)
		__m128 summed = _mm_mul_ps(Xmm, normal.Xmm);
		__m128 shuffled = _mm_shuffle_ps(summed, summed, _MM_SHUFFLE(2, 3, 0, 1));
		summed = _mm_add_ps(summed, shuffled);
		shuffled = _mm_movehl_ps(shuffled, summed);
		summed = _mm_add_ss(summed, shuffled);
		summed = _mm_mul_ps(normal.Xmm, _mm_mul_ps(_mm_shuffle_ps(summed, summed, 0), _mm_set_ps1(2.0f)));
		return _mm_sub_ps(Xmm, summed);
#else
		return *this - ((2 * Dot(normal)) * normal);
#endif
	}

	inline std::string Vector4::ToString() const
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

	inline bool Vector4::operator==(const Vector4& other) const
	{
		return Equals(other);
	}

	inline bool Vector4::operator!=(const Vector4& other) const
	{
		return !Equals(other);
	}

	inline float& Vector4::operator[](uint32 index)
	{
		return reinterpret_cast<float*>(this)[index];
	}

	inline const float& Vector4::operator[](uint32 index) const
	{
		return reinterpret_cast<const float*>(this)[index];
	}

	forceinline Vector4 vectorcall operator-(Vector4 left, const Vector4& right)
	{
		return left.Subtract(right);
	}

	forceinline Vector4 vectorcall operator+(Vector4 left, const Vector4& right)
	{
		return left.Add(right);
	}

	forceinline Vector4 vectorcall operator-(Vector4 left, float right)
	{
		return left.Subtract(right);
	}

	forceinline Vector4 vectorcall operator+(Vector4 left, float right)
	{
		return left.Add(right);
	}

	forceinline Vector4 vectorcall operator*(Vector4 left, float right)
	{
		return left.Multiply(right);
	}

	forceinline Vector4 vectorcall operator/(Vector4 left, float right)
	{
		return left.Divide(right);
	}

	forceinline Vector4 vectorcall operator*(float left, Vector4 right)
	{
		return right.Multiply(left);
	}

	inline Vector4& Vector4::operator=(const Vector3& vector3)
	{
		*reinterpret_cast<Vector3*>(&x) = vector3;
		w = 1.0f;

		return *this;
	}

	inline Vector4& Vector4::operator=(const Vector4& other)
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

	forceinline Vector4 vectorcall Vector4::operator-() const
	{
		return Vector4(-x, -y, -z, -w);
	}

	forceinline Vector4& vectorcall Vector4::operator-=(const Vector4& other)
	{
		return Subtract(other);
	}

	forceinline Vector4& vectorcall Vector4::operator+=(const Vector4& other)
	{
		return Add(other);
	}

	forceinline Vector4& vectorcall Vector4::operator-=(float scalar)
	{
		return Subtract(scalar);
	}

	forceinline Vector4& vectorcall Vector4::operator+=(float scalar)
	{
		return Add(scalar);
	}

	forceinline Vector4& vectorcall Vector4::operator*=(float scalar)
	{
		return Multiply(scalar);
	}

	forceinline Vector4& vectorcall Vector4::operator/=(float scalar)
	{
		return Divide(scalar);
	}

	inline Vector4::operator Vector3&()
	{
		return *reinterpret_cast<Vector3*>(&x);
	}

	inline Vector4::operator const Vector3&() const
	{
		return *reinterpret_cast<const Vector3*>(&x);
	}

	inline Vector4 Vector4::Nan()
	{
		return Vector4(NAN, NAN, NAN, NAN);
	}
}