#pragma once
#include "Float2.h"

namespace Math
{
	///Non SIMD version
	///Should be used when sending data to shaders and generally for storage purposes
	struct Vec3f
	{
	public:
		forceinline explicit Vec3f(float xyz = 0.0f);
		forceinline explicit Vec3f(float x, float y, float z);

		forceinline bool vectorcall operator==(Vec3f other) const;
		forceinline bool vectorcall operator!=(Vec3f other) const;

		friend forceinline std::string vectorcall ToString(Vec3f f);

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
	};

	namespace Vec3Const
	{
		const Vec3f UP = Vec3f(0.0f, 1.0f, 0.0f);
		const Vec3f FORWARD = Vec3f(0.0f, 0.0f, 1.0f);
		const Vec3f RIGHT = Vec3f(1.0f, 0.0f, 0.0f);
	}

    
	forceinline Vec3f::Vec3f(float xyz)
	{
		x = y = z = xyz;
	}

    
	forceinline Vec3f::Vec3f(float x, float y, float z)	
	{
		this->x = x;
		this->y = y;
		this->z = z;
	}

    
	forceinline std::string vectorcall ToString(Vec3f f)
	{
		return '(' + std::to_string(f.x) + ", " + std::to_string(f.y) + ", " + std::to_string(f.z) + ')';
	}

    
	forceinline bool vectorcall Math::Vec3f::operator==(Vec3f other) const
	{
		return (x == other.x) && (y == other.y) && (z == other.z);
	}

    
	forceinline bool vectorcall Vec3f::operator!=(Vec3f other) const
	{
		return !(*this == other);
	}


	///SIMD version
	struct VECTORALIGN Float3
	{
	public:
		forceinline explicit Float3(float scalar = 0.0f);
		forceinline explicit Float3(float x, float y, float z);
		forceinline explicit Float3(Float2 xy, float z);
		forceinline explicit Float3(SSE_VARIABLE(xmm));

		forceinline void  vectorcall SetX(float x);
		forceinline void  vectorcall SetY(float y);
		forceinline void  vectorcall SetZ(float z);
		forceinline float vectorcall GetX() const;
		forceinline float vectorcall GetY() const;
		forceinline float vectorcall GetZ() const;

		forceinline Float3& vectorcall Add(Float3 other);
		forceinline Float3& vectorcall Add(float scalar);
		forceinline Float3& vectorcall Subtract(Float3 other);
		forceinline Float3& vectorcall Subtract(float scalar);
		forceinline Float3& vectorcall Multiply(Float3 other);
		forceinline Float3& vectorcall Multiply(float scalar);
		forceinline Float3& vectorcall Divide(Float3 other);
		forceinline Float3& vectorcall Divide(float scalar);

		bool vectorcall IsUnitVector() const;
		forceinline bool vectorcall Equals(Float3 other) const;

		forceinline float   vectorcall Dot(Float3 other) const;
		forceinline float   vectorcall LengthSqrd() const;
		forceinline float   vectorcall Length() const;
		forceinline Float3& vectorcall Normalize();
		forceinline Float3  vectorcall GetNormalized() const;
		forceinline Float3  vectorcall Cross(Float3 other) const;
		forceinline Float3  vectorcall Project(Float3 other) const;
		forceinline Float3  vectorcall Reflect(Float3 normal) const;

		forceinline Float3  vectorcall operator-() const;
		forceinline Float3& vectorcall operator=(Float3 other);
		forceinline Float3& vectorcall operator-=(Float3 other);
		forceinline Float3& vectorcall operator+=(Float3 other);
		forceinline Float3& vectorcall operator-=(float other);
		forceinline Float3& vectorcall operator+=(float other);
		forceinline Float3& vectorcall operator*=(Float3 other);
		forceinline Float3& vectorcall operator/=(Float3 other);
		forceinline Float3& vectorcall operator*=(float other);
		forceinline Float3& vectorcall operator/=(float other);

		forceinline bool vectorcall operator==(Float3 other) const;
		forceinline bool vectorcall operator!=(Float3 other) const;

