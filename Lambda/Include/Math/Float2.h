#pragma once
#include "MathCommon.h"

namespace Math
{
	///Non SIMD version
	///Should be used when sending data to shaders and generally for storage purposes
	struct Vec2f
	{
	public:
		forceinline explicit Vec2f(float xy = 0.0f);
		forceinline explicit Vec2f(float x, float y);

		forceinline bool vectorcall operator==(Vec2f other) const;
		forceinline bool vectorcall operator!=(Vec2f other) const;

		friend forceinline std::string vectorcall ToString(Vec2f v);

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
	};
    

	forceinline Vec2f::Vec2f(float xy)
	{
		x = y = xy;
	}
    

	forceinline Vec2f::Vec2f(float x, float y)
	{
		this->x = x;
		this->y = y;
	}

    
	forceinline std::string vectorcall ToString(Vec2f v)
	{
		return '(' + std::to_string(v.x) + ", " + std::to_string(v.y) + ')';
	}

    
	forceinline bool vectorcall Math::Vec2f::operator==(Vec2f other) const
	{
		return (x == other.x) && (y == other.y);
	}
    

	forceinline bool vectorcall Vec2f::operator!=(Vec2f other) const
	{
		return !(*this == other);
	}


	///SIMD version
	struct VECTORALIGN Float2
	{
	public:
		forceinline explicit Float2(float scalar = 0.0f);
		forceinline explicit Float2(float x, float y);
		forceinline explicit Float2(SSE_VARIABLE(xmm));

		forceinline void vectorcall SetX(float x);
		forceinline void vectorcall SetY(float y);
		forceinline float vectorcall GetX() const;
		forceinline float vectorcall GetY() const;

		forceinline Float2& vectorcall Add(Float2 other);
		forceinline Float2& vectorcall Add(float scalar);
		forceinline Float2& vectorcall Subtract(Float2 other);
		forceinline Float2& vectorcall Subtract(float scalar);
		forceinline Float2& vectorcall Multiply(Float2 other);
		forceinline Float2& vectorcall Multiply(float scalar);
		forceinline Float2& vectorcall Divide(Float2 other);
		forceinline Float2& vectorcall Divide(float scalar);

		bool vectorcall IsUnitVector() const;
		forceinline bool vectorcall Equals(Float2 other) const;

		forceinline float vectorcall Dot(Float2 other) const;
		forceinline float vectorcall LengthSqrd() const;
		forceinline float vectorcall Length() const;
		forceinline Float2& vectorcall Normalize();
		forceinline Float2 vectorcall GetNormalized() const;
		forceinline Float2 vectorcall Normal() const;
		forceinline Float2 vectorcall Reflect(Float2 normal) const;
		//Project other onto this
		forceinline Float2 vectorcall Project(Float2 other) const;

		forceinline Float2 vectorcall operator-() const;
		forceinline Float2& vectorcall operator=(Float2 other);
		forceinline Float2& vectorcall operator-=(Float2 other);
		forceinline Float2& vectorcall operator+=(Float2 other);
		forceinline Float2& vectorcall operator-=(float other);
		forceinline Float2& vectorcall operator+=(float other);
		forceinline Float2& vectorcall operator*=(Float2 other);
		forceinline Float2& vectorcall operator/=(Float2 other);
		forceinline Float2& vectorcall operator*=(float other);
		forceinline Float2& vectorcall operator/=(float other);

		forceinline bool vectorcall operator==(Float2 other) const;
		forceinline bool vectorcall operator!=(Float2 other) const;

		forceinline float vectorcall operator[](uint32 index);

