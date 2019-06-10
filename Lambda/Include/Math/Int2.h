#pragma once
#include "MathCommon.h"
#include <iostream>

namespace Math
{
	///Non SIMD version
	///Should be used when sending data to shaders and generally for storage purposes
	struct Vec2i
	{
	public:
		forceinline bool vectorcall operator==(Vec2i other) const;
		forceinline bool vectorcall operator!=(Vec2i other) const;

		friend forceinline std::string vectorcall ToString(Vec2i v);

	public:
		union
		{
			struct
			{
				int32 x;
				int32 y;
			};

			struct
			{
				int32 r;
				int32 g;
			};

			struct
			{
				int32 u;
				int32 v;
			};
		};
	};

    
	forceinline std::string vectorcall ToString(Vec2i v)
	{
		return '(' + std::to_string(v.x) + ", " + std::to_string(v.y) + ')';
	}
    

	forceinline bool vectorcall Math::Vec2i::operator==(Vec2i other) const
	{
		return (x == other.x) && (y == other.y);
	}

    
	forceinline bool vectorcall Vec2i::operator!=(Vec2i other) const
	{
		return !(*this == other);
	}


	///SIMD version
	struct VECTORALIGN Int2
	{
	public:
		forceinline explicit Int2(int32 scalar = 0.0f);
		forceinline explicit Int2(int32 x, int32 y);
		forceinline explicit Int2(SSE_VARIABLE_I(xmm));

		forceinline void  vectorcall SetX(int32 x);
		forceinline void  vectorcall SetY(int32 y);
		forceinline int32 vectorcall GetX() const;
		forceinline int32 vectorcall GetY() const;

		forceinline Int2& vectorcall Add(Int2 other);
		forceinline Int2& vectorcall Add(int32 scalar);
		forceinline Int2& vectorcall Subtract(Int2 other);
		forceinline Int2& vectorcall Subtract(int32 scalar);

		forceinline bool vectorcall Equals(Int2 other) const;

		forceinline Int2  vectorcall operator-() const;
		forceinline Int2& vectorcall operator=(Int2 other);
		forceinline Int2& vectorcall operator-=(Int2 other);
		forceinline Int2& vectorcall operator+=(Int2 other);
		forceinline Int2& vectorcall operator-=(int32 other);
		forceinline Int2& vectorcall operator+=(int32 other);

		forceinline bool vectorcall operator==(Int2 other) const;
		forceinline bool vectorcall operator!=(Int2 other) const;

		forceinline int32 vectorcall operator[](uint32 index);

		friend forceinline std::string	vectorcall ToString(Int2 i);
		friend forceinline Vec2i		vectorcall Store(Int2 i);
		friend forceinline Int2			vectorcall Load(Vec2i storage);
		friend forceinline Int2			vectorcall operator-(Int2 left, Int2 right);
		friend forceinline Int2			vectorcall operator+(Int2 left, Int2 right);
		friend forceinline Int2			vectorcall operator-(Int2 left, int32 right);
		friend forceinline Int2			vectorcall operator+(Int2 left, int32 right);

	private:
		SSE_VARIABLE_I(m_Xmm);
	};

#if defined(SSE_INTRIN)
	forceinline Int2::Int2(int32 scalar)
	{
		m_Xmm = _mm_set_epi32(0, 0, scalar, scalar);
	}

	forceinline Int2::Int2(int32 x, int32 y)
	{
		m_Xmm = _mm_set_epi32(0, 0, y, x);
	}

	forceinline Int2::Int2(SSE_VARIABLE_I(xmm))
	{
		m_Xmm = xmm;
	}

	forceinline void vectorcall Int2::SetX(int32 x)
	{
		//Cast to float - No conversion, basically (float*)&y
		__m128 a =	_mm_castsi128_ps(_mm_cvtsi32_si128(x));
		__m128 m =	_mm_castsi128_ps(m_Xmm);
		//Move into variable
		m_Xmm =		_mm_castps_si128(_mm_move_ss(m, a));
	}
	
	forceinline void vectorcall Int2::SetY(int32 y)
	{
		//Cast to float - No conversion, basically (float*)&y
		__m128 a = _mm_castsi128_ps(_mm_cvtsi32_si128(y));
		__m128 m = _mm_castsi128_ps(m_Xmm);
		//t = [y, this->y, this->z, this->w]
		__m128 t = _mm_move_ss(m, a);
		//t = [y, y, this->z, this->w]
		t = _mm_shuffle_ps(t, t, _MM_SHUFFLE(3, 2, 0, 0));
		//m_Xmm = [this->x, y, this->z, this->w]
		m_Xmm = _mm_castps_si128(_mm_move_ss(t, m));
	}

	forceinline int32 vectorcall Int2::GetX() const
	{
		return _mm_cvtsi128_si32(m_Xmm);
	}

	forceinline int32 vectorcall Int2::GetY() const
	{
		return _mm_cvtsi128_si32(_mm_shuffle_epi32(m_Xmm, _MM_SHUFFLE(3, 2, 1, 1)));
	}

	forceinline Int2& vectorcall Int2::Add(Int2 other)
	{
		m_Xmm = _mm_add_epi32(m_Xmm, other.m_Xmm);
		return *this;
	}

	forceinline Int2& vectorcall Int2::Add(int32 scalar)
	{
		m_Xmm = _mm_add_epi32(m_Xmm, _mm_set1_epi32(scalar));
		return *this;
	}

	forceinline Int2& vectorcall Int2::Subtract(Int2 other)
	{
		m_Xmm = _mm_sub_epi32(m_Xmm, other.m_Xmm);
		return *this;
	}

