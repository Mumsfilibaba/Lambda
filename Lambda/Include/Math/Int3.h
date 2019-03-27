#pragma once
#include "Int2.h"

namespace Math
{
	///Non SIMD version
	///Should be used when sending data to shaders and generally for storage purposes
	struct Vec3i
	{
	public:
		forceinline bool vectorcall operator==(Vec3i other) const;
		forceinline bool vectorcall operator!=(Vec3i other) const;

		friend forceinline std::string vectorcall ToString(Vec3i v);

	public:
		union
		{
			struct
			{
				int32 x;
				int32 y;
				int32 z;
			};

			struct
			{
				int32 r;
				int32 g;
				int32 b;
			};
		};
	};

	forceinline std::string vectorcall ToString(Vec3i v)
	{
		return '(' + std::to_string(v.x) + ", " + std::to_string(v.y) + ", " + std::to_string(v.z) + ')';
	}

	forceinline bool vectorcall Math::Vec3i::operator==(Vec3i other) const
	{
		return (x == other.x) && (y == other.y) && (z == other.z);
	}

	forceinline bool vectorcall Vec3i::operator!=(Vec3i other) const
	{
		return !(*this == other);
	}


	///SIMD version
	struct VECTORALIGN Int3
	{
	public:
		forceinline explicit Int3(int32 scalar = 0.0f);
		forceinline explicit Int3(int32 x, int32 y, int32 z);
		forceinline explicit Int3(Int2 xy, int32 z);
		forceinline explicit Int3(SSE_VARIABLE_I(xmm));

		forceinline void vectorcall SetX(int32 x);
		forceinline void vectorcall SetY(int32 y);
		forceinline void vectorcall SetZ(int32 z);
		forceinline int32 vectorcall GetX() const;
		forceinline int32 vectorcall GetY() const;
		forceinline int32 vectorcall GetZ() const;

		forceinline Int3& vectorcall Add(Int3 other);
		forceinline Int3& vectorcall Add(int32 scalar);
		forceinline Int3& vectorcall Subtract(Int3 other);
		forceinline Int3& vectorcall Subtract(int32 scalar);

		forceinline bool vectorcall Equals(Int3 other) const;

		forceinline Int3 vectorcall operator-() const;
		forceinline Int3& vectorcall operator=(Int3 other);
		forceinline Int3& vectorcall operator-=(Int3 other);
		forceinline Int3& vectorcall operator+=(Int3 other);
		forceinline Int3& vectorcall operator-=(int32 other);
		forceinline Int3& vectorcall operator+=(int32 other);

		forceinline bool vectorcall operator==(Int3 other) const;
		forceinline bool vectorcall operator!=(Int3 other) const;

		forceinline int32 vectorcall operator[](uint32 index);

		friend forceinline std::string vectorcall ToString(Int3 f);
		friend forceinline Vec3i vectorcall Store(Int3 f);
		friend forceinline Int3 vectorcall Load(Vec3i storage);
		friend forceinline Int3 vectorcall operator-(Int3 left, Int3 right);
		friend forceinline Int3 vectorcall operator+(Int3 left, Int3 right);
		friend forceinline Int3 vectorcall operator-(Int3 left, int32 right);
		friend forceinline Int3 vectorcall operator+(Int3 left, int32 right);

	private:
		SSE_VARIABLE_I(m_Xmm);
	};

#if defined(SSE_INTRIN)
	forceinline Int3::Int3(int32 scalar)
	{
		m_Xmm = _mm_set_epi32(0, scalar, scalar, scalar);
	}

	forceinline Int3::Int3(int32 x, int32 y, int32 z)
	{
		m_Xmm = _mm_set_epi32(0, z, y, x);
	}

	forceinline Int3::Int3(Int2 xy, int32 z)
	{
		m_Xmm = _mm_set_epi32(0, z, xy.GetY(), xy.GetX());
	}

	forceinline Int3::Int3(SSE_VARIABLE_I(xmm))
	{
		m_Xmm = xmm;
	}

