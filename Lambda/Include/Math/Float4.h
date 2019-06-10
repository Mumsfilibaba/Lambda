#pragma once
#include "Float3.h"

namespace Math
{
	///Non SIMD version
	///Should be used when sending data to shaders and generally for storage purposes
	struct Vec4f
	{
	public:
		forceinline Vec4f();
		forceinline explicit Vec4f(float xyzw);
		forceinline explicit Vec4f(float x, float y, float z, float w);

		forceinline bool vectorcall operator==(Vec4f other) const;
		forceinline bool vectorcall operator!=(Vec4f other) const;

		friend forceinline std::string vectorcall ToString(Vec4f f);

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
		};
	};

    
	namespace Vec4Const
	{
		const Vec4f BLACK = Vec4f(0.0f, 0.0f, 0.0f, 1.0f);
		const Vec4f WHITE = Vec4f(1.0f, 1.0f, 1.0f, 1.0f);
		const Vec4f WARMWHITE = Vec4f(1.0f, 0.945f, 0.88f, 1.0f);
		const Vec4f GRAY = Vec4f(0.502f, 0.502f, 0.502f, 1.0f);
		const Vec4f DARKGRAY = Vec4f(0.251f, 0.251f, 0.251f, 1.0f);
		const Vec4f SOFTGRAY = Vec4f(0.753f, 0.753f, 0.753f, 1.0f);
		const Vec4f RED = Vec4f(1.0f, 0.0f, 0.0f, 1.0f);
		const Vec4f DARKRED = Vec4f(0.502f, 0.0f, 0.0f, 1.0f);
		const Vec4f SOFTRED = Vec4f(1.0f, 0.251f, 0.251f, 1.0f);
		const Vec4f GREEN = Vec4f(0.0f, 1.0f, 0.0f, 1.0f);
		const Vec4f SOFTGREEN = Vec4f(0.251f, 1.0f, 0.251f, 1.0f);
		const Vec4f DARKGREEN = Vec4f(0.0f, 0.502f, 0.0f, 1.0f);
		const Vec4f BLUE = Vec4f(0.0f, 0.0f, 1.0f, 1.0f);
		const Vec4f DARKBLUE = Vec4f(0.0f, 0.0f, 0.502f, 1.0f);
		const Vec4f SOFTBLUE = Vec4f(0.325f, 0.325f, 1.0f, 1.0f);
		const Vec4f CORNFLOWERBLUE = Vec4f(0.392f, 0.584f, 0.929f, 1.0f);
		const Vec4f YELLOW = Vec4f(1.0f, 1.0f, 0.0f, 1.0f);
		const Vec4f SOFTYELLOW = Vec4f(1.0f, 1.0f, 0.251f, 1.0f);
		const Vec4f DARKYELLOW = Vec4f(0.502f, 0.502f, 0.0f, 1.0f);
		const Vec4f MAGENTA = Vec4f(1.0f, 0.0f, 1.0f, 1.0f);
		const Vec4f PURPLE = Vec4f(0.502f, 0.0f, 0.502f, 1.0f);
		const Vec4f PINK = Vec4f(1.0f, 0.753f, 0.796f, 1.0f);
	}

    
	forceinline Vec4f::Vec4f()
	{
		x = y = z = w = 0;
	}
    

	forceinline Vec4f::Vec4f(float xyzw)
	{
		x = y = z = w =  xyzw;
	}
    

	forceinline Vec4f::Vec4f(float x, float y, float z, float w)
	{
		this->x = x;
		this->y = y;
		this->z = z;
		this->w = w;
	}
    

	forceinline std::string vectorcall ToString(Vec4f f)
	{
		return '(' + std::to_string(f.x) + ", " + std::to_string(f.y) + ", " + std::to_string(f.z) + ", " + std::to_string(f.w) + ')';
	}

    
	forceinline bool vectorcall Math::Vec4f::operator==(Vec4f other) const
	{
		return (x == other.x) && (y == other.y) && (z == other.z) && (w == other.w);
	}

    
	forceinline bool vectorcall Vec4f::operator!=(Vec4f other) const
	{
		return !(*this == other);
	}


	///SIMD version
	struct VECTORALIGN Float4
	{
		friend struct Float4x4;

	public:
		forceinline explicit Float4(float scalar = 0.0f);
		forceinline explicit Float4(float x, float y, float z, float w);
		forceinline explicit Float4(Float2 xy, Float2 zw);
		forceinline explicit Float4(Float3 xyw, float w);
		forceinline explicit Float4(SSE_VARIABLE(xmm));

		forceinline void	vectorcall SetX(float x);
		forceinline void	vectorcall SetY(float y);
		forceinline void	vectorcall SetZ(float z);
		forceinline void	vectorcall SetW(float w);
		forceinline float	vectorcall GetX() const;
		forceinline float	vectorcall GetY() const;
		forceinline float	vectorcall GetZ() const;
		forceinline float	vectorcall GetW() const;

		forceinline Float4& vectorcall Add(Float4 other);
		forceinline Float4& vectorcall Add(float scalar);
		forceinline Float4& vectorcall Subtract(Float4 other);
		forceinline Float4& vectorcall Subtract(float scalar);
		forceinline Float4& vectorcall Multiply(Float4 other);
		forceinline Float4& vectorcall Multiply(float scalar);
		forceinline Float4& vectorcall Divide(Float4 other);
		forceinline Float4& vectorcall Divide(float scalar);

		bool vectorcall IsUnitVector() const;
		forceinline bool vectorcall Equals(Float4 other) const;

		forceinline float	vectorcall Dot(Float4 other) const;
		forceinline float	vectorcall LengthSqrd() const;
		forceinline float	vectorcall Length() const;
		forceinline Float4& vectorcall Normalize();
		forceinline Float4	vectorcall GetNormalized() const;
		forceinline Float4	vectorcall Project(Float4 other) const;
		forceinline Float4	vectorcall Reflect(Float4 normal) const;

		forceinline Float4	vectorcall operator-() const;
		forceinline Float4& vectorcall operator=(Float4 other);
		forceinline Float4& vectorcall operator-=(Float4 other);
		forceinline Float4& vectorcall operator+=(Float4 other);
		forceinline Float4& vectorcall operator-=(float other);
		forceinline Float4& vectorcall operator+=(float other);
		forceinline Float4& vectorcall operator*=(Float4 other);
		forceinline Float4& vectorcall operator/=(Float4 other);
		forceinline Float4& vectorcall operator*=(float other);
		forceinline Float4& vectorcall operator/=(float other);

		forceinline bool vectorcall operator==(Float4 other) const;
		forceinline bool vectorcall operator!=(Float4 other) const;

		forceinline float vectorcall operator[](uint32 index);

		friend forceinline std::string	vectorcall ToString(Float4 f);
		friend forceinline Vec4f		vectorcall Store(Float4 f);
		friend forceinline Float4		vectorcall Load(Vec4f storage);
		friend forceinline Float4		vectorcall Min(Float4 f1, Float4 f2);
		friend forceinline Float4		vectorcall Max(Float4 f1, Float4 f2);
		friend forceinline Float4		vectorcall Lerp(float t, Float4 begin, Float4 end);
		friend forceinline Float4		vectorcall Clamp(Float4 num, Float4 min, Float4 max);
		friend forceinline Float4		vectorcall operator-(Float4 left, Float4 right);
		friend forceinline Float4		vectorcall operator+(Float4 left, Float4 right);
		friend forceinline Float4		vectorcall operator-(Float4 left, float right);
		friend forceinline Float4		vectorcall operator+(Float4 left, float right);
		friend forceinline Float4		vectorcall operator*(Float4 left, Float4 right);
		friend forceinline Float4		vectorcall operator/(Float4 left, Float4 right);
		friend forceinline Float4		vectorcall operator*(Float4 left, float right);
		friend forceinline Float4		vectorcall operator/(Float4 left, float right);
		friend forceinline Float4		vectorcall operator*(float left, Float4 right);

	private:
		SSE_VARIABLE(m_Xmm);
	};