		friend forceinline std::string vectorcall ToString(Float2 f);
		friend forceinline Vec2f vectorcall Store(Float2 f);
		friend forceinline Float2 vectorcall Load(Vec2f storage);
		friend forceinline Float2 vectorcall Min(Float2 f1, Float2 f2);
		friend forceinline Float2 vectorcall Max(Float2 f1, Float2 f2);
		friend forceinline Float2 vectorcall Lerp(float t, Float2 begin, Float2 end);
		friend forceinline Float2 vectorcall Clamp(Float2 num, Float2 min, Float2 max);
		friend forceinline Float2 vectorcall operator-(Float2 left, Float2 right);
		friend forceinline Float2 vectorcall operator+(Float2 left, Float2 right);
		friend forceinline Float2 vectorcall operator-(Float2 left, float right);
		friend forceinline Float2 vectorcall operator+(Float2 left, float right);
		friend forceinline Float2 vectorcall operator*(Float2 left, Float2 right);
		friend forceinline Float2 vectorcall operator/(Float2 left, Float2 right);
		friend forceinline Float2 vectorcall operator*(Float2 left, float right);
		friend forceinline Float2 vectorcall operator/(Float2 left, float right);
		friend forceinline Float2 vectorcall operator*(float left, Float2 right);

	private:
		SSE_VARIABLE(m_Xmm);
	};

#if defined(SSE_INTRIN)
	forceinline Float2::Float2(float scalar)
	{
		m_Xmm = _mm_set_ps(0.0f, 0.0f, scalar, scalar);
	}

    
	forceinline Float2::Float2(float x, float y)
	{
		m_Xmm = _mm_set_ps(0.0f, 0.0f, y, x);
	}

    
	forceinline Float2::Float2(SSE_VARIABLE(xmm))
	{
		m_Xmm = xmm;
	}

    
	forceinline void vectorcall Float2::SetX(float x)
	{
		m_Xmm = _mm_move_ss(m_Xmm, _mm_set_ss(x));
	}
	
    
	forceinline void vectorcall Float2::SetY(float y)
	{
		//t = [y, this->y, this->z, this->w]
		__m128 t = _mm_move_ss(m_Xmm, _mm_set_ss(y));
		//t = [y, y, this->z, this->w]
		t = _mm_shuffle_ps(t, t, _MM_SHUFFLE(3, 2, 0, 0));
		//m_Xmm = [this->x, y, this->z, this->w]
		m_Xmm = _mm_move_ss(t, m_Xmm);
	}

    
	forceinline float vectorcall Float2::GetX() const
	{
		return _mm_cvtss_f32(m_Xmm);
	}

    
	forceinline float vectorcall Float2::GetY() const
	{
		return _mm_cvtss_f32(_mm_shuffle_ps(m_Xmm, m_Xmm, _MM_SHUFFLE(3, 2, 1, 1)));
	}

    
	forceinline Float2& vectorcall Float2::Add(Float2 other)
	{
		m_Xmm = _mm_add_ps(m_Xmm, other.m_Xmm);
		return *this;
	}

    
	forceinline Float2& vectorcall Float2::Add(float scalar)
	{
		m_Xmm = _mm_add_ps(m_Xmm, _mm_set1_ps(scalar));
		return *this;
	}

    
	forceinline Float2& vectorcall Float2::Subtract(Float2 other)
	{
		m_Xmm = _mm_sub_ps(m_Xmm, other.m_Xmm);
		return *this;
	}

    
	forceinline Float2& vectorcall Float2::Subtract(float scalar)
	{
		m_Xmm = _mm_sub_ps(m_Xmm, _mm_set1_ps(scalar));
		return *this;
	}

    
	forceinline Float2& vectorcall Float2::Multiply(Float2 other)
	{
		m_Xmm = _mm_mul_ps(m_Xmm, other.m_Xmm);
		return *this;
	}

    
	forceinline Float2& vectorcall Float2::Multiply(float scalar)
	{
		m_Xmm = _mm_mul_ps(m_Xmm, _mm_set1_ps(scalar));
		return *this;
	}

    
	forceinline Float2& vectorcall Float2::Divide(Float2 other)
	{
		m_Xmm = _mm_div_ps(m_Xmm, other.m_Xmm);
		m_Xmm = _mm_shuffle_ps(m_Xmm, _mm_setzero_ps(), _MM_SHUFFLE(0, 0, 1, 0));
		return *this;
	}
    