		forceinline float vectorcall operator[](uint32 index);

		friend forceinline std::string	vectorcall ToString(Float3 f);
		friend forceinline Vec3f		vectorcall Store(Float3 f);
		friend forceinline Float3		vectorcall Load(Vec3f storage);
		friend forceinline Float3		vectorcall Min(Float3 f1, Float3 f2);
		friend forceinline Float3		vectorcall Max(Float3 f1, Float3 f2);
		friend forceinline Float3		vectorcall Lerp(float t, Float3 begin, Float3 end);
		friend forceinline Float3		vectorcall Clamp(Float3 num, Float3 min, Float3 max);
		friend forceinline Float3		vectorcall operator-(Float3 left, Float3 right);
		friend forceinline Float3		vectorcall operator+(Float3 left, Float3 right);
		friend forceinline Float3		vectorcall operator-(Float3 left, float right);
		friend forceinline Float3		vectorcall operator+(Float3 left, float right);
		friend forceinline Float3		vectorcall operator*(Float3 left, Float3 right);
		friend forceinline Float3		vectorcall operator/(Float3 left, Float3 right);
		friend forceinline Float3		vectorcall operator*(Float3 left, float right);
		friend forceinline Float3		vectorcall operator/(Float3 left, float right);
		friend forceinline Float3		vectorcall operator*(float left, Float3 right);

	private:
		SSE_VARIABLE(m_Xmm);
	};

#if defined(SSE_INTRIN)
	forceinline Float3::Float3(float scalar)
	{
		m_Xmm = _mm_set_ps(0.0f, scalar, scalar, scalar);
	}

    
	forceinline Float3::Float3(float x, float y, float z)
	{
		m_Xmm = _mm_set_ps(0.0f, z, y, x);
	}

    
	forceinline Float3::Float3(Float2 xy, float z)
	{
		m_Xmm = _mm_set_ps(0.0f, z, xy.GetY(), xy.GetX());
	}

    
	forceinline Float3::Float3(SSE_VARIABLE(xmm))
	{
		m_Xmm = xmm;
	}

    
	forceinline void vectorcall Float3::SetX(float x)
	{
		m_Xmm = _mm_move_ss(m_Xmm, _mm_set_ss(x));
	}
	
    
	forceinline void vectorcall Float3::SetY(float y)
	{
		//t = [y, this->y, this->z, this->w]
		__m128 t = _mm_move_ss(m_Xmm, _mm_set_ss(y));
		//t = [y, y, this->z, this->w]
		t = _mm_shuffle_ps(t, t, _MM_SHUFFLE(3, 2, 0, 0));
		//m_Xmm = [this->x, y, this->z, this->w]
		m_Xmm = _mm_move_ss(t, m_Xmm);
	}
	
    
	forceinline void vectorcall Float3::SetZ(float z)
	{
		//t = [z, this->y, this->z, this->w]
		__m128 t = _mm_move_ss(m_Xmm, _mm_set_ss(z));
		//t = [z, this->y, z, this->w]
		t = _mm_shuffle_ps(t, t, _MM_SHUFFLE(3, 0, 1, 0));
		//m_Xmm = [this->x, this->y, z, this->w]
		m_Xmm = _mm_move_ss(t, m_Xmm);
	}

    
	forceinline float vectorcall Float3::GetX() const
	{
		return _mm_cvtss_f32(m_Xmm);
	}

    
	forceinline float vectorcall Float3::GetY() const
	{
		return _mm_cvtss_f32(_mm_shuffle_ps(m_Xmm, m_Xmm, _MM_SHUFFLE(3, 2, 1, 1)));
	}
    

	forceinline float vectorcall Float3::GetZ() const
	{
		return _mm_cvtss_f32(_mm_shuffle_ps(m_Xmm, m_Xmm, _MM_SHUFFLE(3, 2, 1, 2)));
	}
    

	forceinline Float3& vectorcall Float3::Add(Float3 other)
	{
		m_Xmm = _mm_add_ps(m_Xmm, other.m_Xmm);
		return *this;
	}
    

