#pragma once
#include "Float4.h"

namespace Math
{
	///Non SIMD version
	///Should be used when sending data to shaders and generally for storage purposes
	struct MATRIXALIGN Mat4f
	{
	public:
		forceinline bool vectorcall operator==(Mat4f other) const;
		forceinline bool vectorcall operator!=(Mat4f other) const;

		friend forceinline std::string vectorcall ToString(Mat4f f);

	public:
		float m[16];
	};

    
	forceinline bool vectorcall Mat4f::operator==(Mat4f other) const
	{
		for (int i = 0; i < 16; i++)
		{
			if (m[i] != other.m[i])
				return false;
		}
		return true;
	}

    
	forceinline bool vectorcall Mat4f::operator!=(Mat4f other) const
	{
		return !(*this == other);
	}

    
	forceinline std::string vectorcall ToString(Mat4f f)
	{
		return
			'[' + std::to_string(f.m[ 0]) + ", " + std::to_string(f.m[ 1]) + ", " + std::to_string(f.m[ 2]) + ", " + std::to_string(f.m[ 3]) + ']' +
			'[' + std::to_string(f.m[ 4]) + ", " + std::to_string(f.m[ 5]) + ", " + std::to_string(f.m[ 6]) + ", " + std::to_string(f.m[ 7]) + ']' +
			'[' + std::to_string(f.m[ 8]) + ", " + std::to_string(f.m[ 9]) + ", " + std::to_string(f.m[10]) + ", " + std::to_string(f.m[11]) + ']' +
			'[' + std::to_string(f.m[12]) + ", " + std::to_string(f.m[13]) + ", " + std::to_string(f.m[14]) + ", " + std::to_string(f.m[15]) + ']';
	}
    

	///SIMD version
	struct MATRIXALIGN Float4x4
	{
	public:
		forceinline explicit Float4x4(float diagonal = 1.0f);
		forceinline explicit Float4x4(Float4 r0, Float4 r1, Float4 r2, Float4 r3);
		forceinline explicit Float4x4(SSE_VARIABLE(r0), SSE_VARIABLE(r1), SSE_VARIABLE(r2), SSE_VARIABLE(r3));

		forceinline Float4	  vectorcall Multiply(Float4 vector) const;
		forceinline Float4x4& vectorcall Multiply(Float4x4 other);
		forceinline Float4x4& vectorcall Multiply(float scalar);
		forceinline Float4x4& vectorcall Add(Float4x4 other);
		forceinline Float4x4& vectorcall Add(float scalar);
		forceinline Float4x4& vectorcall Subtract(Float4x4 other);
		forceinline Float4x4& vectorcall Subtract(float scalar);
		forceinline Float4x4& vectorcall Divide(float scalar);

		forceinline bool vectorcall Equals(Float4x4 other) const;

		forceinline void	vectorcall SetRow(uint32 r, float x, float y, float z, float w);
		forceinline float	vectorcall GetElement(uint32 r, uint32 c) const;
		forceinline Float4	vectorcall GetRow(uint32 r) const;

		Float4x4	vectorcall GetTranspose() const;
		Float4x4	vectorcall GetInverse() const;
		Float4x4&	vectorcall Transpose();
		Float4x4&	vectorcall Invert();

		Float4x4 vectorcall Cofactor() const;
		Float4x4 vectorcall Adjugate() const;

		float vectorcall Determinant() const;

		forceinline bool vectorcall operator==(Float4x4 other) const;
		forceinline bool vectorcall operator!=(Float4x4 other) const;

		forceinline float operator()(uint32 r, uint32 c) const;
		forceinline Float4 operator()(uint32 r) const;

		forceinline Float4x4& vectorcall operator=(Float4x4 other);
		forceinline Float4x4& vectorcall operator+=(Float4x4 right);
		forceinline Float4x4& vectorcall operator-=(Float4x4 right);
		forceinline Float4x4& vectorcall operator*=(Float4x4 right);
		forceinline Float4x4& vectorcall operator+=(float right);
		forceinline Float4x4& vectorcall operator-=(float right);
		forceinline Float4x4& vectorcall operator*=(float right);
		forceinline Float4x4& vectorcall operator/=(float right);

		friend forceinline std::string	vectorcall ToString(Float4x4 f);
		friend forceinline Mat4f		vectorcall Store(Float4x4 f);
		friend forceinline Float4x4		vectorcall Load(Mat4f storage);
		friend forceinline Float4		vectorcall operator*(Float4 left, Float4x4 right);
		friend forceinline Float4&		vectorcall operator*=(Float4& left, Float4x4 right);
		friend forceinline Float4x4		vectorcall operator+(Float4x4 left, Float4x4 right);
		friend forceinline Float4x4		vectorcall operator-(Float4x4 left, Float4x4 right);
		friend forceinline Float4x4		vectorcall operator*(Float4x4 left, Float4x4 right);
		friend forceinline Float4x4		vectorcall operator+(Float4x4 left, float right);
		friend forceinline Float4x4		vectorcall operator-(Float4x4 left, float right);
		friend forceinline Float4x4		vectorcall operator*(Float4x4 left, float right);
		friend forceinline Float4x4		vectorcall operator/(Float4x4 left, float right);
		friend forceinline Float4x4		vectorcall operator*(float left, Float4x4 right);

	private:
		SSE_ARRAY(m_Xmm, 4);

	public:
		static forceinline Float4x4 vectorcall Identity();

		static forceinline Float4x4 vectorcall Perspective(float fovRad, float aspectWiHe, float farZ, float nearZ);
		static forceinline Float4x4 vectorcall Orthographic(float aspectWiHe, float farZ, float nearZ);
		static forceinline Float4x4 vectorcall Orthographic(float width, float height, float farZ, float nearZ);
		static forceinline Float4x4 vectorcall Orthographic(float leftX, float rightX, float topY, float bottomY, float farZ, float nearZ);
		static forceinline Float4x4 vectorcall LookAt(Vec3f up, Vec3f at, Vec3f position);

		static forceinline Float4x4 vectorcall Translation(Vec3f translation);

		static forceinline Float4x4 vectorcall RotationRollPitchYaw(float angleRadZ, float angleRadX, float angleRadY);
		static forceinline Float4x4 vectorcall Rotation(Vec3f axis, float angleRad);
		static forceinline Float4x4 vectorcall RotationX(float angleRad);
		static forceinline Float4x4 vectorcall RotationY(float angleRad);
		static forceinline Float4x4 vectorcall RotationZ(float angleRad);

		static forceinline Float4x4 vectorcall Scale(Vec3f scale);
		static forceinline Float4x4 vectorcall Scale(float scale);
	};

    
#if defined(SSE_INTRIN)
	forceinline Float4x4::Float4x4(float diagonal)
	{
		m_Xmm[0] = _mm_set_ps(0.0f, 0.0f, 0.0f, diagonal);
		m_Xmm[1] = _mm_set_ps(0.0f, 0.0f, diagonal, 0.0f);
		m_Xmm[2] = _mm_set_ps(0.0f, diagonal, 0.0f, 0.0f);
		m_Xmm[3] = _mm_set_ps(diagonal, 0.0f, 0.0f, 0.0f);
	}
    
    
	forceinline Float4x4::Float4x4(Float4 r0, Float4 r1, Float4 r2, Float4 r3)
	{
		m_Xmm[0] = r0.m_Xmm;
		m_Xmm[1] = r1.m_Xmm;
		m_Xmm[2] = r2.m_Xmm;
		m_Xmm[3] = r3.m_Xmm;
	}
    

