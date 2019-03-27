#pragma once
#include "Int3.h"

namespace Math
{
	///Non SIMD version
	///Should be used when sending data to shaders and generally for storage purposes
	struct Vec4i
	{
	public:
		forceinline bool vectorcall operator==(Vec4i other) const;
		forceinline bool vectorcall operator!=(Vec4i other) const;

		friend forceinline std::string vectorcall ToString(Vec4i v);

	public:
		union
		{
			struct
			{
				int32 x;
				int32 y;
				int32 z;
				int32 w;
			};

			struct
			{
				int32 r;
				int32 g;
				int32 b;
				int32 a;
			};
		};
	};

	forceinline std::string vectorcall ToString(Vec4i v)
	{
		return '(' + std::to_string(v.x) + ", " + std::to_string(v.y) + ", " + std::to_string(v.z) + ", " + std::to_string(v.w) + ')';
	}

	forceinline bool vectorcall Math::Vec4i::operator==(Vec4i other) const
	{
		return (x == other.x) && (y == other.y) && (z == other.z) && (w == other.w);
	}

	forceinline bool vectorcall Vec4i::operator!=(Vec4i other) const
	{
		return !(*this == other);
	}


	///SIMD version
	struct VECTORALIGN Int4
	{
	public:
		forceinline explicit Int4(int32 scalar = 0.0f);
		forceinline explicit Int4(int32 x, int32 y, int32 z, int32 w);
		forceinline explicit Int4(Int2 xy, Int2 zw);
		forceinline explicit Int4(Int3 xyw, int32 w);
		forceinline explicit Int4(SSE_VARIABLE_I(xmm));

		forceinline void vectorcall SetX(int32 x);
		forceinline void vectorcall SetY(int32 y);
		forceinline void vectorcall SetZ(int32 z);
		forceinline void vectorcall SetW(int32 w);
		forceinline int32 vectorcall GetX() const;
		forceinline int32 vectorcall GetY() const;
		forceinline int32 vectorcall GetZ() const;
		forceinline int32 vectorcall GetW() const;

		forceinline Int4& vectorcall Add(Int4 other);
		forceinline Int4& vectorcall Add(int32 scalar);
		forceinline Int4& vectorcall Subtract(Int4 other);
		forceinline Int4& vectorcall Subtract(int32 scalar);

		forceinline bool vectorcall Equals(Int4 other) const;

		forceinline Int4 vectorcall operator-() const;
		forceinline Int4& vectorcall operator=(Int4 other);
		forceinline Int4& vectorcall operator-=(Int4 other);
		forceinline Int4& vectorcall operator+=(Int4 other);
		forceinline Int4& vectorcall operator-=(int32 other);
		forceinline Int4& vectorcall operator+=(int32 other);

		forceinline bool vectorcall operator==(Int4 other) const;
		forceinline bool vectorcall operator!=(Int4 other) const;

		forceinline int32 vectorcall operator[](uint32 index);

		friend forceinline std::string vectorcall ToString(Int4 f);
		friend forceinline Vec4i vectorcall Store(Int4 f);
		friend forceinline Int4 vectorcall Load(Vec4i storage);
		friend forceinline Int4 vectorcall operator-(Int4 left, Int4 right);
		friend forceinline Int4 vectorcall operator+(Int4 left, Int4 right);
		friend forceinline Int4 vectorcall operator-(Int4 left, int32 right);
		friend forceinline Int4 vectorcall operator+(Int4 left, int32 right);

	private:
		SSE_VARIABLE_I(m_Xmm);
	};

#if defined(SSE_INTRIN)
	forceinline Int4::Int4(int32 scalar)
	{
		m_Xmm = _mm_set_epi32(0, scalar, scalar, scalar);
	}

	forceinline Int4::Int4(int32 x, int32 y, int32 z, int32 w)
	{
		m_Xmm = _mm_set_epi32(w, z, y, x);
	}

	forceinline Int4::Int4(Int2 xy, Int2 zw)
	{
		m_Xmm = _mm_set_epi32(zw.GetY(), zw.GetX(), xy.GetY(), xy.GetX());
	}

	forceinline Int4::Int4(Int3 xyw, int32 w)
	{
		m_Xmm = _mm_set_epi32(w, xyw.GetZ(), xyw.GetY(), xyw.GetX());
	}

	forceinline Int4::Int4(SSE_VARIABLE_I(xmm))
	{
		m_Xmm = xmm;
	}

	forceinline void vectorcall Int4::SetX(int32 x)
	{
		//Cast to float - No conversion, basically (float*)&y
		__m128 a = _mm_castsi128_ps(_mm_cvtsi32_si128(x));
		__m128 m = _mm_castsi128_ps(m_Xmm);
		//Move into variable
		m_Xmm = _mm_castps_si128(_mm_move_ss(m, a));
	}

	forceinline void vectorcall Int4::SetY(int32 y)
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