	forceinline Float3& vectorcall Float3::Add(float scalar)
	{
		m_Xmm = _mm_add_ps(m_Xmm, _mm_set1_ps(scalar));
		return *this;
	}

    
	forceinline Float3& vectorcall Float3::Subtract(Float3 other)
	{
		m_Xmm = _mm_sub_ps(m_Xmm, other.m_Xmm);
		return *this;
	}

    
	forceinline Float3& vectorcall Float3::Subtract(float scalar)
	{
		m_Xmm = _mm_sub_ps(m_Xmm, _mm_set1_ps(scalar));
		return *this;
	}
    

	forceinline Float3& vectorcall Float3::Multiply(Float3 other)
	{
		m_Xmm = _mm_mul_ps(m_Xmm, other.m_Xmm);
		return *this;
	}
    

	forceinline Float3& vectorcall Float3::Multiply(float scalar)
	{
		m_Xmm = _mm_mul_ps(m_Xmm, _mm_set1_ps(scalar));
		return *this;
	}
    

	forceinline Float3& vectorcall Float3::Divide(Float3 other)
	{
		m_Xmm = _mm_div_ps(m_Xmm, other.m_Xmm);
		m_Xmm = _mm_shuffle_ps(m_Xmm, _mm_move_ss(m_Xmm, _mm_setzero_ps()), _MM_SHUFFLE(0, 2, 1, 0));
		return *this;
	}
    
    
	forceinline Float3& vectorcall Float3::Divide(float scalar)
	{
		m_Xmm = _mm_div_ps(m_Xmm, _mm_set1_ps(scalar));
		return *this;
	}

    
	forceinline bool vectorcall Float3::Equals(Float3 other) const
	{
		int32 mask = _mm_movemask_ps(_mm_cmpeq_ps(m_Xmm, other.m_Xmm));
		return 0x7 == (mask & 0x7);
	}

    
	forceinline float vectorcall Float3::Dot(Float3 other) const
	{
		__m128 dot = __m128Dot(m_Xmm, other.m_Xmm);
		return _mm_cvtss_f32(dot);
	}
    

	forceinline float vectorcall Float3::LengthSqrd() const
	{
		__m128 dot = __m128Dot(m_Xmm, m_Xmm);
		return _mm_cvtss_f32(dot);
	}

    
	forceinline float vectorcall Float3::Length() const
	{
		__m128 dot = __m128Dot(m_Xmm, m_Xmm);
		return _mm_cvtss_f32(_mm_sqrt_ss(dot));
	}

    
	forceinline Float3 vectorcall Min(Float3 f1, Float3 f2)
	{
		return Float3(_mm_min_ps(f1.m_Xmm, f2.m_Xmm));
	}

    
	forceinline Float3 vectorcall Max(Float3 f1, Float3 f2)
	{
		return Float3(_mm_max_ps(f1.m_Xmm, f2.m_Xmm));
	}

    
	forceinline Float3& vectorcall Float3::Normalize()
	{
		__m128 dot = __m128Dot(m_Xmm, m_Xmm);
		m_Xmm = _mm_mul_ps(m_Xmm, _mm_rsqrt_ps(_mm_shuffle_ps(dot, dot, 0)));
		return *this;
	}

    
	forceinline Float3 vectorcall Float3::Cross(Float3 other) const
	{
		__m128 mul1 = _mm_mul_ps(other.m_Xmm, _mm_shuffle_ps(m_Xmm, m_Xmm, _MM_SHUFFLE(3, 0, 2, 1)));
		__m128 mul2 = _mm_mul_ps(m_Xmm, _mm_shuffle_ps(other.m_Xmm, other.m_Xmm, _MM_SHUFFLE(3, 0, 2, 1)));
		__m128 result = _mm_sub_ps(mul2, mul1);
		return Float3(_mm_shuffle_ps(result, result, _MM_SHUFFLE(3, 0, 2, 1)));
	}

    
	forceinline Float3 vectorcall Float3::Project(Float3 other) const
	{
		__m128 norm = GetNormalized().m_Xmm;
		__m128 dot = __m128Dot(other.m_Xmm, norm);
		return Float3(_mm_mul_ps(norm, _mm_shuffle_ps(dot, dot, 0)));
	}
    