	forceinline Float4x4::Float4x4(SSE_VARIABLE(r0), SSE_VARIABLE(r1), SSE_VARIABLE(r2), SSE_VARIABLE(r3))
	{
		m_Xmm[0] = r0;
		m_Xmm[1] = r1;
		m_Xmm[2] = r2;
		m_Xmm[3] = r3;
	}

    
	forceinline Float4 vectorcall Float4x4::Multiply(Float4 vector) const
	{
		__m128 r0 = __m128RowAdd(m_Xmm, vector.m_Xmm);
		return Float4(r0);
	}

    
	forceinline Float4x4& vectorcall Float4x4::Multiply(Float4x4 other)
	{
		__m128 r0 = __m128RowAdd(other.m_Xmm, m_Xmm[0]);
		__m128 r1 = __m128RowAdd(other.m_Xmm, m_Xmm[1]);
		__m128 r2 = __m128RowAdd(other.m_Xmm, m_Xmm[2]);
		__m128 r3 = __m128RowAdd(other.m_Xmm, m_Xmm[3]);
		
		m_Xmm[0] = r0;
		m_Xmm[1] = r1;
		m_Xmm[2] = r2;
		m_Xmm[3] = r3;

		return *this;
	}

    
	forceinline Float4x4& vectorcall Float4x4::Multiply(float scalar)
	{
		__m128 scalars	= _mm_set_ps1(scalar);
		__m128 r0		= _mm_mul_ps(m_Xmm[0], scalars);
		__m128 r1		= _mm_mul_ps(m_Xmm[1], scalars);
		__m128 r2		= _mm_mul_ps(m_Xmm[2], scalars);
		__m128 r3		= _mm_mul_ps(m_Xmm[3], scalars);
		
		m_Xmm[0] = r0;
		m_Xmm[1] = r1;
		m_Xmm[2] = r2;
		m_Xmm[3] = r3;

		return *this;
	}

    
	forceinline Float4x4& vectorcall Float4x4::Add(Float4x4 other)
	{
		__m128 r0 = _mm_add_ps(m_Xmm[0], other.m_Xmm[0]);
		__m128 r1 = _mm_add_ps(m_Xmm[1], other.m_Xmm[1]);
		__m128 r2 = _mm_add_ps(m_Xmm[2], other.m_Xmm[2]);
		__m128 r3 = _mm_add_ps(m_Xmm[3], other.m_Xmm[3]);

		m_Xmm[0] = r0;
		m_Xmm[1] = r1;
		m_Xmm[2] = r2;
		m_Xmm[3] = r3;

		return *this;
	}

    
	forceinline Float4x4& vectorcall Float4x4::Add(float scalar)
	{
		__m128 scalars	= _mm_set_ps1(scalar);
		__m128 r0		= _mm_add_ps(m_Xmm[0], scalars);
		__m128 r1		= _mm_add_ps(m_Xmm[1], scalars);
		__m128 r2		= _mm_add_ps(m_Xmm[2], scalars);
		__m128 r3		= _mm_add_ps(m_Xmm[3], scalars);

		m_Xmm[0] = r0;
		m_Xmm[1] = r1;
		m_Xmm[2] = r2;
		m_Xmm[3] = r3;

		return *this;
	}

    
	forceinline Float4x4& vectorcall Float4x4::Subtract(Float4x4 other)
	{
		__m128 r0 = _mm_sub_ps(m_Xmm[0], other.m_Xmm[0]);
		__m128 r1 = _mm_sub_ps(m_Xmm[1], other.m_Xmm[1]);
		__m128 r2 = _mm_sub_ps(m_Xmm[2], other.m_Xmm[2]);
		__m128 r3 = _mm_sub_ps(m_Xmm[3], other.m_Xmm[3]);

		m_Xmm[0] = r0;
		m_Xmm[1] = r1;
		m_Xmm[2] = r2;
		m_Xmm[3] = r3;

		return *this;
	}
    

	forceinline Float4x4& vectorcall Float4x4::Subtract(float scalar)
	{
		__m128 scalars	= _mm_set_ps1(scalar);
		__m128 r0		= _mm_sub_ps(m_Xmm[0], scalars);
		__m128 r1		= _mm_sub_ps(m_Xmm[1], scalars);
		__m128 r2		= _mm_sub_ps(m_Xmm[2], scalars);
		__m128 r3		= _mm_sub_ps(m_Xmm[3], scalars);

		m_Xmm[0] = r0;
		m_Xmm[1] = r1;
		m_Xmm[2] = r2;
		m_Xmm[3] = r3;

		return *this;
	}
    

	forceinline Float4x4& vectorcall Float4x4::Divide(float scalar)
	{
		__m128 scalars	= _mm_set_ps1(scalar);
		__m128 r0		= _mm_div_ps(m_Xmm[0], scalars);
		__m128 r1		= _mm_div_ps(m_Xmm[1], scalars);
		__m128 r2		= _mm_div_ps(m_Xmm[2], scalars);
		__m128 r3		= _mm_div_ps(m_Xmm[3], scalars);

		m_Xmm[0] = r0;
		m_Xmm[1] = r1;
		m_Xmm[2] = r2;
		m_Xmm[3] = r3;

		return *this;
	}

    
	forceinline bool vectorcall Float4x4::Equals(Float4x4 other) const
	{
		int32 m0 = _mm_movemask_ps(_mm_cmpeq_ps(m_Xmm[0], other.m_Xmm[0]));
		int32 m1 = _mm_movemask_ps(_mm_cmpeq_ps(m_Xmm[1], other.m_Xmm[1]));
		int32 m2 = _mm_movemask_ps(_mm_cmpeq_ps(m_Xmm[2], other.m_Xmm[2]));
		int32 m3 = _mm_movemask_ps(_mm_cmpeq_ps(m_Xmm[3], other.m_Xmm[3]));
		return	(0xf == (m0 & 0xf)) &&
				(0xf == (m1 & 0xf)) &&
				(0xf == (m2 & 0xf)) &&
				(0xf == (m3 & 0xf));
	}

    
	forceinline void vectorcall Float4x4::SetRow(uint32 r, float x, float y, float z, float w)
	{
		assert(r < 4);
		m_Xmm[r] = _mm_set_ps(w, z, y, x);
	}

    
	forceinline Float4 vectorcall Float4x4::GetRow(uint32 r) const
	{
		assert(r < 4);
		return Float4(m_Xmm[r]);
	}
    