	forceinline Float2& vectorcall Float2::Divide(float scalar)
	{
		m_Xmm = _mm_div_ps(m_Xmm, _mm_set1_ps(scalar));
		return *this;
	}
    

	forceinline bool vectorcall Float2::Equals(Float2 other) const
	{
		int32 mask = _mm_movemask_ps(_mm_cmpeq_ps(m_Xmm, other.m_Xmm));
		return 0x3 == (mask & 0x3);
	}

    
	forceinline float vectorcall Float2::Dot(Float2 other) const
	{
		__m128 dot = __m128Dot(m_Xmm, other.m_Xmm);
		return _mm_cvtss_f32(dot);
	}

    
	forceinline float vectorcall Float2::LengthSqrd() const
	{
		__m128 dot = __m128Dot(m_Xmm, m_Xmm);
		return _mm_cvtss_f32(dot);
	}

    
	forceinline float vectorcall Float2::Length() const
	{
		__m128 dot = __m128Dot(m_Xmm, m_Xmm);
		return _mm_cvtss_f32(_mm_sqrt_ss(dot));
	}

    
	forceinline Float2 vectorcall Min(Float2 f1, Float2 f2)
	{
		return Float2(_mm_min_ps(f1.m_Xmm, f2.m_Xmm));
	}

    
	forceinline Float2 vectorcall Max(Float2 f1, Float2 f2)
	{
		return Float2(_mm_max_ps(f1.m_Xmm, f2.m_Xmm));
	}

    
	forceinline Float2& vectorcall Float2::Normalize()
	{
		__m128 dot = __m128Dot(m_Xmm, m_Xmm);
		m_Xmm = _mm_mul_ps(m_Xmm, _mm_rsqrt_ps(_mm_shuffle_ps(dot, dot, 0)));
		return *this;
	}

    
	forceinline Float2 vectorcall Float2::Project(Float2 other) const
	{
		__m128 norm = GetNormalized().m_Xmm;
		__m128 dot = __m128Dot(other.m_Xmm, norm);
		return Float2(_mm_mul_ps(norm, _mm_shuffle_ps(dot, dot, 0)));
	}

    
	forceinline Float2 vectorcall Float2::Reflect(Float2 normal) const
	{
		__m128 dot = __m128Dot(m_Xmm, normal.m_Xmm);
		dot = _mm_mul_ps(normal.m_Xmm, _mm_mul_ps(_mm_shuffle_ps(dot, dot, 0), _mm_set_ps1(2.0f)));
		return Float2(_mm_sub_ps(m_Xmm, dot));
	}

    
	forceinline Float2 vectorcall Float2::operator-() const
	{
		__m128 zero = _mm_setzero_ps();
		__m128 result = _mm_sub_ps(zero, m_Xmm);
		return Float2(result);
	}

    
	forceinline Float2& vectorcall Float2::operator=(Float2 other)
	{
		m_Xmm = other.m_Xmm;
		return *this;
	}
    

	forceinline float vectorcall Float2::operator[](uint32 index)
	{
		assert(index < 2);
		return m_Xmm.m128_f32[index];
	}
#else
	forceinline Float2::Float2(float scalar)
	{
		m_Xmm[0] = scalar;
		m_Xmm[1] = scalar;
		m_Xmm[2] = 0.0f;
		m_Xmm[3] = 0.0f;
	}

    
	forceinline Float2::Float2(float x, float y)
	{
		m_Xmm[0] = x;
		m_Xmm[1] = y;
		m_Xmm[2] = 0.0f;
		m_Xmm[3] = 0.0f;
	}

    
	forceinline Float2::Float2(SSE_VARIABLE(xmm))
	{
		memcpy(m_Xmm, xmm, sizeof(m_Xmm));
	}

    
	forceinline void vectorcall Float2::SetX(float x)
	{
		m_Xmm[0] = x;
	}

    
	forceinline void vectorcall Float2::SetY(float y)
	{
		m_Xmm[1] = y;
	}

    
	forceinline float vectorcall Float2::GetX() const
	{
		return m_Xmm[0];
	}

    
	forceinline float vectorcall Float2::GetY() const
	{
		return m_Xmm[1];
	}
    