#if defined(SSE_INTRIN)
	forceinline Float4::Float4(float scalar)
	{
		m_Xmm = _mm_set1_ps(scalar);
	}

    
	forceinline Float4::Float4(float x, float y, float z, float w)
	{
		m_Xmm = _mm_set_ps(w, z, y, x);
	}

    
	forceinline Float4::Float4(Float2 xy, Float2 zw)
	{
		m_Xmm = _mm_set_ps(zw.GetY(), zw.GetX(), xy.GetY(), xy.GetX());
	}
    

	forceinline Float4::Float4(Float3 xyw, float w)
	{
		m_Xmm = _mm_set_ps(w, xyw.GetZ(), xyw.GetY(), xyw.GetX());
	}
    
    
	forceinline Float4::Float4(SSE_VARIABLE(xmm))
	{
		m_Xmm = xmm;
	}
    

	forceinline void vectorcall Float4::SetX(float x)
	{
		m_Xmm = _mm_move_ss(m_Xmm, _mm_set_ss(x));
	}
	
    
	forceinline void vectorcall Float4::SetY(float y)
	{
		//t = [y, this->y, this->z, this->w]
		__m128 t = _mm_move_ss(m_Xmm, _mm_set_ss(y));
		//t = [y, y, this->z, this->w]
		t = _mm_shuffle_ps(t, t, _MM_SHUFFLE(3, 2, 0, 0));
		//m_Xmm = [this->x, y, this->z, this->w]
		m_Xmm = _mm_move_ss(t, m_Xmm);
	}
	
    
	forceinline void vectorcall Float4::SetZ(float z)
	{
		//t = [z, this->y, this->z, this->w]
		__m128 t = _mm_move_ss(m_Xmm, _mm_set_ss(z));
		//t = [z, this->y, z, this->w]
		t = _mm_shuffle_ps(t, t, _MM_SHUFFLE(3, 0, 1, 0));
		//m_Xmm = [this->x, this->y, z, this->w]
		m_Xmm = _mm_move_ss(t, m_Xmm);
	}

    
	forceinline void vectorcall Float4::SetW(float w)
	{
		//t = [w, this->y, this->z, this->w]
		__m128 t = _mm_move_ss(m_Xmm, _mm_set_ss(w));
		//t = [w, this->y, this->z, w]
		t = _mm_shuffle_ps(t, t, _MM_SHUFFLE(0, 2, 1, 0));
		//m_Xmm = [this->x, this->y, this->z, w]
		m_Xmm = _mm_move_ss(t, m_Xmm);
	}
    

	forceinline float vectorcall Float4::GetX() const
	{
		return _mm_cvtss_f32(m_Xmm);
	}

    
	forceinline float vectorcall Float4::GetY() const
	{
		return _mm_cvtss_f32(_mm_shuffle_ps(m_Xmm, m_Xmm, _MM_SHUFFLE(3, 2, 1, 1)));
	}
    

	forceinline float vectorcall Float4::GetZ() const
	{
		return _mm_cvtss_f32(_mm_shuffle_ps(m_Xmm, m_Xmm, _MM_SHUFFLE(3, 2, 1, 2)));
	}
    

	forceinline float vectorcall Float4::GetW() const
	{
		return _mm_cvtss_f32(_mm_shuffle_ps(m_Xmm, m_Xmm, _MM_SHUFFLE(3, 2, 1, 3)));
	}

    
	forceinline Float4& vectorcall Float4::Add(Float4 other)
	{
		m_Xmm = _mm_add_ps(m_Xmm, other.m_Xmm);
		return *this;
	}

    
	forceinline Float4& vectorcall Float4::Add(float scalar)
	{
		m_Xmm = _mm_add_ps(m_Xmm, _mm_set1_ps(scalar));
		return *this;
	}

    
	forceinline Float4& vectorcall Float4::Subtract(Float4 other)
	{
		m_Xmm = _mm_sub_ps(m_Xmm, other.m_Xmm);
		return *this;
	}
    

	forceinline Float4& vectorcall Float4::Subtract(float scalar)
	{
		m_Xmm = _mm_sub_ps(m_Xmm, _mm_set1_ps(scalar));
		return *this;
	}

    
	forceinline Float4& vectorcall Float4::Multiply(Float4 other)
	{
		m_Xmm = _mm_mul_ps(m_Xmm, other.m_Xmm);
		return *this;
	}
    

	forceinline Float4& vectorcall Float4::Multiply(float scalar)
	{
		m_Xmm = _mm_mul_ps(m_Xmm, _mm_set1_ps(scalar));
		return *this;
	}

    
	forceinline Float4& vectorcall Float4::Divide(Float4 other)
	{
		m_Xmm = _mm_div_ps(m_Xmm, other.m_Xmm);
		return *this;
	}
    

	forceinline Float4& vectorcall Float4::Divide(float scalar)
	{
		m_Xmm = _mm_div_ps(m_Xmm, _mm_set1_ps(scalar));
		return *this;
	}

    
	forceinline bool vectorcall Float4::Equals(Float4 other) const
	{
		int32 mask = _mm_movemask_ps(_mm_cmpeq_ps(m_Xmm, other.m_Xmm));
		return 0xf == (mask & 0xf);
	}
    

	forceinline float vectorcall Float4::Dot(Float4 other) const
	{
		__m128 dot = __m128Dot(m_Xmm, other.m_Xmm);
		return _mm_cvtss_f32(dot);
	}
    

	forceinline float vectorcall Float4::LengthSqrd() const
	{
		__m128 dot = __m128Dot(m_Xmm, m_Xmm);
		return _mm_cvtss_f32(dot);
	}
    

	forceinline float vectorcall Float4::Length() const
	{
		__m128 dot = __m128Dot(m_Xmm, m_Xmm);
		return _mm_cvtss_f32(_mm_sqrt_ss(dot));
	}
    

	forceinline Float4 vectorcall Min(Float4 f1, Float4 f2)
	{
		return Float4(_mm_min_ps(f1.m_Xmm, f2.m_Xmm));
	}
    

	forceinline Float4 vectorcall Max(Float4 f1, Float4 f2)
	{
		return Float4(_mm_max_ps(f1.m_Xmm, f2.m_Xmm));
	}

    
	forceinline Float4& vectorcall Float4::Normalize()
	{
		__m128 dot = __m128Dot(m_Xmm, m_Xmm);
		m_Xmm = _mm_mul_ps(m_Xmm, _mm_rsqrt_ps(_mm_shuffle_ps(dot, dot, 0)));
		return *this;
	}

    
	forceinline Float4 vectorcall Float4::Project(Float4 other) const
	{
		__m128 norm = GetNormalized().m_Xmm;
		__m128 dot = __m128Dot(other.m_Xmm, norm);
		return Float4(_mm_mul_ps(norm, _mm_shuffle_ps(dot, dot, 0)));
	}

    
	forceinline Float4 vectorcall Float4::Reflect(Float4 normal) const
	{
		__m128 dot = __m128Dot(m_Xmm, normal.m_Xmm);
		dot = _mm_mul_ps(normal.m_Xmm, _mm_mul_ps(_mm_shuffle_ps(dot, dot, 0), _mm_set_ps1(2.0f)));
		return Float4(_mm_sub_ps(m_Xmm, dot));
	}

    
	forceinline Float4 vectorcall Float4::operator-() const
	{
		__m128 zero = _mm_setzero_ps();
		__m128 result = _mm_sub_ps(zero, m_Xmm);
		return Float4(result);
	}
    

	forceinline Float4& vectorcall Float4::operator=(Float4 other)
	{
		m_Xmm = other.m_Xmm;
		return *this;
	}
    

	forceinline float vectorcall Float4::operator[](uint32 index)
	{
		assert(index < 4);
		return m_Xmm.m128_f32[index];
	}