	inline Float4x4& vectorcall Float4x4::Transpose()
	{
		_MM_TRANSPOSE4_PS(m_Xmm[0], m_Xmm[1], m_Xmm[2], m_Xmm[3]);
		return *this;
	}

    
	inline Float4x4& vectorcall Float4x4::Invert()
	{
		__m128 a = _mm_movelh_ps(m_Xmm[0], m_Xmm[1]);
		__m128 b = _mm_movehl_ps(m_Xmm[1], m_Xmm[0]);
		__m128 c = _mm_movelh_ps(m_Xmm[2], m_Xmm[3]);
		__m128 d = _mm_movehl_ps(m_Xmm[3], m_Xmm[2]);

		__m128 detSub = _mm_sub_ps(
			_mm_mul_ps(_mm_shuffle_ps(m_Xmm[0], m_Xmm[2], _MM_SHUFFLE(2, 0, 2, 0)), _mm_shuffle_ps(m_Xmm[1], m_Xmm[3], _MM_SHUFFLE(3, 1, 3, 1))),
			_mm_mul_ps(_mm_shuffle_ps(m_Xmm[0], m_Xmm[2], _MM_SHUFFLE(3, 1, 3, 1)), _mm_shuffle_ps(m_Xmm[1], m_Xmm[3], _MM_SHUFFLE(2, 0, 2, 0)))
		);

		__m128 detA = __m128Swizzle1(detSub, 0);
		__m128 detB = __m128Swizzle1(detSub, 1);
		__m128 detC = __m128Swizzle1(detSub, 2);
		__m128 detD = __m128Swizzle1(detSub, 3);

		__m128 dc = __m128Float2AdjMul(d, c);
		__m128 ab = __m128Float2AdjMul(a, b);

		__m128 x = _mm_sub_ps(_mm_mul_ps(detD, a), __m128Float2Mul(b, dc));
		__m128 w = _mm_sub_ps(_mm_mul_ps(detA, d), __m128Float2Mul(c, ab));

		__m128 detM = _mm_mul_ps(detA, detD);

		__m128 y = _mm_sub_ps(_mm_mul_ps(detB, c), __m128Float2MulAdj(d, ab));
		__m128 z = _mm_sub_ps(_mm_mul_ps(detC, b), __m128Float2MulAdj(a, dc));

		detM = _mm_add_ps(detM, _mm_mul_ps(detB, detC));

		__m128	trace = _mm_mul_ps(ab, __m128Swizzle(dc, 0, 2, 1, 3));
				trace = _mm_hadd_ps(trace, trace);
				trace = _mm_hadd_ps(trace, trace);
		
		detM = _mm_sub_ps(detM, trace);

		const __m128 adjSignMask = _mm_setr_ps(1.f, -1.f, -1.f, 1.f);
		detM = _mm_div_ps(adjSignMask, detM);

		x = _mm_mul_ps(x, detM);
		y = _mm_mul_ps(y, detM);
		z = _mm_mul_ps(z, detM);
		w = _mm_mul_ps(w, detM);

		m_Xmm[0] = _mm_shuffle_ps(x, y, _MM_SHUFFLE(1, 3, 1, 3));
		m_Xmm[1] = _mm_shuffle_ps(x, y, _MM_SHUFFLE(0, 2, 0, 2));
		m_Xmm[2] = _mm_shuffle_ps(z, w, _MM_SHUFFLE(1, 3, 1, 3));
		m_Xmm[3] = _mm_shuffle_ps(z, w, _MM_SHUFFLE(0, 2, 0, 2));
		
		return *this;
	}
    

	inline Float4x4 vectorcall Float4x4::Adjugate() const
	{
		__m128 a = _mm_movelh_ps(m_Xmm[0], m_Xmm[1]);
		__m128 b = _mm_movehl_ps(m_Xmm[1], m_Xmm[0]);
		__m128 c = _mm_movelh_ps(m_Xmm[2], m_Xmm[3]);
		__m128 d = _mm_movehl_ps(m_Xmm[3], m_Xmm[2]);

		__m128 detSub = _mm_sub_ps(
			_mm_mul_ps(_mm_shuffle_ps(m_Xmm[0], m_Xmm[2], _MM_SHUFFLE(2, 0, 2, 0)), _mm_shuffle_ps(m_Xmm[1], m_Xmm[3], _MM_SHUFFLE(3, 1, 3, 1))),
			_mm_mul_ps(_mm_shuffle_ps(m_Xmm[0], m_Xmm[2], _MM_SHUFFLE(3, 1, 3, 1)), _mm_shuffle_ps(m_Xmm[1], m_Xmm[3], _MM_SHUFFLE(2, 0, 2, 0)))
		);

		__m128 detA = __m128Swizzle1(detSub, 0);
		__m128 detB = __m128Swizzle1(detSub, 1);
		__m128 detC = __m128Swizzle1(detSub, 2);
		__m128 detD = __m128Swizzle1(detSub, 3);

		__m128 ab = __m128Float2AdjMul(a, b);
		__m128 dc = __m128Float2AdjMul(d, c);

		__m128 w = _mm_sub_ps(_mm_mul_ps(detA, d), __m128Float2Mul(c, ab));
		__m128 x = _mm_sub_ps(_mm_mul_ps(detD, a), __m128Float2Mul(b, dc));

		__m128 y = _mm_sub_ps(_mm_mul_ps(detB, c), __m128Float2MulAdj(d, ab));
		__m128 z = _mm_sub_ps(_mm_mul_ps(detC, b), __m128Float2MulAdj(a, dc));

		__m128 mask = _mm_setr_ps(1.0f, -1.0f, -1.0f, 1.0f);
		x = _mm_mul_ps(x, mask);
		y = _mm_mul_ps(y, mask);
		z = _mm_mul_ps(z, mask);
		w = _mm_mul_ps(w, mask);

		__m128 r0 = _mm_shuffle_ps(x, y, _MM_SHUFFLE(1, 3, 1, 3));
		__m128 r1 = _mm_shuffle_ps(x, y, _MM_SHUFFLE(0, 2, 0, 2));
		__m128 r2 = _mm_shuffle_ps(z, w, _MM_SHUFFLE(1, 3, 1, 3));
		__m128 r3 = _mm_shuffle_ps(z, w, _MM_SHUFFLE(0, 2, 0, 2));
		return Float4x4(r0, r1, r2, r3);
	}

    
	inline float vectorcall Float4x4::Determinant() const
	{
		__m128 a = _mm_movelh_ps(m_Xmm[0], m_Xmm[1]);
		__m128 b = _mm_movehl_ps(m_Xmm[1], m_Xmm[0]);
		__m128 c = _mm_movelh_ps(m_Xmm[2], m_Xmm[3]);
		__m128 d = _mm_movehl_ps(m_Xmm[3], m_Xmm[2]);

		__m128 detSub = _mm_sub_ps(
			_mm_mul_ps(_mm_shuffle_ps(m_Xmm[0], m_Xmm[2], _MM_SHUFFLE(2, 0, 2, 0)), _mm_shuffle_ps(m_Xmm[1], m_Xmm[3], _MM_SHUFFLE(3, 1, 3, 1))),
			_mm_mul_ps(_mm_shuffle_ps(m_Xmm[0], m_Xmm[2], _MM_SHUFFLE(3, 1, 3, 1)), _mm_shuffle_ps(m_Xmm[1], m_Xmm[3], _MM_SHUFFLE(2, 0, 2, 0)))
		);

		__m128 detA = __m128Swizzle1(detSub, 0);
		__m128 detB = __m128Swizzle1(detSub, 1);
		__m128 detC = __m128Swizzle1(detSub, 2);
		__m128 detD = __m128Swizzle1(detSub, 3);

		__m128 dc = __m128Float2AdjMul(d, c);
		__m128 ab = __m128Float2AdjMul(a, b);

		__m128 detM = _mm_mul_ps(detA, detD);
		detM = _mm_add_ps(detM, _mm_mul_ps(detB, detC));
		detM = _mm_sub_ps(detM, __m128HorizonatalSum(_mm_mul_ps(ab, __m128Swizzle(dc, 0, 2, 1, 3))));

		return _mm_cvtss_f32(detM);
	}
    