	forceinline Float2& vectorcall Float2::Add(Float2 other)
	{
		m_Xmm[0] += other.m_Xmm[0];
		m_Xmm[1] += other.m_Xmm[1];
		return *this;
	}

    
	forceinline Float2& vectorcall Float2::Add(float scalar)
	{
		m_Xmm[0] += scalar;
		m_Xmm[1] += scalar;
		return *this;
	}

    
	forceinline Float2& vectorcall Float2::Subtract(Float2 other)
	{
		m_Xmm[0] -= other.m_Xmm[0];
		m_Xmm[1] -= other.m_Xmm[1];
		return *this;
	}

    
	forceinline Float2& vectorcall Float2::Subtract(float scalar)
	{
		m_Xmm[0] -= scalar;
		m_Xmm[1] -= scalar;
		return *this;
	}

    
	forceinline Float2& vectorcall Float2::Multiply(Float2 other)
	{
		m_Xmm[0] *= other.m_Xmm[0];
		m_Xmm[1] *= other.m_Xmm[1];
		return *this;
	}

    
	forceinline Float2& vectorcall Float2::Multiply(float scalar)
	{
		m_Xmm[0] *= scalar;
		m_Xmm[1] *= scalar;
		return *this;
	}
    

	forceinline Float2& vectorcall Float2::Divide(Float2 other)
	{
		m_Xmm[0] /= other.m_Xmm[0];
		m_Xmm[1] /= other.m_Xmm[1];
		return *this;
	}
    

	forceinline Float2& vectorcall Float2::Divide(float scalar)
	{
		m_Xmm[0] /= scalar;
		m_Xmm[1] /= scalar;
		return *this;
	}

    
	forceinline bool vectorcall Float2::Equals(Float2 other) const
	{
		return	(m_Xmm[0] == other.m_Xmm[0]) &&
				(m_Xmm[1] == other.m_Xmm[1]) &&
				(m_Xmm[2] == other.m_Xmm[2]);
	}

    
	forceinline float vectorcall Float2::Dot(Float2 other) const
	{
		return	(m_Xmm[0] * other.m_Xmm[0]) +
				(m_Xmm[1] * other.m_Xmm[1]);
	}

    
	forceinline float vectorcall Float2::LengthSqrd() const
	{
		return	(m_Xmm[0] * m_Xmm[0]) +
				(m_Xmm[1] * m_Xmm[1]);
	}
    

	forceinline float vectorcall Float2::Length() const
	{
		return sqrt(LengthSqrd());
	}

    
	forceinline Float2 vectorcall Min(Float2 f1, Float2 f2)
	{
		Float2 result;
		result.m_Xmm[0] = Min<float>(f1.m_Xmm[0], f2.m_Xmm[0]);
		result.m_Xmm[1] = Min<float>(f1.m_Xmm[1], f2.m_Xmm[1]);
		return result;
	}

    
	forceinline Float2 vectorcall Max(Float2 f1, Float2 f2)
	{
		Float2 result;
		result.m_Xmm[0] = Max<float>(f1.m_Xmm[0], f2.m_Xmm[0]);
		result.m_Xmm[1] = Max<float>(f1.m_Xmm[1], f2.m_Xmm[1]);
		return result;
	}
    

	forceinline Float2& vectorcall Float2::Normalize()
	{
		float len = Length();
		if (len < 0.000001f)
			*this = Float2(NAN);
		else
			*this /= len;

		return *this;
	}

    
	forceinline Float2 vectorcall Float2::Project(Float2 other) const
	{
		Float2 norm = *this;
		norm.Normalize();
		return other.Dot(norm) * norm;
	}

    
	forceinline Float2 vectorcall Float2::Reflect(Float2 normal) const
	{
		return *this - ((2.0f * Dot(normal)) * normal);
	}

    
	forceinline Float2 vectorcall Float2::operator-() const
	{
		Float2 result;
		result.m_Xmm[0] = -m_Xmm[0];
		result.m_Xmm[1] = -m_Xmm[1];
		result.m_Xmm[2] = -m_Xmm[2];
		return result;
	}
    