	forceinline Int2& vectorcall Int2::Subtract(int32 scalar)
	{
		m_Xmm = _mm_sub_epi32(m_Xmm, _mm_set1_epi32(scalar));
		return *this;
	}

	forceinline bool vectorcall Int2::Equals(Int2 other) const
	{
		int32 mask = _mm_movemask_epi8(_mm_cmpeq_epi32(m_Xmm, other.m_Xmm));
		return mask == 0xffff;
	}

	forceinline Int2 vectorcall Int2::operator-() const
	{
		__m128i zero = _mm_setzero_si128();
		__m128i result = _mm_sub_epi32(zero, m_Xmm);
		return Int2(result);
	}

	forceinline Int2& vectorcall Int2::operator=(Int2 other)
	{
		m_Xmm = other.m_Xmm;
		return *this;
	}

	forceinline int32 vectorcall Int2::operator[](uint32 index)
	{
		assert(index < 2);
		return m_Xmm.m128i_i32[index];
	}
#else
	forceinline Int2::Int2(int32 scalar)
	{
		m_Xmm[0] = scalar;
		m_Xmm[1] = scalar;
		m_Xmm[2] = 0;
		m_Xmm[3] = 0;
	}

	forceinline Int2::Int2(int32 x, int32 y)
	{
		m_Xmm[0] = x;
		m_Xmm[1] = y;
		m_Xmm[2] = 0;
		m_Xmm[3] = 0;
	}

	forceinline Int2::Int2(SSE_VARIABLE_I(xmm))
	{
		memcpy(m_Xmm, xmm, sizeof(m_Xmm));
	}

	forceinline void vectorcall Int2::SetX(int32 x)
	{
		m_Xmm[0] = x;
	}

	forceinline void vectorcall Int2::SetY(int32 y)
	{
		m_Xmm[1] = y;
	}

	forceinline int32 vectorcall Int2::GetX() const
	{
		return m_Xmm[0];
	}

	forceinline int32 vectorcall Int2::GetY() const
	{
		return m_Xmm[1];
	}

	forceinline Int2& vectorcall Int2::Add(Int2 other)
	{
		m_Xmm[0] += other.m_Xmm[0];
		m_Xmm[1] += other.m_Xmm[1];
		return *this;
	}

	forceinline Int2& vectorcall Int2::Add(int32 scalar)
	{
		m_Xmm[0] += scalar;
		m_Xmm[1] += scalar;
		return *this;
	}

	forceinline Int2& vectorcall Int2::Subtract(Int2 other)
	{
		m_Xmm[0] -= other.m_Xmm[0];
		m_Xmm[1] -= other.m_Xmm[1];
		return *this;
	}

	forceinline Int2& vectorcall Int2::Subtract(int32 scalar)
	{
		m_Xmm[0] -= scalar;
		m_Xmm[1] -= scalar;
		return *this;
	}

	forceinline bool vectorcall Int2::Equals(Int2 other) const
	{
		return	(m_Xmm[0] == other.m_Xmm[0]) &&
				(m_Xmm[1] == other.m_Xmm[1]) &&
				(m_Xmm[2] == other.m_Xmm[2]);
	}

	forceinline Int2 vectorcall Int2::operator-() const
	{
		Int2 result;
		result.m_Xmm[0] = -m_Xmm[0];
		result.m_Xmm[1] = -m_Xmm[1];
		result.m_Xmm[2] = -m_Xmm[2];
		return result;
	}

	forceinline Int2& vectorcall Int2::operator=(Int2 other)
	{
		memcpy(m_Xmm, other.m_Xmm, sizeof(other));
		return *this;
	}

	forceinline int32 vectorcall Int2::operator[](uint32 index)
	{
		assert(index < 2);
		return m_Xmm[index];
	}
#endif

	forceinline std::string vectorcall ToString(Int2 i)
	{
		return '(' + std::to_string(i.GetX()) + ", " + std::to_string(i.GetY()) + ')';
	}

	forceinline Int2& vectorcall Int2::operator-=(Int2 other)
	{
		return *this = *this - other;
	}

	forceinline Int2& vectorcall Int2::operator+=(Int2 other)
	{
		return *this = *this + other;
	}

	forceinline Int2& vectorcall Int2::operator-=(int32 scalar)
	{
		return *this = *this - scalar;
	}

	forceinline Int2& vectorcall Int2::operator+=(int32 scalar)
	{
		return *this = *this + scalar;
	}
	
	forceinline bool vectorcall Int2::operator==(Int2 other) const
	{
		return Equals(other);
	}

	forceinline bool vectorcall Int2::operator!=(Int2 other) const
	{
		return !Equals(other);
	}

	forceinline Int2 vectorcall Load(Vec2i storage)
	{
		return Int2(storage.x, storage.y);
	}

	forceinline Vec2i vectorcall Store(Int2 i)
	{
		Vec2i result;
		result.x = i.GetX();
		result.y = i.GetY();
		return result;
	}

	forceinline Int2 vectorcall Math::operator-(Int2 left, Int2 right)
	{
		return left.Subtract(right);
	}

	forceinline Int2 vectorcall operator+(Int2 left, Int2 right)
	{
		return left.Add(right);
	}

	forceinline Int2 vectorcall operator-(Int2 left, int32 right)
	{
		return left.Subtract(right);
	}

	forceinline Int2 vectorcall operator+(Int2 left, int32 right)
	{
		return left.Add(right);
	}
}