	forceinline Float4x4& vectorcall Float4x4::operator=(Float4x4 other)
	{
		m_Xmm[0] = other.m_Xmm[0];
		m_Xmm[1] = other.m_Xmm[1];
		m_Xmm[2] = other.m_Xmm[2];
		m_Xmm[3] = other.m_Xmm[3];
		return *this;
	}

    
	forceinline Mat4f vectorcall Store(Float4x4 f)
	{
		Mat4f result;
		_mm_store_ps(result.m,		f.m_Xmm[0]);
		_mm_store_ps(result.m + 4,	f.m_Xmm[1]);
		_mm_store_ps(result.m + 8,	f.m_Xmm[2]);
		_mm_store_ps(result.m + 12,	f.m_Xmm[3]);
		return result;
	}

    
	forceinline Float4x4 vectorcall Load(Mat4f storage)
	{
		Float4x4 result;
		result.m_Xmm[0] = _mm_load_ps(storage.m);
		result.m_Xmm[1] = _mm_load_ps(storage.m + 4);
		result.m_Xmm[2] = _mm_load_ps(storage.m + 8);
		result.m_Xmm[3] = _mm_load_ps(storage.m + 12);
		return result;
	}

#else
	forceinline Float4x4::Float4x4(float diagonal)
	{
		memset(m_Xmm, 0, sizeof(m_Xmm));

		m_Xmm[0] = diagonal;
		m_Xmm[5] = diagonal;
		m_Xmm[10] = diagonal;
		m_Xmm[15] = diagonal;
	}

    
	forceinline Float4x4::Float4x4(Float4 r0, Float4 r1, Float4 r2, Float4 r3)
	{
		memcpy(m_Xmm,		r0.m_Xmm, sizeof(r0.m_Xmm));
		memcpy(m_Xmm + 4,	r1.m_Xmm, sizeof(r1.m_Xmm));
		memcpy(m_Xmm + 8,	r2.m_Xmm, sizeof(r2.m_Xmm));
		memcpy(m_Xmm + 12,	r3.m_Xmm, sizeof(r3.m_Xmm));
	}
    

	forceinline Float4x4::Float4x4(SSE_VARIABLE(r0), SSE_VARIABLE(r1), SSE_VARIABLE(r2), SSE_VARIABLE(r3))
	{
		memcpy(m_Xmm,		r0, sizeof(r0));
		memcpy(m_Xmm + 4,	r1, sizeof(r1));
		memcpy(m_Xmm + 8,	r2, sizeof(r2));
		memcpy(m_Xmm + 12,	r3, sizeof(r3));
	}

    
	forceinline Float4 vectorcall Float4x4::Multiply(Float4 vector) const
	{
		float t[4];

		t[0] = (vector[0] * m_Xmm[0]) + (vector[1] * m_Xmm[4]) + (vector[2] * m_Xmm[ 8]) + (vector[3] * m_Xmm[12]);
		t[1] = (vector[0] * m_Xmm[1]) + (vector[1] * m_Xmm[5]) + (vector[2] * m_Xmm[ 9]) + (vector[3] * m_Xmm[13]);
		t[2] = (vector[0] * m_Xmm[2]) + (vector[1] * m_Xmm[6]) + (vector[2] * m_Xmm[10]) + (vector[3] * m_Xmm[14]);
		t[3] = (vector[0] * m_Xmm[3]) + (vector[1] * m_Xmm[7]) + (vector[2] * m_Xmm[11]) + (vector[3] * m_Xmm[15]);

		return Float4(t[0], t[1], t[2], t[3]);
	}

    
	forceinline Float4x4& vectorcall Float4x4::Multiply(Float4x4 other)
	{
		float t[16];

		t[ 0] = (m_Xmm[ 0] * other.m_Xmm[0]) + (m_Xmm[ 1] * other.m_Xmm[4]) + (m_Xmm[ 2] * other.m_Xmm[ 8]) + (m_Xmm[ 3] * other.m_Xmm[12]);
		t[ 1] = (m_Xmm[ 0] * other.m_Xmm[1]) + (m_Xmm[ 1] * other.m_Xmm[5]) + (m_Xmm[ 2] * other.m_Xmm[ 9]) + (m_Xmm[ 3] * other.m_Xmm[13]);
		t[ 2] = (m_Xmm[ 0] * other.m_Xmm[2]) + (m_Xmm[ 1] * other.m_Xmm[6]) + (m_Xmm[ 2] * other.m_Xmm[10]) + (m_Xmm[ 3] * other.m_Xmm[14]);
		t[ 3] = (m_Xmm[ 0] * other.m_Xmm[3]) + (m_Xmm[ 1] * other.m_Xmm[7]) + (m_Xmm[ 2] * other.m_Xmm[11]) + (m_Xmm[ 3] * other.m_Xmm[15]);

		t[ 4] = (m_Xmm[ 4] * other.m_Xmm[0]) + (m_Xmm[ 5] * other.m_Xmm[4]) + (m_Xmm[ 6] * other.m_Xmm[ 8]) + (m_Xmm[ 7] * other.m_Xmm[12]);
		t[ 5] = (m_Xmm[ 4] * other.m_Xmm[1]) + (m_Xmm[ 5] * other.m_Xmm[5]) + (m_Xmm[ 6] * other.m_Xmm[ 9]) + (m_Xmm[ 7] * other.m_Xmm[13]);
		t[ 6] = (m_Xmm[ 4] * other.m_Xmm[2]) + (m_Xmm[ 5] * other.m_Xmm[6]) + (m_Xmm[ 6] * other.m_Xmm[10]) + (m_Xmm[ 7] * other.m_Xmm[14]);
		t[ 7] = (m_Xmm[ 4] * other.m_Xmm[3]) + (m_Xmm[ 5] * other.m_Xmm[7]) + (m_Xmm[ 6] * other.m_Xmm[11]) + (m_Xmm[ 7] * other.m_Xmm[15]);

		t[ 8] = (m_Xmm[ 8] * other.m_Xmm[0]) + (m_Xmm[ 9] * other.m_Xmm[4]) + (m_Xmm[10] * other.m_Xmm[ 8]) + (m_Xmm[11] * other.m_Xmm[12]);
		t[ 9] = (m_Xmm[ 8] * other.m_Xmm[1]) + (m_Xmm[ 9] * other.m_Xmm[5]) + (m_Xmm[10] * other.m_Xmm[ 9]) + (m_Xmm[11] * other.m_Xmm[13]);
		t[10] = (m_Xmm[ 8] * other.m_Xmm[2]) + (m_Xmm[ 9] * other.m_Xmm[6]) + (m_Xmm[10] * other.m_Xmm[10]) + (m_Xmm[11] * other.m_Xmm[14]);
		t[11] = (m_Xmm[ 8] * other.m_Xmm[3]) + (m_Xmm[ 9] * other.m_Xmm[7]) + (m_Xmm[10] * other.m_Xmm[11]) + (m_Xmm[11] * other.m_Xmm[15]);

		t[12] = (m_Xmm[12] * other.m_Xmm[0]) + (m_Xmm[13] * other.m_Xmm[4]) + (m_Xmm[14] * other.m_Xmm[ 8]) + (m_Xmm[15] * other.m_Xmm[12]);
		t[13] = (m_Xmm[12] * other.m_Xmm[1]) + (m_Xmm[13] * other.m_Xmm[5]) + (m_Xmm[14] * other.m_Xmm[ 9]) + (m_Xmm[15] * other.m_Xmm[13]);
		t[14] = (m_Xmm[12] * other.m_Xmm[2]) + (m_Xmm[13] * other.m_Xmm[6]) + (m_Xmm[14] * other.m_Xmm[10]) + (m_Xmm[15] * other.m_Xmm[14]);
		t[15] = (m_Xmm[12] * other.m_Xmm[3]) + (m_Xmm[13] * other.m_Xmm[7]) + (m_Xmm[14] * other.m_Xmm[11]) + (m_Xmm[15] * other.m_Xmm[15]);

		memcpy(m_Xmm, t, sizeof(m_Xmm));
		return *this;
	}
    