	forceinline void vectorcall Int3::SetX(int32 x)
	{
		//Cast to float - No conversion, basically (float*)&y
		__m128 a = _mm_castsi128_ps(_mm_cvtsi32_si128(x));
		__m128 m = _mm_castsi128_ps(m_Xmm);
		//Move into variable
		m_Xmm = _mm_castps_si128(_mm_move_ss(m, a));
	}
	
	forceinline void vectorcall Int3::SetY(int32 y)
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
	
	forceinline void vectorcall Int3::SetZ(int32 z)
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

	forceinline int32 vectorcall Int3::GetX() const
	{
		return _mm_cvtsi128_si32(m_Xmm);
	}

	forceinline int32 vectorcall Int3::GetY() const
	{
		return _mm_cvtsi128_si32(_mm_shuffle_epi32(m_Xmm, _MM_SHUFFLE(3, 2, 1, 1)));
	}

	forceinline int32 vectorcall Int3::GetZ() const
	{
		return _mm_cvtsi128_si32(_mm_shuffle_epi32(m_Xmm, _MM_SHUFFLE(3, 2, 1, 2)));
	}

	forceinline Int3& vectorcall Int3::Add(Int3 other)
	{
		m_Xmm = _mm_add_epi32(m_Xmm, other.m_Xmm);
		return *this;
	}

	forceinline Int3& vectorcall Int3::Add(int32 scalar)
	{
		m_Xmm = _mm_add_epi32(m_Xmm, _mm_set1_epi32(scalar));
		return *this;
	}

	forceinline Int3& vectorcall Int3::Subtract(Int3 other)
	{
		m_Xmm = _mm_sub_epi32(m_Xmm, other.m_Xmm);
		return *this;
	}

	forceinline Int3& vectorcall Int3::Subtract(int32 scalar)
	{
		m_Xmm = _mm_sub_epi32(m_Xmm, _mm_set1_epi32(scalar));
		return *this;
	}

	forceinline bool vectorcall Int3::Equals(Int3 other) const
	{
		int32 mask = _mm_movemask_epi8(_mm_cmpeq_epi32(m_Xmm, other.m_Xmm));
		return mask == 0xffff;
	}

	forceinline Int3 vectorcall Int3::operator-() const
	{
		__m128i zero = _mm_setzero_si128();
		__m128i result = _mm_sub_epi32(zero, m_Xmm);
		return Int3(result);
	}

	forceinline Int3& vectorcall Int3::operator=(Int3 other)
	{
		m_Xmm = other.m_Xmm;
		return *this;
	}

	forceinline int32 vectorcall Int3::operator[](uint32 index)
	{
		assert(index < 3);
		return m_Xmm.m128i_i32[index];
	}
#else
	forceinline Int3::Int3(int32 scalar)
	{
		m_Xmm[0] = scalar;
		m_Xmm[1] = scalar;
		m_Xmm[2] = scalar;
		m_Xmm[3] = 0;
	}

	forceinline Int3::Int3(int32 x, int32 y, int32 z)
	{
		m_Xmm[0] = x;
		m_Xmm[1] = y;
		m_Xmm[2] = z;
		m_Xmm[3] = 0;
	}

	forceinline Int3::Int3(Int2 xy, int32 z)
	{
		m_Xmm[0] = xy.GetX();
		m_Xmm[1] = xy.GetY();
		m_Xmm[2] = z;
		m_Xmm[3] = 0;
	}

	forceinline Int3::Int3(SSE_VARIABLE_I(xmm))
	{
		memcpy(m_Xmm, xmm, sizeof(m_Xmm));
	}

	forceinline void vectorcall Int3::SetX(int32 x)
	{
		m_Xmm[0] = x;
	}

	forceinline void vectorcall Int3::SetY(int32 y)
	{
		m_Xmm[1] = y;
	}

	forceinline void vectorcall Int3::SetZ(int32 z)
	{
		m_Xmm[2] = z;
	}