#else
	forceinline Float4::Float4(float scalar)
	{
		m_Xmm[0] = scalar;
		m_Xmm[1] = scalar;
		m_Xmm[2] = scalar;
		m_Xmm[3] = scalar;
	}

    
	forceinline Float4::Float4(float x, float y, float z, float w)
	{
		m_Xmm[0] = x;
		m_Xmm[1] = y;
		m_Xmm[2] = z;
		m_Xmm[3] = w;
	}
    

	forceinline Float4::Float4(Float2 xy, Float2 zw)
	{
		m_Xmm[0] = xy.GetX();
		m_Xmm[1] = xy.GetY();
		m_Xmm[2] = zw.GetX();
		m_Xmm[3] = zw.GetY();
	}

    
	forceinline Float4::Float4(Float3 xyw, float w)
	{
		m_Xmm[0] = xyw.GetX();
		m_Xmm[1] = xyw.GetY();
		m_Xmm[2] = xyw.GetZ();
		m_Xmm[3] = w;
	}

    
	forceinline Float4::Float4(SSE_VARIABLE(xmm))
	{
		memcpy(m_Xmm, xmm, sizeof(m_Xmm));
	}
    

	forceinline void vectorcall Float4::SetX(float x)
	{
		m_Xmm[0] = x;
	}
    

	forceinline void vectorcall Float4::SetY(float y)
	{
		m_Xmm[1] = y;
	}
    

	forceinline void vectorcall Float4::SetZ(float z)
	{
		m_Xmm[2] = z;
	}
    

	forceinline void vectorcall Float4::SetW(float w)
	{
		m_Xmm[3] = w;
	}

    
	forceinline float vectorcall Float4::GetX() const
	{
		return m_Xmm[0];
	}

    
	forceinline float vectorcall Float4::GetY() const
	{
		return m_Xmm[1];
	}

    
	forceinline float vectorcall Float4::GetZ() const
	{
		return m_Xmm[2];
	}

    
	forceinline float vectorcall Float4::GetW() const
	{
		return m_Xmm[3];
	}
    

	forceinline Float4& vectorcall Float4::Add(Float4 other)
	{
		m_Xmm[0] += other.m_Xmm[0];
		m_Xmm[1] += other.m_Xmm[1];
		m_Xmm[2] += other.m_Xmm[2];
		m_Xmm[3] += other.m_Xmm[3];
		return *this;
	}

    
	forceinline Float4& vectorcall Float4::Add(float scalar)
	{
		m_Xmm[0] += scalar;
		m_Xmm[1] += scalar;
		m_Xmm[2] += scalar;
		m_Xmm[3] += scalar;
		return *this;
	}
    

	forceinline Float4& vectorcall Float4::Subtract(Float4 other)
	{
		m_Xmm[0] -= other.m_Xmm[0];
		m_Xmm[1] -= other.m_Xmm[1];
		m_Xmm[2] -= other.m_Xmm[2];
		m_Xmm[3] -= other.m_Xmm[3];
		return *this;
	}
    

	forceinline Float4& vectorcall Float4::Subtract(float scalar)
	{
		m_Xmm[0] -= scalar;
		m_Xmm[1] -= scalar;
		m_Xmm[2] -= scalar;
		m_Xmm[3] -= scalar;
		return *this;
	}

    
	forceinline Float4& vectorcall Float4::Multiply(Float4 other)
	{
		m_Xmm[0] *= other.m_Xmm[0];
		m_Xmm[1] *= other.m_Xmm[1];
		m_Xmm[2] *= other.m_Xmm[2];
		m_Xmm[3] *= other.m_Xmm[3];
		return *this;
	}
    

	forceinline Float4& vectorcall Float4::Multiply(float scalar)
	{
		m_Xmm[0] *= scalar;
		m_Xmm[1] *= scalar;
		m_Xmm[2] *= scalar;
		m_Xmm[3] *= scalar;
		return *this;
	}
    

	forceinline Float4& vectorcall Float4::Divide(Float4 other)
	{
		m_Xmm[0] /= other.m_Xmm[0];
		m_Xmm[1] /= other.m_Xmm[1];
		m_Xmm[2] /= other.m_Xmm[2];
		m_Xmm[3] /= other.m_Xmm[3];
		return *this;
	}

    
	forceinline Float4& vectorcall Float4::Divide(float scalar)
	{
		m_Xmm[0] /= scalar;
		m_Xmm[1] /= scalar;
		m_Xmm[2] /= scalar;
		m_Xmm[3] /= scalar;
		return *this;
	}
    

	forceinline bool vectorcall Float4::Equals(Float4 other) const
	{
		return	(m_Xmm[0] == other.m_Xmm[0]) &&
				(m_Xmm[1] == other.m_Xmm[1]) &&
				(m_Xmm[2] == other.m_Xmm[2]) &&
				(m_Xmm[3] == other.m_Xmm[3]);
	}
    

	forceinline float vectorcall Float4::Dot(Float4 other) const
	{
		return	(m_Xmm[0] * other.m_Xmm[0]) +
				(m_Xmm[1] * other.m_Xmm[1]) +
				(m_Xmm[2] * other.m_Xmm[2]) +
				(m_Xmm[3] * other.m_Xmm[3]);
	}

    
	forceinline float vectorcall Float4::LengthSqrd() const
	{
		return	(m_Xmm[0] * m_Xmm[0]) +
				(m_Xmm[1] * m_Xmm[1]) +
				(m_Xmm[2] * m_Xmm[2]) +
				(m_Xmm[3] * m_Xmm[3]);
	}
    

	forceinline float vectorcall Float4::Length() const
	{
		return sqrt(LengthSqrd());
	}

    
	forceinline Float4 vectorcall Min(Float4 f1, Float4 f2)
	{
		Float4 result;
		result.m_Xmm[0] = Min<float>(f1.m_Xmm[0], f2.m_Xmm[0]);
		result.m_Xmm[1] = Min<float>(f1.m_Xmm[1], f2.m_Xmm[1]);
		result.m_Xmm[2] = Min<float>(f1.m_Xmm[2], f2.m_Xmm[2]);
		result.m_Xmm[3] = Min<float>(f1.m_Xmm[3], f2.m_Xmm[3]);
		return result;
	}

    
	forceinline Float4 vectorcall Max(Float4 f1, Float4 f2)
	{
		Float4 result;
		result.m_Xmm[0] = Max<float>(f1.m_Xmm[0], f2.m_Xmm[0]);
		result.m_Xmm[1] = Max<float>(f1.m_Xmm[1], f2.m_Xmm[1]);
		result.m_Xmm[2] = Max<float>(f1.m_Xmm[2], f2.m_Xmm[2]);
		result.m_Xmm[3] = Max<float>(f1.m_Xmm[3], f2.m_Xmm[3]);
		return result;
	}

    
	forceinline Float4& vectorcall Float4::Normalize()
	{
		float len = Length();
		if (len < 0.000001f)
			*this = Float4(NAN);
		else
			*this /= len;

		return *this;
	}

    
	forceinline Float4 vectorcall Float4::Project(Float4 other) const
	{
		Float4 norm = *this;
		norm.Normalize();
		return other.Dot(norm) * norm;
	}

    
	forceinline Float4 vectorcall Float4::Reflect(Float4 normal) const
	{
		return *this - ((2.0f * Dot(normal)) * normal);
	}

    
	forceinline Float4 vectorcall Float4::operator-() const
	{
		Float4 result;
		result.m_Xmm[0] = -m_Xmm[0];
		result.m_Xmm[1] = -m_Xmm[1];
		result.m_Xmm[2] = -m_Xmm[2];
		result.m_Xmm[3] = -m_Xmm[3];
		return result;
	}
    

	forceinline Float4& vectorcall Float4::operator=(Float4 other)
	{
		memcpy(m_Xmm, other.m_Xmm, sizeof(other));
		return *this;
	}

    
	forceinline float vectorcall Float4::operator[](uint32 index)
	{
		return m_Xmm[index];
	}