	forceinline Float2& vectorcall Float2::operator=(Float2 other)
	{
		memcpy(m_Xmm, other.m_Xmm, sizeof(other));
		return *this;
	}

    
	forceinline float vectorcall Float2::operator[](uint32 index)
	{
		assert(index < 2);
		return m_Xmm[index];
	}
#endif

    
	forceinline Float2 vectorcall Math::Float2::Normal() const
	{
		return Float2(-GetY(), GetX());
	}

    
	inline bool vectorcall Float2::IsUnitVector() const
	{
		float len = Length();
		return 0.99999f > len && len < 1.00001f;
	}

    
	forceinline Float2 vectorcall Lerp(float t, Float2 begin, Float2 end)
	{
		return ((1.0f - t) * begin) + (t * end);
	}

    
	forceinline Float2 vectorcall Clamp(Float2 num, Float2 min, Float2 max)
	{
		return Max(Min(num, max), min);
	}
    

	forceinline Float2 vectorcall Float2::GetNormalized() const
	{
		Float2 normalized = *this;
		normalized.Normalize();
		return normalized;
	}
    

	forceinline std::string vectorcall ToString(Float2 f)
	{
		return '(' + std::to_string(f.GetX()) + ", " + std::to_string(f.GetY()) + ')';
	}
    

	forceinline Float2& vectorcall Float2::operator-=(Float2 other)
	{
		return *this = *this - other;
	}
    

	forceinline Float2& vectorcall Float2::operator+=(Float2 other)
	{
		return *this = *this + other;
	}
    

	forceinline Float2& vectorcall Float2::operator-=(float scalar)
	{
		return *this = *this - scalar;
	}

    
	forceinline Float2& vectorcall Float2::operator+=(float scalar)
	{
		return *this = *this + scalar;
	}
    

	forceinline Float2& vectorcall Float2::operator*=(Float2 other)
	{
		return *this = *this * other;
	}

    
	forceinline Float2& vectorcall Float2::operator/=(Float2 other)
	{
		return *this = *this / other;
	}

    
	forceinline Float2& vectorcall Float2::operator*=(float scalar)
	{
		return *this = *this * scalar;
	}

    
	forceinline Float2& vectorcall Float2::operator/=(float scalar)
	{
		return *this = *this / scalar;
	}
	
    
	forceinline bool vectorcall Float2::operator==(Float2 other) const
	{
		return Equals(other);
	}

    
	forceinline bool vectorcall Float2::operator!=(Float2 other) const
	{
		return !Equals(other);
	}
    

	forceinline Float2 vectorcall Load(Vec2f storage)
	{
		return Float2(storage.x, storage.y);
	}
    

	forceinline Vec2f vectorcall Store(Float2 f)
	{
		Vec2f result;
		result.x = f.GetX();
		result.y = f.GetY();
		return result;
	}
    

	forceinline Float2 vectorcall operator-(Float2 left, Float2 right)
	{
		return left.Subtract(right);
	}

    
	forceinline Float2 vectorcall operator+(Float2 left, Float2 right)
	{
		return left.Add(right);
	}

    
	forceinline Float2 vectorcall operator-(Float2 left, float right)
	{
		return left.Subtract(right);
	}

    
	forceinline Float2 vectorcall operator+(Float2 left, float right)
	{
		return left.Add(right);
	}

    
	forceinline Float2 vectorcall operator*(Float2 left, Float2 right)
	{
		return left.Multiply(right);
	}

    
	forceinline Float2 vectorcall operator/(Float2 left, Float2 right)
	{
		return left.Divide(right);
	}
    

	forceinline Float2 vectorcall operator*(Float2 left, float right)
	{
		return left.Multiply(right);
	}

    
	forceinline Float2 vectorcall operator/(Float2 left, float right)
	{
		return left.Divide(right);
	}

    
	forceinline Float2 vectorcall operator*(float left, Float2 right)
	{
		return right.Multiply(left);
	}
}