	forceinline Float4x4& vectorcall Float4x4::Multiply(float scalar)
	{
		for (int i = 0; i < 16; i++)
			m_Xmm[i] *= scalar;
		return *this;
	}
    

	forceinline Float4x4& vectorcall Float4x4::Add(Float4x4 other)
	{
		for (int i = 0; i < 16; i++)
			m_Xmm[i] += other.m_Xmm[i];
		return *this;
	}

    
	forceinline Float4x4& vectorcall Float4x4::Add(float scalar)
	{
		for (int i = 0; i < 16; i++)
			m_Xmm[i] += scalar;
		return *this;
	}
    

	forceinline Float4x4& vectorcall Float4x4::Subtract(Float4x4 other)
	{
		for (int i = 0; i < 16; i++)
			m_Xmm[i] -= other.m_Xmm[i];
		return *this;
	}

    
	forceinline Float4x4& vectorcall Float4x4::Subtract(float scalar)
	{
		for (int i = 0; i < 16; i++)
			m_Xmm[i] -= scalar;
		return *this;
	}

    
	forceinline Float4x4& vectorcall Float4x4::Divide(float scalar)
	{
		for (int i = 0; i < 16; i++)
			m_Xmm[i] /= scalar;
		return *this;
	}
    

	forceinline bool vectorcall Float4x4::Equals(Float4x4 other) const
	{
		for (int i = 0; i < 16; i++)
		{
			if (m_Xmm[i] != other.m_Xmm[i])
				return false;
		}
		return true;
	}

    
	forceinline void vectorcall Float4x4::SetRow(uint32 r, float x, float y, float z, float w)
	{
		assert(r < 4);
		m_Xmm[(r * 4) + 0] = x;
		m_Xmm[(r * 4) + 1] = y;
		m_Xmm[(r * 4) + 2] = z;
		m_Xmm[(r * 4) + 3] = w;
	}

    
	forceinline Float4 vectorcall Float4x4::GetRow(uint32 r) const
	{
		assert(r < 4);
		return Float4(m_Xmm[(r * 4) + 0], m_Xmm[(r * 4) + 1], m_Xmm[(r * 4) + 2], m_Xmm[(r * 4) + 3]);
	}
    