	forceinline Float3 vectorcall Float3::Reflect(Float3 normal) const
	{
		__m128 dot = __m128Dot(m_Xmm, normal.m_Xmm);
		dot = _mm_mul_ps(normal.m_Xmm, _mm_mul_ps(_mm_shuffle_ps(dot, dot, 0), _mm_set_ps1(2.0f)));
		return Float3(_mm_sub_ps(m_Xmm, dot));
	}

    
	forceinline Float3 vectorcall Float3::operator-() const
	{
		__m128 zero = _mm_setzero_ps();
		__m128 result = _mm_sub_ps(zero, m_Xmm);
		return Float3(result);
	}

    
	forceinline Float3& vectorcall Float3::operator=(Float3 other)
	{
		m_Xmm = other.m_Xmm;
		return *this;
	}

    
	forceinline float vectorcall Float3::operator[](uint32 index)
	{
		assert(index < 3);
		return m_Xmm.m128_f32[index];
	}
#else
	forceinline Float3::Float3(float scalar)
	{
		m_Xmm[0] = scalar;
		m_Xmm[1] = scalar;
		m_Xmm[2] = scalar;
		m_Xmm[3] = 0.0f;
	}

    
	forceinline Float3::Float3(float x, float y, float z)
	{
		m_Xmm[0] = x;
		m_Xmm[1] = y;
		m_Xmm[2] = z;
		m_Xmm[3] = 0.0f;
	}

    
	forceinline Float3::Float3(Float2 xy, float z)
	{
		m_Xmm[0] = xy.GetX();
		m_Xmm[1] = xy.GetY();
		m_Xmm[2] = z;
		m_Xmm[3] = 0.0f;
	}

    
	forceinline Float3::Float3(SSE_VARIABLE(xmm))
	{
		memcpy(m_Xmm, xmm, sizeof(m_Xmm));
	}

    
	forceinline void vectorcall Float3::SetX(float x)
	{
		m_Xmm[0] = x;
	}

    
	forceinline void vectorcall Float3::SetY(float y)
	{
		m_Xmm[1] = y;
	}

    
	forceinline void vectorcall Float3::SetZ(float z)
	{
		m_Xmm[2] = z;
	}

    
	forceinline float vectorcall Float3::GetX() const
	{
		return m_Xmm[0];
	}
    

	forceinline float vectorcall Float3::GetY() const
	{
		return m_Xmm[1];
	}
    

	forceinline float vectorcall Float3::GetZ() const
	{
		return m_Xmm[2];
	}
    
    
	forceinline Float3& vectorcall Float3::Add(Float3 other)
	{
		m_Xmm[0] += other.m_Xmm[0];
		m_Xmm[1] += other.m_Xmm[1];
		m_Xmm[2] += other.m_Xmm[2];
		return *this;
	}

    
	forceinline Float3& vectorcall Float3::Add(float scalar)
	{
		m_Xmm[0] += scalar;
		m_Xmm[1] += scalar;
		m_Xmm[2] += scalar;
		return *this;
	}
    

	forceinline Float3& vectorcall Float3::Subtract(Float3 other)
	{
		m_Xmm[0] -= other.m_Xmm[0];
		m_Xmm[1] -= other.m_Xmm[1];
		m_Xmm[2] -= other.m_Xmm[2];
		return *this;
	}
    

	forceinline Float3& vectorcall Float3::Subtract(float scalar)
	{
		m_Xmm[0] -= scalar;
		m_Xmm[1] -= scalar;
		m_Xmm[2] -= scalar;
		return *this;
	}
    