	forceinline void vectorcall Int4::SetZ(int32 z)
	{
		//Cast to float - No conversion, basically (float*)&y
		__m128 a = _mm_castsi128_ps(_mm_cvtsi32_si128(z));
		__m128 m = _mm_castsi128_ps(m_Xmm);
		//t = [z, this->y, this->z, this->w]
		__m128 t = _mm_move_ss(m, a);
		//t = [z, this->y, z, this->w]
		t = _mm_shuffle_ps(t, t, _MM_SHUFFLE(3, 0, 1, 0));
		//m_Xmm = [this->x, this->y, z, this->w]
		m_Xmm = _mm_castps_si128(_mm_move_ss(t, m));
	}

	forceinline void vectorcall Int4::SetW(int32 w)
	{
		//Cast to float - No conversion, basically (float*)&y
		__m128 a = _mm_castsi128_ps(_mm_cvtsi32_si128(w));
		__m128 m = _mm_castsi128_ps(m_Xmm);
		//t = [w, this->y, this->z, this->w]
		__m128 t = _mm_move_ss(m, a);
		//t = [w, this->y, this->z, w]
		t = _mm_shuffle_ps(t, t, _MM_SHUFFLE(0, 2, 1, 0));
		//m_Xmm = [this->x, this->y, z, this->w]
		m_Xmm = _mm_castps_si128(_mm_move_ss(t, m));
	}

	forceinline int32 vectorcall Int4::GetX() const
	{
		return _mm_cvtsi128_si32(m_Xmm);
	}

	forceinline int32 vectorcall Int4::GetY() const
	{
		return _mm_cvtsi128_si32(_mm_shuffle_epi32(m_Xmm, _MM_SHUFFLE(3, 2, 1, 1)));
	}

	forceinline int32 vectorcall Int4::GetZ() const
	{
		return _mm_cvtsi128_si32(_mm_shuffle_epi32(m_Xmm, _MM_SHUFFLE(3, 2, 1, 2)));
	}

	forceinline int32 vectorcall Int4::GetW() const
	{
		return _mm_cvtsi128_si32(_mm_shuffle_epi32(m_Xmm, _MM_SHUFFLE(3, 2, 1, 3)));
	}

	forceinline Int4& vectorcall Int4::Add(Int4 other)
	{
		m_Xmm = _mm_add_epi32(m_Xmm, other.m_Xmm);
		return *this;
	}

	forceinline Int4& vectorcall Int4::Add(int32 scalar)
	{
		m_Xmm = _mm_add_epi32(m_Xmm, _mm_set1_epi32(scalar));
		return *this;
	}

	forceinline Int4& vectorcall Int4::Subtract(Int4 other)
	{
		m_Xmm = _mm_sub_epi32(m_Xmm, other.m_Xmm);
		return *this;
	}

	forceinline Int4& vectorcall Int4::Subtract(int32 scalar)
	{
		m_Xmm = _mm_sub_epi32(m_Xmm, _mm_set1_epi32(scalar));
		return *this;
	}

	forceinline bool vectorcall Int4::Equals(Int4 other) const
	{
		int32 mask = _mm_movemask_epi8(_mm_cmpeq_epi32(m_Xmm, other.m_Xmm));
		return mask == 0xffff;
	}

	forceinline Int4 vectorcall Int4::operator-() const
	{
		__m128i zero = _mm_setzero_si128();
		__m128i result = _mm_sub_epi32(zero, m_Xmm);
		return Int4(result);
	}

	forceinline Int4& vectorcall Int4::operator=(Int4 other)
	{
		m_Xmm = other.m_Xmm;
		return *this;
	}

	forceinline int32 vectorcall Int4::operator[](uint32 index)
	{
		assert(index < 4);
		return m_Xmm.m128i_i32[index];
	}
#else
	forceinline Int4::Int4(int32 scalar)
	{
		m_Xmm[0] = scalar;
		m_Xmm[1] = scalar;
		m_Xmm[2] = scalar;
		m_Xmm[3] = scalar;
	}

	forceinline Int4::Int4(int32 x, int32 y, int32 z, int32 w)
	{
		m_Xmm[0] = x;
		m_Xmm[1] = y;
		m_Xmm[2] = z;
		m_Xmm[3] = w;
	}

	forceinline Int4::Int4(Int2 xy, Int2 zw)
	{
		m_Xmm[0] = xy.GetX();
		m_Xmm[1] = xy.GetY();
		m_Xmm[2] = zw.GetX();
		m_Xmm[3] = zw.GetY();
	}

	forceinline Int4::Int4(Int3 xyw, int32 w)
	{
		m_Xmm[0] = xyw.GetX();
		m_Xmm[1] = xyw.GetY();
		m_Xmm[2] = xyw.GetZ();
		m_Xmm[3] = w;
	}