	inline Float4x4& vectorcall Float4x4::Transpose()
	{
		float temp[16];

		temp[ 0] = m_Xmm[ 0];
		temp[ 1] = m_Xmm[ 4];
		temp[ 2] = m_Xmm[ 8];
		temp[ 3] = m_Xmm[12];

		temp[ 4] = m_Xmm[ 1];
		temp[ 5] = m_Xmm[ 5];
		temp[ 6] = m_Xmm[ 9];
		temp[ 7] = m_Xmm[13];

		temp[ 8] = m_Xmm[ 2];
		temp[ 9] = m_Xmm[ 6];
		temp[10] = m_Xmm[10];
		temp[11] = m_Xmm[14];

		temp[12] = m_Xmm[ 3];
		temp[13] = m_Xmm[ 7];
		temp[14] = m_Xmm[11];
		temp[15] = m_Xmm[15];

		memcpy(m_Xmm, temp, sizeof(m_Xmm));
		return *this;
	}

    
	inline Float4x4& vectorcall Float4x4::Invert()
	{
		float det = Determinant();

		if (det == 0.0f)
			*this = Float4x4(0.0f);
		else
			*this = (Adjugate() / det);
		return *this;
	}

    
	inline Float4x4 vectorcall Float4x4::Adjugate() const
	{
		Float4x4 adjugate;
		//d11
		adjugate.m_Xmm[ 0] =  (m_Xmm[ 5] * ((m_Xmm[10] * m_Xmm[15]) - (m_Xmm[11] * m_Xmm[14])));
		adjugate.m_Xmm[ 0] -= (m_Xmm[ 6] * ((m_Xmm[ 9] * m_Xmm[15]) - (m_Xmm[11] * m_Xmm[13])));
		adjugate.m_Xmm[ 0] += (m_Xmm[ 7] * ((m_Xmm[ 9] * m_Xmm[14]) - (m_Xmm[10] * m_Xmm[13])));
		//d12
		adjugate.m_Xmm[ 4] =  (m_Xmm[ 4] * ((m_Xmm[10] * m_Xmm[15]) - (m_Xmm[11] * m_Xmm[14])));
		adjugate.m_Xmm[ 4] -= (m_Xmm[ 6] * ((m_Xmm[ 8] * m_Xmm[15]) - (m_Xmm[11] * m_Xmm[12])));
		adjugate.m_Xmm[ 4] += (m_Xmm[ 7] * ((m_Xmm[ 8] * m_Xmm[14]) - (m_Xmm[10] * m_Xmm[12])));
		adjugate.m_Xmm[ 4] *= -1.0f;
		//d13
		adjugate.m_Xmm[ 8] =  (m_Xmm[ 4] * ((m_Xmm[ 9] * m_Xmm[15]) - (m_Xmm[11] * m_Xmm[13])));
		adjugate.m_Xmm[ 8] -= (m_Xmm[ 5] * ((m_Xmm[ 8] * m_Xmm[15]) - (m_Xmm[11] * m_Xmm[12])));
		adjugate.m_Xmm[ 8] += (m_Xmm[ 7] * ((m_Xmm[ 8] * m_Xmm[13]) - (m_Xmm[ 9] * m_Xmm[12])));
		//d14
		adjugate.m_Xmm[12] =  (m_Xmm[ 4] * ((m_Xmm[ 9] * m_Xmm[14]) - (m_Xmm[10] * m_Xmm[13])));
		adjugate.m_Xmm[12] -= (m_Xmm[ 5] * ((m_Xmm[ 8] * m_Xmm[14]) - (m_Xmm[10] * m_Xmm[12])));
		adjugate.m_Xmm[12] += (m_Xmm[ 6] * ((m_Xmm[ 8] * m_Xmm[13]) - (m_Xmm[ 9] * m_Xmm[12])));
		adjugate.m_Xmm[12] *= -1.0f;

		//d21
		adjugate.m_Xmm[ 1] =  (m_Xmm[ 1] * ((m_Xmm[10] * m_Xmm[15]) - (m_Xmm[11] * m_Xmm[14])));
		adjugate.m_Xmm[ 1] -= (m_Xmm[ 2] * ((m_Xmm[ 9] * m_Xmm[15]) - (m_Xmm[11] * m_Xmm[13])));
		adjugate.m_Xmm[ 1] += (m_Xmm[ 3] * ((m_Xmm[ 9] * m_Xmm[14]) - (m_Xmm[10] * m_Xmm[13])));
		adjugate.m_Xmm[ 1] *= -1.0f;
		//d22
		adjugate.m_Xmm[ 5] =  (m_Xmm[ 0] * ((m_Xmm[10] * m_Xmm[15]) - (m_Xmm[11] * m_Xmm[14])));
		adjugate.m_Xmm[ 5] -= (m_Xmm[ 2] * ((m_Xmm[ 8] * m_Xmm[15]) - (m_Xmm[11] * m_Xmm[12])));
		adjugate.m_Xmm[ 5] += (m_Xmm[ 3] * ((m_Xmm[ 8] * m_Xmm[14]) - (m_Xmm[10] * m_Xmm[12])));
		//d23
		adjugate.m_Xmm[ 9] =  (m_Xmm[ 0] * ((m_Xmm[ 9] * m_Xmm[15]) - (m_Xmm[11] * m_Xmm[13])));
		adjugate.m_Xmm[ 9] -= (m_Xmm[ 1] * ((m_Xmm[ 8] * m_Xmm[15]) - (m_Xmm[11] * m_Xmm[12])));
		adjugate.m_Xmm[ 9] += (m_Xmm[ 3] * ((m_Xmm[ 8] * m_Xmm[13]) - (m_Xmm[ 9] * m_Xmm[12])));
		adjugate.m_Xmm[ 9] *= -1.0f;
		//d24
		adjugate.m_Xmm[13] =  (m_Xmm[ 0] * ((m_Xmm[ 9] * m_Xmm[14]) - (m_Xmm[10] * m_Xmm[13])));
		adjugate.m_Xmm[13] -= (m_Xmm[ 1] * ((m_Xmm[ 8] * m_Xmm[14]) - (m_Xmm[10] * m_Xmm[12])));
		adjugate.m_Xmm[13] += (m_Xmm[ 2] * ((m_Xmm[ 8] * m_Xmm[13]) - (m_Xmm[ 9] * m_Xmm[12])));

		//d31
		adjugate.m_Xmm[ 2] =  (m_Xmm[ 1] * ((m_Xmm[ 6] * m_Xmm[15]) - (m_Xmm[ 7] * m_Xmm[14])));
		adjugate.m_Xmm[ 2] -= (m_Xmm[ 2] * ((m_Xmm[ 5] * m_Xmm[15]) - (m_Xmm[ 7] * m_Xmm[13])));
		adjugate.m_Xmm[ 2] += (m_Xmm[ 3] * ((m_Xmm[ 5] * m_Xmm[14]) - (m_Xmm[ 6] * m_Xmm[13])));
		//d32
		adjugate.m_Xmm[ 6] =  (m_Xmm[ 0] * ((m_Xmm[ 6] * m_Xmm[15]) - (m_Xmm[ 7] * m_Xmm[14])));
		adjugate.m_Xmm[ 6] -= (m_Xmm[ 2] * ((m_Xmm[ 4] * m_Xmm[15]) - (m_Xmm[ 7] * m_Xmm[12])));
		adjugate.m_Xmm[ 6] += (m_Xmm[ 3] * ((m_Xmm[ 4] * m_Xmm[14]) - (m_Xmm[ 6] * m_Xmm[12])));
		adjugate.m_Xmm[ 6] *= -1.0f;
		//d33
		adjugate.m_Xmm[10] =  (m_Xmm[ 0] * ((m_Xmm[ 5] * m_Xmm[15]) - (m_Xmm[ 7] * m_Xmm[13])));
		adjugate.m_Xmm[10] -= (m_Xmm[ 1] * ((m_Xmm[ 4] * m_Xmm[15]) - (m_Xmm[ 7] * m_Xmm[12])));
		adjugate.m_Xmm[10] += (m_Xmm[ 3] * ((m_Xmm[ 4] * m_Xmm[13]) - (m_Xmm[ 5] * m_Xmm[12])));
		//d34
		adjugate.m_Xmm[14] =  (m_Xmm[ 0] * ((m_Xmm[ 5] * m_Xmm[14]) - (m_Xmm[ 6] * m_Xmm[13])));
		adjugate.m_Xmm[14] -= (m_Xmm[ 1] * ((m_Xmm[ 4] * m_Xmm[14]) - (m_Xmm[ 6] * m_Xmm[12])));
		adjugate.m_Xmm[14] += (m_Xmm[ 2] * ((m_Xmm[ 4] * m_Xmm[13]) - (m_Xmm[ 5] * m_Xmm[12])));
		adjugate.m_Xmm[14] *= -1.0f;

		//d41
		adjugate.m_Xmm[ 3] =  (m_Xmm[ 1] * ((m_Xmm[ 6] * m_Xmm[11]) - (m_Xmm[ 7] * m_Xmm[10])));
		adjugate.m_Xmm[ 3] -= (m_Xmm[ 2] * ((m_Xmm[ 5] * m_Xmm[11]) - (m_Xmm[ 7] * m_Xmm[ 9])));
		adjugate.m_Xmm[ 3] += (m_Xmm[ 3] * ((m_Xmm[ 5] * m_Xmm[10]) - (m_Xmm[ 6] * m_Xmm[ 9])));
		adjugate.m_Xmm[ 3] *= -1.0f;
		//d42
		adjugate.m_Xmm[ 7] =  (m_Xmm[ 0] * ((m_Xmm[ 6] * m_Xmm[11]) - (m_Xmm[ 7] * m_Xmm[10])));
		adjugate.m_Xmm[ 7] -= (m_Xmm[ 2] * ((m_Xmm[ 4] * m_Xmm[11]) - (m_Xmm[ 7] * m_Xmm[ 8])));
		adjugate.m_Xmm[ 7] += (m_Xmm[ 3] * ((m_Xmm[ 4] * m_Xmm[10]) - (m_Xmm[ 6] * m_Xmm[ 8])));
		//d43
		adjugate.m_Xmm[11] =  (m_Xmm[ 0] * ((m_Xmm[ 5] * m_Xmm[11]) - (m_Xmm[ 7] * m_Xmm[ 9])));
		adjugate.m_Xmm[11] -= (m_Xmm[ 1] * ((m_Xmm[ 4] * m_Xmm[11]) - (m_Xmm[ 7] * m_Xmm[ 8])));
		adjugate.m_Xmm[11] += (m_Xmm[ 3] * ((m_Xmm[ 4] * m_Xmm[ 9]) - (m_Xmm[ 5] * m_Xmm[ 8])));
		adjugate.m_Xmm[11] *= -1.0f;
		//d44
		adjugate.m_Xmm[15] =  (m_Xmm[ 0] * ((m_Xmm[ 5] * m_Xmm[10]) - (m_Xmm[ 6] * m_Xmm[ 9])));
		adjugate.m_Xmm[15] -= (m_Xmm[ 1] * ((m_Xmm[ 4] * m_Xmm[10]) - (m_Xmm[ 6] * m_Xmm[ 8])));
		adjugate.m_Xmm[15] += (m_Xmm[ 2] * ((m_Xmm[ 4] * m_Xmm[ 9]) - (m_Xmm[ 5] * m_Xmm[ 8])));
		return adjugate;
	}

    
	inline float vectorcall Float4x4::Determinant() const
	{
		float det = 0.0f;

		//d11
		det =	 m_Xmm[0] * (
				(m_Xmm[5] * ((m_Xmm[10] * m_Xmm[15]) - (m_Xmm[11] * m_Xmm[14]))) -
				(m_Xmm[6] * ((m_Xmm[ 9] * m_Xmm[15]) - (m_Xmm[11] * m_Xmm[13]))) +
				(m_Xmm[7] * ((m_Xmm[ 9] * m_Xmm[14]) - (m_Xmm[10] * m_Xmm[13]))));
		//d12
		det -=	 m_Xmm[1] * (
				(m_Xmm[4] * ((m_Xmm[10] * m_Xmm[15]) - (m_Xmm[11] * m_Xmm[14]))) -
				(m_Xmm[6] * ((m_Xmm[ 8] * m_Xmm[15]) - (m_Xmm[11] * m_Xmm[12]))) +
				(m_Xmm[7] * ((m_Xmm[ 8] * m_Xmm[14]) - (m_Xmm[10] * m_Xmm[12]))));
		//d13
		det +=	 m_Xmm[2] * (
				(m_Xmm[4] * ((m_Xmm[ 9] * m_Xmm[15]) - (m_Xmm[11] * m_Xmm[13]))) -
				(m_Xmm[5] * ((m_Xmm[ 8] * m_Xmm[15]) - (m_Xmm[11] * m_Xmm[12]))) +
				(m_Xmm[7] * ((m_Xmm[ 8] * m_Xmm[13]) - (m_Xmm[ 9] * m_Xmm[12]))));
		//d14
		det -=	 m_Xmm[3] * (
				(m_Xmm[4] * ((m_Xmm[ 9] * m_Xmm[14]) - (m_Xmm[10] * m_Xmm[13]))) -
				(m_Xmm[5] * ((m_Xmm[ 8] * m_Xmm[14]) - (m_Xmm[10] * m_Xmm[12]))) +
				(m_Xmm[6] * ((m_Xmm[ 8] * m_Xmm[13]) - (m_Xmm[ 9] * m_Xmm[12]))));

		return det;
	}
    