	forceinline int32 vectorcall Int3::GetX() const
	{
		return m_Xmm[0];
	}

	forceinline int32 vectorcall Int3::GetY() const
	{
		return m_Xmm[1];
	}

	forceinline int32 vectorcall Int3::GetZ() const
	{
		return m_Xmm[2];
	}

	forceinline Int3& vectorcall Int3::Add(Int3 other)
	{
		m_Xmm[0] += other.m_Xmm[0];
		m_Xmm[1] += other.m_Xmm[1];
		m_Xmm[2] += other.m_Xmm[2];
		return *this;
	}

	forceinline Int3& vectorcall Int3::Add(int32 scalar)
	{
		m_Xmm[0] += scalar;
		m_Xmm[1] += scalar;
		m_Xmm[2] += scalar;
		return *this;
	}

	forceinline Int3& vectorcall Int3::Subtract(Int3 other)
	{
		m_Xmm[0] -= other.m_Xmm[0];
		m_Xmm[1] -= other.m_Xmm[1];
		m_Xmm[2] -= other.m_Xmm[2];
		return *this;
	}

	forceinline Int3& vectorcall Int3::Subtract(int32 scalar)
	{
		m_Xmm[0] -= scalar;
		m_Xmm[1] -= scalar;
		m_Xmm[2] -= scalar;
		return *this;
	}

	forceinline bool vectorcall Int3::Equals(Int3 other) const
	{
		return	(m_Xmm[0] == other.m_Xmm[0]) &&
				(m_Xmm[1] == other.m_Xmm[1]) &&
				(m_Xmm[2] == other.m_Xmm[2]);
	}

	forceinline Int3 vectorcall Int3::operator-() const
	{
		Int3 result;
		result.m_Xmm[0] = -m_Xmm[0];
		result.m_Xmm[1] = -m_Xmm[1];
		result.m_Xmm[2] = -m_Xmm[2];
		return result;
	}

	forceinline Int3& vectorcall Int3::operator=(Int3 other)
	{
		memcpy(m_Xmm, other.m_Xmm, sizeof(other));
		return *this;
	}

	forceinline int32 vectorcall Int3::operator[](uint32 index)
	{
		assert(index < 3);
		return m_Xmm[index];
	}
#endif

	forceinline std::string vectorcall ToString(Int3 i)
	{
		return '(' + std::to_string(i.GetX()) + ", " + std::to_string(i.GetY()) + ", " + std::to_string(i.GetZ()) + ')';
	}

	forceinline Int3& vectorcall Int3::operator-=(Int3 other)
	{
		return *this = *this - other;
	}

	forceinline Int3& vectorcall Int3::operator+=(Int3 other)
	{
		return *this = *this + other;
	}

	forceinline Int3& vectorcall Int3::operator-=(int32 scalar)
	{
		return *this = *this - scalar;
	}

	forceinline Int3& vectorcall Int3::operator+=(int32 scalar)
	{
		return *this = *this + scalar;
	}
	
	forceinline bool vectorcall Int3::operator==(Int3 other) const
	{
		return Equals(other);
	}

	forceinline bool vectorcall Int3::operator!=(Int3 other) const
	{
		return !Equals(other);
	}

	forceinline Int3 vectorcall Load(Vec3i storage)
	{
		return Int3(storage.x, storage.y, storage.z);
	}

	forceinline Vec3i vectorcall Store(Int3 i)
	{
		Vec3i result;
		result.x = i.GetX();
		result.y = i.GetY();
		result.z = i.GetZ();
		return result;
	}

	forceinline Int3 vectorcall Math::operator-(Int3 left, Int3 right)
	{
		return left.Subtract(right);
	}

	forceinline Int3 vectorcall operator+(Int3 left, Int3 right)
	{
		return left.Add(right);
	}

	forceinline Int3 vectorcall operator-(Int3 left, int32 right)
	{
		return left.Subtract(right);
	}

	forceinline Int3 vectorcall operator+(Int3 left, int32 right)
	{
		return left.Add(right);
	}
}