	forceinline Float3& vectorcall Float3::Multiply(Float3 other)
	{
		m_Xmm[0] *= other.m_Xmm[0];
		m_Xmm[1] *= other.m_Xmm[1];
		m_Xmm[2] *= other.m_Xmm[2];
		return *this;
	}

    
	forceinline Float3& vectorcall Float3::Multiply(float scalar)
	{
		m_Xmm[0] *= scalar;
		m_Xmm[1] *= scalar;
		m_Xmm[2] *= scalar;
		return *this;
	}

    
	forceinline Float3& vectorcall Float3::Divide(Float3 other)
	{
		m_Xmm[0] /= other.m_Xmm[0];
		m_Xmm[1] /= other.m_Xmm[1];
		m_Xmm[2] /= other.m_Xmm[2];
		return *this;
	}

    
	forceinline Float3& vectorcall Float3::Divide(float scalar)
	{
		m_Xmm[0] /= scalar;
		m_Xmm[1] /= scalar;
		m_Xmm[2] /= scalar;
		return *this;
	}

    
	forceinline bool vectorcall Float3::Equals(Float3 other) const
	{
		return	(m_Xmm[0] == other.m_Xmm[0]) &&
				(m_Xmm[1] == other.m_Xmm[1]) &&
				(m_Xmm[2] == other.m_Xmm[2]);
	}
    

	forceinline float vectorcall Float3::Dot(Float3 other) const
	{
		return	(m_Xmm[0] * other.m_Xmm[0]) +
				(m_Xmm[1] * other.m_Xmm[1]) +
				(m_Xmm[2] * other.m_Xmm[2]);
	}

    
	forceinline float vectorcall Float3::LengthSqrd() const
	{
		return	(m_Xmm[0] * m_Xmm[0]) +
				(m_Xmm[1] * m_Xmm[1]) +
				(m_Xmm[2] * m_Xmm[2]);
	}
    

	forceinline float vectorcall Float3::Length() const
	{
		return sqrt(LengthSqrd());
	}
    

	forceinline Float3 vectorcall Min(Float3 f1, Float3 f2)
	{
		Float3 result;
		result.m_Xmm[0] = Min<float>(f1.m_Xmm[0], f2.m_Xmm[0]);
		result.m_Xmm[1] = Min<float>(f1.m_Xmm[1], f2.m_Xmm[1]);
		result.m_Xmm[2] = Min<float>(f1.m_Xmm[2], f2.m_Xmm[2]);
		return result;
	}
    

	forceinline Float3 vectorcall Max(Float3 f1, Float3 f2)
	{
		Float3 result;
		result.m_Xmm[0] = Max<float>(f1.m_Xmm[0], f2.m_Xmm[0]);
		result.m_Xmm[1] = Max<float>(f1.m_Xmm[1], f2.m_Xmm[1]);
		result.m_Xmm[2] = Max<float>(f1.m_Xmm[2], f2.m_Xmm[2]);
		return result;
	}

    
	forceinline Float3& vectorcall Float3::Normalize()
	{
		float len = Length();
		if (len < 0.000001f)
			*this = Float3(NAN);
		else
			*this /= len;

		return *this;
	}

    
	forceinline Float3 vectorcall Float3::Cross(Float3 other) const
	{
		return Float3(
			(m_Xmm[1] * other.m_Xmm[2]) - (other.m_Xmm[1] * m_Xmm[2]),
			(m_Xmm[2] * other.m_Xmm[0]) - (other.m_Xmm[2] * m_Xmm[0]),
			(m_Xmm[0] * other.m_Xmm[1]) - (other.m_Xmm[0] * m_Xmm[1]));
	}
    

	forceinline Float3 vectorcall Float3::Project(Float3 other) const
	{
		Float3 norm = *this;
		norm.Normalize();
		return other.Dot(norm) * norm;
	}

    
	forceinline Float3 vectorcall Float3::Reflect(Float3 normal) const
	{
		return *this - ((2.0f * Dot(normal)) * normal);
	}
    