	forceinline Float4x4& vectorcall Float4x4::operator=(Float4x4 other)
	{
		memcpy(m_Xmm, other.m_Xmm, sizeof(m_Xmm));
		return *this;
	}
    

	forceinline Mat4f vectorcall Store(Float4x4 f)
	{
		Mat4f result;
		memcpy(result.m, f.m_Xmm, sizeof(f.m_Xmm));
		return result;
	}

    
	forceinline Float4x4 vectorcall Load(Mat4f storage)
	{
		Float4x4 result;
		memcpy(result.m_Xmm, storage.m, sizeof(result.m_Xmm));
		return result;
	}
#endif

    
	forceinline float vectorcall Float4x4::GetElement(uint32 r, uint32 c) const
	{
		assert(r < 4);
		assert(c < 4);
		return GetRow(r)[c];
	}

    
	inline Float4x4 vectorcall Float4x4::GetTranspose() const
	{
		Float4x4 transpose = *this;
		transpose.Transpose();
		return transpose;
	}

    
	inline Float4x4 vectorcall Float4x4::GetInverse() const
	{
		Float4x4 inverse = *this;
		inverse.Invert();
		return inverse;
	}

    
	inline Float4x4 vectorcall Float4x4::Cofactor() const
	{
		return Adjugate().Transpose();
	}

    
	forceinline bool vectorcall Float4x4::operator==(Float4x4 other) const
	{
		return Equals(other);
	}

    
	forceinline bool vectorcall Float4x4::operator!=(Float4x4 other) const
	{
		return !Equals(other);
	}

    
	forceinline float Float4x4::operator()(uint32 r, uint32 c) const
	{
		return GetElement(r, c);
	}
    

	forceinline Float4 Float4x4::operator()(uint32 r) const
	{
		return GetRow(r);
	}
    

	forceinline Float4x4& vectorcall Float4x4::operator+=(Float4x4 right)
	{
		return *this = *this + right;
	}
    
    
	forceinline Float4x4& vectorcall Float4x4::operator-=(Float4x4 right)
	{
		return *this = *this - right;
	}

    
	forceinline Float4x4& vectorcall Float4x4::operator*=(Float4x4 right)
	{
		return *this = *this * right;
	}

    
	forceinline Float4x4& vectorcall Float4x4::operator+=(float right)
	{
		return *this = *this + right;
	}

    
	forceinline Float4x4& vectorcall Float4x4::operator-=(float right)
	{
		return *this = *this - right;
	}

    
	forceinline Float4x4& vectorcall Float4x4::operator*=(float right)
	{
		return *this = *this * right;
	}

    
	forceinline Float4x4& vectorcall Float4x4::operator/=(float right)
	{
		return *this = *this / right;
	}

    
	forceinline std::string vectorcall ToString(Float4x4 f)
	{
		return
			'[' + std::to_string(f(0, 0)) + ", " + std::to_string(f(0, 1)) + ", " + std::to_string(f(0, 2)) + ", " + std::to_string(f(0, 3)) + "]\n" +
			'[' + std::to_string(f(1, 0)) + ", " + std::to_string(f(1, 1)) + ", " + std::to_string(f(1, 2)) + ", " + std::to_string(f(1, 3)) + "]\n" +
			'[' + std::to_string(f(2, 0)) + ", " + std::to_string(f(2, 1)) + ", " + std::to_string(f(2, 2)) + ", " + std::to_string(f(2, 3)) + "]\n" +
			'[' + std::to_string(f(3, 0)) + ", " + std::to_string(f(3, 1)) + ", " + std::to_string(f(3, 2)) + ", " + std::to_string(f(3, 3)) + "]\n";
	}

    
	forceinline Float4 vectorcall operator*(Float4 left, Float4x4 right)
	{
		return right.Multiply(left);
	}

    
	forceinline Float4& vectorcall operator*=(Float4& left, Float4x4 right)
	{
		left = right.Multiply(left);
		return left;
	}

    
	forceinline Float4x4 vectorcall operator+(Float4x4 left, Float4x4 right)
	{
		return left.Add(right);
	}

    
	forceinline Float4x4 vectorcall operator-(Float4x4 left, Float4x4 right)
	{
		return left.Subtract(right);
	}

    
	forceinline Float4x4 vectorcall operator*(Float4x4 left, Float4x4 right)
	{
		return left.Multiply(right);
	}

    
	forceinline Float4x4 vectorcall operator+(Float4x4 left, float right)
	{
		return left.Add(right);
	}
    

	forceinline Float4x4 vectorcall operator-(Float4x4 left, float right)
	{
		return  left.Subtract(right);
	}

    
	forceinline Float4x4 vectorcall operator*(Float4x4 left, float right)
	{
		return left.Multiply(right);
	}
    

	forceinline Float4x4 vectorcall operator/(Float4x4 left, float right)
	{
		return left.Divide(right);
	}
    

	forceinline Float4x4 vectorcall operator*(float left, Float4x4 right)
	{
		return right.Multiply(left);
	}
    

	forceinline Float4x4 vectorcall Float4x4::Identity()
	{
		return Float4x4(1.0f);
	}
    