#endif

    
	inline bool vectorcall Float4::IsUnitVector() const
	{
		float len = Length();
		return 0.99999f > len && len < 1.00001f;
	}
    

	forceinline Float4 vectorcall Lerp(float t, Float4 begin, Float4 end)
	{
		return ((1.0f - t) * begin) + (t * end);
	}
    

	forceinline Float4 vectorcall Clamp(Float4 num, Float4 min, Float4 max)
	{
		return Max(Min(num, max), min);
	}

    
	forceinline Float4 vectorcall Float4::GetNormalized() const
	{
		Float4 normalized = *this;
		normalized.Normalize();
		return normalized;
	}

    
	forceinline std::string vectorcall ToString(Float4 f)
	{
		return '(' + std::to_string(f.GetX()) + ", " + std::to_string(f.GetY()) + ", " + std::to_string(f.GetZ()) + ", " + std::to_string(f.GetW()) + ')';
	}

    
	forceinline Float4& vectorcall Float4::operator-=(Float4 other)
	{
		return *this = *this - other;
	}

    
	forceinline Float4& vectorcall Float4::operator+=(Float4 other)
	{
		return *this = *this + other;
	}

    
	forceinline Float4& vectorcall Float4::operator-=(float scalar)
	{
		return *this = *this - scalar;
	}

    
	forceinline Float4& vectorcall Float4::operator+=(float scalar)
	{
		return *this = *this + scalar;
	}
    

	forceinline Float4& vectorcall Float4::operator*=(Float4 other)
	{
		return *this = *this * other;
	}
    

	forceinline Float4& vectorcall Float4::operator/=(Float4 other)
	{
		return *this = *this / other;
	}

    
	forceinline Float4& vectorcall Float4::operator*=(float scalar)
	{
		return *this = *this * scalar;
	}

    
	forceinline Float4& vectorcall Float4::operator/=(float scalar)
	{
		return *this = *this / scalar;
	}
    
	
	forceinline bool vectorcall Float4::operator==(Float4 other) const
	{
		return Equals(other);
	}

    
	forceinline bool vectorcall Float4::operator!=(Float4 other) const
	{
		return !Equals(other);
	}
    

	forceinline Float4 vectorcall Load(Vec4f storage)
	{
		return Float4(storage.x, storage.y, storage.z, storage.w);
	}
    

	forceinline Vec4f vectorcall Store(Float4 f)
	{
		Vec4f result;
		result.x = f.GetX();
		result.y = f.GetY();
		result.z = f.GetZ();
		result.w = f.GetW();
		return result;
	}

    
	forceinline Float4 vectorcall operator-(Float4 left, Float4 right)
	{
		return left.Subtract(right);
	}

    
	forceinline Float4 vectorcall operator+(Float4 left, Float4 right)
	{
		return left.Add(right);
	}

    
	forceinline Float4 vectorcall operator-(Float4 left, float right)
	{
		return left.Subtract(right);
	}

    
	forceinline Float4 vectorcall operator+(Float4 left, float right)
	{
		return left.Add(right);
	}
    

	forceinline Float4 vectorcall operator*(Float4 left, Float4 right)
	{
		return left.Multiply(right);
	}

    
	forceinline Float4 vectorcall operator/(Float4 left, Float4 right)
	{
		return left.Divide(right);
	}

    
	forceinline Float4 vectorcall operator*(Float4 left, float right)
	{
		return left.Multiply(right);
	}

    
	forceinline Float4 vectorcall operator/(Float4 left, float right)
	{
		return left.Divide(right);
	}

    
	forceinline Float4 vectorcall operator*(float left, Float4 right)
	{
		return right.Multiply(left);
	}
}