	forceinline Int4::Int4(SSE_VARIABLE_I(xmm))
	{
		memcpy(m_Xmm, xmm, sizeof(m_Xmm));
	}

	forceinline void vectorcall Int4::SetX(int32 x)
	{
		m_Xmm[0] = x;
	}

	forceinline void vectorcall Int4::SetY(int32 y)
	{
		m_Xmm[1] = y;
	}

	forceinline void vectorcall Int4::SetZ(int32 z)
	{
		m_Xmm[2] = z;
	}

	forceinline void vectorcall Int4::SetW(int32 w)
	{
		m_Xmm[3] = w;
	}

	forceinline int32 vectorcall Int4::GetX() const
	{
		return m_Xmm[0];
	}

	forceinline int32 vectorcall Int4::GetY() const
	{
		return m_Xmm[1];
	}

	forceinline int32 vectorcall Int4::GetZ() const
	{
		return m_Xmm[2];
	}

	forceinline int32 vectorcall Int4::GetW() const
	{
		return m_Xmm[3];
	}

	forceinline Int4& vectorcall Int4::Add(Int4 other)
	{
		m_Xmm[0] += other.m_Xmm[0];
		m_Xmm[1] += other.m_Xmm[1];
		m_Xmm[2] += other.m_Xmm[2];
		m_Xmm[3] += other.m_Xmm[3];
		return *this;
	}

	forceinline Int4& vectorcall Int4::Add(int32 scalar)
	{
		m_Xmm[0] += scalar;
		m_Xmm[1] += scalar;
		m_Xmm[2] += scalar;
		m_Xmm[3] += scalar;
		return *this;
	}

	forceinline Int4& vectorcall Int4::Subtract(Int4 other)
	{
		m_Xmm[0] -= other.m_Xmm[0];
		m_Xmm[1] -= other.m_Xmm[1];
		m_Xmm[2] -= other.m_Xmm[2];
		m_Xmm[3] -= other.m_Xmm[3];
		return *this;
	}

	forceinline Int4& vectorcall Int4::Subtract(int32 scalar)
	{
		m_Xmm[0] -= scalar;
		m_Xmm[1] -= scalar;
		m_Xmm[2] -= scalar;
		m_Xmm[3] -= scalar;
		return *this;
	}

	forceinline bool vectorcall Int4::Equals(Int4 other) const
	{
		return	(m_Xmm[0] == other.m_Xmm[0]) &&
				(m_Xmm[1] == other.m_Xmm[1]) &&
				(m_Xmm[2] == other.m_Xmm[2]) &&
				(m_Xmm[3] == other.m_Xmm[3]);
	}

	
	forceinline Int4 vectorcall Int4::operator-() const
	{
		Int4 result;
		result.m_Xmm[0] = -m_Xmm[0];
		result.m_Xmm[1] = -m_Xmm[1];
		result.m_Xmm[2] = -m_Xmm[2];
		result.m_Xmm[3] = -m_Xmm[3];
		return result;
	}

	forceinline Int4& vectorcall Int4::operator=(Int4 other)
	{
		memcpy(m_Xmm, other.m_Xmm, sizeof(other));
		return *this;
	}

	forceinline int32 vectorcall Int4::operator[](uint32 index)
	{
		return m_Xmm[index];
	}
#endif

	forceinline std::string vectorcall ToString(Int4 i)
	{
		return '(' + std::to_string(i.GetX()) + ", " + std::to_string(i.GetY()) + ", " + std::to_string(i.GetZ()) + ", " + std::to_string(i.GetW()) + ')';
	}

	forceinline Int4& vectorcall Int4::operator-=(Int4 other)
	{
		return *this = *this - other;
	}

	forceinline Int4& vectorcall Int4::operator+=(Int4 other)
	{
		return *this = *this + other;
	}
	
	forceinline bool vectorcall Int4::operator==(Int4 other) const
	{
		return Equals(other);
	}

	forceinline bool vectorcall Int4::operator!=(Int4 other) const
	{
		return !Equals(other);
	}

	forceinline Int4 vectorcall Load(Vec4i storage)
	{
		return Int4(storage.x, storage.y, storage.z, storage.w);
	}

	forceinline Vec4i vectorcall Store(Int4 i)
	{
		Vec4i result;
		result.x = i.GetX();
		result.y = i.GetY();
		result.z = i.GetZ();
		result.w = i.GetW();
		return result;
	}

	forceinline Int4 vectorcall Math::operator-(Int4 left, Int4 right)
	{
		return left.Subtract(right);
	}

	forceinline Int4 vectorcall operator+(Int4 left, Int4 right)
	{
		return left.Add(right);
	}

	forceinline Int4 vectorcall operator-(Int4 left, int32 right)
	{
		return left.Subtract(right);
	}

	forceinline Int4 vectorcall operator+(Int4 left, int32 right)
	{
		return left.Add(right);
	}
}