	forceinline Float3 vectorcall Float3::operator-() const
	{
		Float3 result;
		result.m_Xmm[0] = -m_Xmm[0];
		result.m_Xmm[1] = -m_Xmm[1];
		result.m_Xmm[2] = -m_Xmm[2];
		return result;
	}

    
	forceinline Float3& vectorcall Float3::operator=(Float3 other)
	{
		memcpy(m_Xmm, other.m_Xmm, sizeof(other));
		return *this;
	}
    

	forceinline float vectorcall Float3::operator[](uint32 index)
	{
		assert(index < 3);
		return m_Xmm[index];
	}
#endif

    
	inline bool vectorcall Float3::IsUnitVector() const
	{
		float len = Length();
		return 0.99999f > len && len < 1.00001f;
	}

    forceinline Float3 vectorcall Lerp(float t, Float3 begin, Float3 end)
	{
		return ((1.0f - t) * begin) + (t * end);
	}

    
	forceinline Float3 vectorcall Clamp(Float3 num, Float3 min, Float3 max)
	{
		return Max(Min(num, max), min);
	}
    

	forceinline Float3 vectorcall Float3::GetNormalized() const
	{
		Float3 normalized = *this;
		normalized.Normalize();
		return normalized;
	}

    
	forceinline std::string vectorcall ToString(Float3 f)
	{
		return '(' + std::to_string(f.GetX()) + ", " + std::to_string(f.GetY()) + ", " + std::to_string(f.GetZ()) + ')';
	}

    
	forceinline Float3& vectorcall Float3::operator-=(Float3 other)
	{
		return *this = *this - other;
	}

    
	forceinline Float3& vectorcall Float3::operator+=(Float3 other)
	{
		return *this = *this + other;
	}

    
	forceinline Float3& vectorcall Float3::operator-=(float scalar)
	{
		return *this = *this - scalar;
	}
    

	forceinline Float3& vectorcall Float3::operator+=(float scalar)
	{
		return *this = *this + scalar;
	}
    

	forceinline Float3& vectorcall Float3::operator*=(Float3 other)
	{
		return *this = *this * other;
	}
    

	forceinline Float3& vectorcall Float3::operator/=(Float3 other)
	{
		return *this = *this / other;
	}

    
	forceinline Float3& vectorcall Float3::operator*=(float scalar)
	{
		return *this = *this * scalar;
	}
    

	forceinline Float3& vectorcall Float3::operator/=(float scalar)
	{
		return *this = *this / scalar;
	}
	
    
	forceinline bool vectorcall Float3::operator==(Float3 other) const
	{
		return Equals(other);
	}

    
	forceinline bool vectorcall Float3::operator!=(Float3 other) const
	{
		return !Equals(other);
	}

    
	forceinline Float3 vectorcall Load(Vec3f storage)
	{
		return Float3(storage.x, storage.y, storage.z);
	}

    
	forceinline Vec3f vectorcall Store(Float3 f)
	{
		Vec3f result;
		result.x = f.GetX();
		result.y = f.GetY();
		result.z = f.GetZ();
		return result;
	}
    

	forceinline Float3 vectorcall Math::operator-(Float3 left, Float3 right)
	{
		return left.Subtract(right);
	}
    

	forceinline Float3 vectorcall operator+(Float3 left, Float3 right)
	{
		return left.Add(right);
	}

    
	forceinline Float3 vectorcall operator-(Float3 left, float right)
	{
		return left.Subtract(right);
	}

    
	forceinline Float3 vectorcall operator+(Float3 left, float right)
	{
		return left.Add(right);
	}
    

	forceinline Float3 vectorcall operator*(Float3 left, Float3 right)
	{
		return left.Multiply(right);
	}

    
	forceinline Float3 vectorcall operator/(Float3 left, Float3 right)
	{
		return left.Divide(right);
	}

    
	forceinline Float3 vectorcall operator*(Float3 left, float right)
	{
		return left.Multiply(right);
	}
    

	forceinline Float3 vectorcall operator/(Float3 left, float right)
	{
		return left.Divide(right);
	}
    

	forceinline Float3 vectorcall operator*(float left, Float3 right)
	{
		return right.Multiply(left);
	}
}