	forceinline Float4x4 vectorcall Float4x4::Perspective(float fovRad, float aspectWiHe, float farZ, float nearZ)
	{
		if (fovRad < (PI_F / 180.0f) || fovRad > (PI_F - (PI_F / 180.0f)))
			return Float4x4(0.0f);

		Float4x4 m;

		float yScale = 1.0f / tan(fovRad * 0.5f);
		float xScale = yScale / aspectWiHe;

		m.SetRow(0, xScale,		0.0f,		0.0f,									0.0f);
		m.SetRow(1, 0.0f,		yScale,		0.0f,									0.0f);
		m.SetRow(2, 0.0f,		0.0f,		farZ / (farZ - nearZ),					1.0f);
		m.SetRow(3, 0.0f,		0.0f,		(-nearZ * farZ) / (farZ - nearZ),		0.0f);

		return m;
	}

    
	forceinline Float4x4 vectorcall Float4x4::Orthographic(float aspectWiHe, float farZ, float nearZ)
	{
		Float4x4 m(1.0f);

		m.SetRow(0, 2.0f / aspectWiHe,		0.0f,		0.0f,						0.0f);
		m.SetRow(1, 0.0f,					2.0f,		0.0f,						0.0f);
		m.SetRow(2, 0.0f,					0.0f,		1.0f / (farZ - nearZ),		0.0f);
		m.SetRow(3, 0.0f,					0.0f,		-nearZ / (farZ - nearZ),	1.0f);

		return m;
	}

    
	forceinline Float4x4 vectorcall Float4x4::Orthographic(float width, float height, float farZ, float nearZ)
	{
		Float4x4 m(1.0f);

		m.SetRow(0, 2.0f / width,	0.0f,				0.0f,						0.0f);
		m.SetRow(1, 0.0f,			2.0f / height,		0.0f,						0.0f);
		m.SetRow(2, 0.0f,			0.0f,				1.0f / (farZ - nearZ),		0.0f);
		m.SetRow(3, 0.0f,			0.0f,				-nearZ / (farZ - nearZ),	1.0f);

		return m;
	}

    
	forceinline Float4x4 vectorcall Float4x4::Orthographic(float leftX, float rightX, float topY, float bottomY, float farZ, float nearZ)
	{
		Float4x4 m(1.0f);

		m.SetRow(0, 2.0f / (rightX - leftX),				0.0f,									0.0f,						0.0f);
		m.SetRow(1, 0.0f,									2.0f / (topY - bottomY),				0.0f,						0.0f);
		m.SetRow(2, 0.0f,									0.0f,									1.0f / (farZ - nearZ),		0.0f);
		m.SetRow(3, (leftX + rightX) / (leftX - rightX),	(topY + bottomY) / (bottomY - topY),	nearZ / (nearZ - farZ),		1.0f);

		return m;
	}

    
	forceinline Float4x4 vectorcall Float4x4::LookAt(Vec3f up, Vec3f at, Vec3f position)
	{
		Float4x4 m(1.0f);

		Float3 pos = Load(position);
		Float3 fz = (Load(at) - pos).Normalize();
		Float3 fx = (Load(up).Normalize().Cross(fz)).Normalize();
		Float3 fy = fz.Cross(fx);

		Vec3f x = Store(fx);
		Vec3f y = Store(fy);
		Vec3f z = Store(fz);

		m.SetRow(0, x.x,			y.x,			z.x,			0.0f);
		m.SetRow(1, x.y,			y.y,			z.y,			0.0f);
		m.SetRow(2, x.z,			y.z,			z.z,			0.0f);
		m.SetRow(3, -fx.Dot(pos),	-fy.Dot(pos),	-fz.Dot(pos),	1.0f);

		return m;
	}

    
	forceinline Float4x4 vectorcall Float4x4::Translation(Vec3f translation)
	{
		Float4x4 m(1.0f);
		m.SetRow(3, translation.x, translation.y, translation.z, 1.0f);
		return m;
	}

    
	forceinline Float4x4 vectorcall Float4x4::RotationRollPitchYaw(float angleRadZ, float angleRadX, float angleRadY)
	{
		Float4x4 m;

		float sinX = sinf(angleRadX);
		float sinY = sinf(angleRadY);
		float sinZ = sinf(angleRadZ);

		float cosX = cosf(angleRadX);
		float cosY = cosf(angleRadY);
		float cosZ = cosf(angleRadZ);

		m.SetRow(0, (cosY * cosZ) + (sinX * sinY * sinZ),	cosX * sinZ,		(sinX * sinY * sinZ) - (cosZ * sinY),	0.0f);
		m.SetRow(1, (cosZ * sinX * sinY) - (cosY * sinZ),	cosX * cosZ,		(cosY * cosZ * sinX) + (sinY * sinZ),	0.0f);
		m.SetRow(2, cosX * sinY,							-sinX,				cosX * cosY,							0.0f);

		return m;
	}

    
	forceinline Float4x4 vectorcall Float4x4::Rotation(Vec3f axis, float angleRad)
	{
		Float4x4 m(1.0f);
		Float3 norm = Load(axis).Normalize();
		Vec3f ax = Store(norm);

		float cosine = cosf(angleRad);
		float sine = sinf(angleRad);
		float c = 1 - cosine;

		m.SetRow(0, (ax.x * ax.x * c) + cosine,			(ax.y * ax.x * c) + (sine * ax.z),	(ax.z * ax.x * c) - (sine * ax.y),	0.0f);
		m.SetRow(1, (ax.x * ax.y * c) - (sine * ax.z),	(ax.y * ax.y * c) + cosine,			(ax.z * ax.y * c) + (sine * ax.x),	0.0f);
		m.SetRow(2, (ax.x * ax.z * c) + (sine * ax.y),	(ax.y * ax.z * c) - (sine * ax.x),	(ax.z * ax.z * c) + cosine,			0.0f);

		return m;
	}
    

	forceinline Float4x4 vectorcall Float4x4::RotationX(float angleRad)
	{
		Float4x4 m(1.0f);

		float cosine = cosf(angleRad);
		float sine = sinf(angleRad);

		m.SetRow(1, 0.0f, cosine,	sine,	0.0f);
		m.SetRow(2, 0.0f, -sine,	cosine,	0.0f);

		return m;
	}
    

	forceinline Float4x4 vectorcall Float4x4::RotationY(float angleRad)
	{
		Float4x4 m(1.0f);

		float cosine = cosf(angleRad);
		float sine = sinf(angleRad);

		m.SetRow(0, cosine,	0.0f,	-sine,	0.0f);
		m.SetRow(2, sine,	0.0f,	cosine,	0.0f);

		return m;
	}
    

	forceinline Float4x4 vectorcall Float4x4::RotationZ(float angleRad)
	{
		Float4x4 m(1.0f);

		float cosine = cosf(angleRad);
		float sine = sinf(angleRad);

		m.SetRow(0, cosine,	sine,	0.0f, 0.0f);
		m.SetRow(1, -sine,	cosine,	0.0f, 0.0f);

		return m;
	}
    

	forceinline Float4x4 vectorcall Float4x4::Scale(Vec3f scale)
	{
		Float4x4 m(1.0f);
		m.SetRow(0, scale.x,	0.0f,		0.0f,		0.0f);
		m.SetRow(1, 0.0f,		scale.y,	0.0f,		0.0f);
		m.SetRow(2, 0.0f,		0.0f,		scale.z,	0.0f);
		return m;
	}
    

	forceinline Float4x4 vectorcall Float4x4::Scale(float scale)
	{
		Float4x4 m(scale);
		m.SetRow(3, 0.0f, 0.0f, 0.0f, 1.0f);
		return m;
	}
}
