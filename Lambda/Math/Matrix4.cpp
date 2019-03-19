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
#include "../../Include/Math/Matrix4.h"

namespace Math
{
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	Matrix4::Matrix4()
	{
		memset(m, 0, sizeof(float) * 16);
	}


	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	Matrix4::Matrix4(float diagonal)
	{
		memset(m, 0, sizeof(float) * 16);

		rows[0].v[0] = diagonal;
		rows[1].v[1] = diagonal;
		rows[2].v[2] = diagonal;
		rows[3].v[3] = diagonal;
	}


	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	Matrix4::Matrix4(const Vector4& r1, const Vector4& r2, const Vector4& r3, const Vector4& r4)
	{
		rows[0] = r1;
		rows[1] = r2;
		rows[2] = r3;
		rows[3] = r4;
	}


	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	Matrix4::Matrix4(const Matrix4& other)
	{
		*this = other;
	}


	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	Vector4 Matrix4::Multiply(const Vector4& vector) const
	{
#if defined(SSE_INTRIN)
		__m128 r1 = _mm_mul_ps(_mm_shuffle_ps(vector.sse128, vector.sse128, 0), sse128[0]);
		r1 = _mm_add_ps(r1, _mm_mul_ps(_mm_shuffle_ps(vector.sse128, vector.sse128, 0x55), sse128[1]));
		r1 = _mm_add_ps(r1, _mm_mul_ps(_mm_shuffle_ps(vector.sse128, vector.sse128, 0xaa), sse128[2]));
		r1 = _mm_add_ps(r1, _mm_mul_ps(_mm_shuffle_ps(vector.sse128, vector.sse128, 0xff), sse128[3]));
		return r1;
#else
		float t[4];

		t[0] = (vector.v[0] * rows[0].v[0]) + (vector.v[1] * rows[1].v[0]) + (vector.v[2] * rows[2].v[0]) + (vector.v[3] * rows[3].v[0]);
		t[1] = (vector.v[0] * rows[0].v[1]) + (vector.v[1] * rows[1].v[1]) + (vector.v[2] * rows[2].v[1]) + (vector.v[3] * rows[3].v[1]);
		t[2] = (vector.v[0] * rows[0].v[2]) + (vector.v[1] * rows[1].v[2]) + (vector.v[2] * rows[2].v[2]) + (vector.v[3] * rows[3].v[2]);
		t[3] = (vector.v[0] * rows[0].v[3]) + (vector.v[1] * rows[1].v[3]) + (vector.v[2] * rows[2].v[3]) + (vector.v[3] * rows[3].v[3]);

		return Vector4(t[0], t[1], t[2], t[3]);
#endif
	}


	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	Matrix4& Matrix4::Multiply(const Matrix4& other)
	{
#if defined(SSE_INTRIN)
		__m128 r1 = _mm_mul_ps(_mm_shuffle_ps(sse128[0], sse128[0], 0), other.sse128[0]);
		r1 = _mm_add_ps(r1, _mm_mul_ps(_mm_shuffle_ps(sse128[0], sse128[0], 0x55), other.sse128[1]));
		r1 = _mm_add_ps(r1, _mm_mul_ps(_mm_shuffle_ps(sse128[0], sse128[0], 0xaa), other.sse128[2]));
		r1 = _mm_add_ps(r1, _mm_mul_ps(_mm_shuffle_ps(sse128[0], sse128[0], 0xff), other.sse128[3]));

		__m128 r2 = _mm_mul_ps(_mm_shuffle_ps(sse128[1], sse128[1], 0), other.sse128[0]);
		r2 = _mm_add_ps(r2, _mm_mul_ps(_mm_shuffle_ps(sse128[1], sse128[1], 0x55), other.sse128[1]));
		r2 = _mm_add_ps(r2, _mm_mul_ps(_mm_shuffle_ps(sse128[1], sse128[1], 0xaa), other.sse128[2]));
		r2 = _mm_add_ps(r2, _mm_mul_ps(_mm_shuffle_ps(sse128[1], sse128[1], 0xff), other.sse128[3]));

		__m128 r3 = _mm_mul_ps(_mm_shuffle_ps(sse128[2], sse128[2], 0), other.sse128[0]);
		r3 = _mm_add_ps(r3, _mm_mul_ps(_mm_shuffle_ps(sse128[2], sse128[2], 0x55), other.sse128[1]));
		r3 = _mm_add_ps(r3, _mm_mul_ps(_mm_shuffle_ps(sse128[2], sse128[2], 0xaa), other.sse128[2]));
		r3 = _mm_add_ps(r3, _mm_mul_ps(_mm_shuffle_ps(sse128[2], sse128[2], 0xff), other.sse128[3]));

		__m128 r4 = _mm_mul_ps(_mm_shuffle_ps(sse128[3], sse128[3], 0), other.sse128[0]);
		r4 = _mm_add_ps(r4, _mm_mul_ps(_mm_shuffle_ps(sse128[3], sse128[3], 0x55), other.sse128[1]));
		r4 = _mm_add_ps(r4, _mm_mul_ps(_mm_shuffle_ps(sse128[3], sse128[3], 0xaa), other.sse128[2]));
		r4 = _mm_add_ps(r4, _mm_mul_ps(_mm_shuffle_ps(sse128[3], sse128[3], 0xff), other.sse128[3]));

		sse128[0] = r1;
		sse128[1] = r2;
		sse128[2] = r3;
		sse128[3] = r4;
#else
		float t[16];

		t[0] = (rows[0].v[0] * other.rows[0].v[0]) + (rows[0].v[1] * other.rows[1].v[0]) + (rows[0].v[2] * other.rows[2].v[0]) + (rows[0].v[3] * other.rows[3].v[0]);
		t[1] = (rows[0].v[0] * other.rows[0].v[1]) + (rows[0].v[1] * other.rows[1].v[1]) + (rows[0].v[2] * other.rows[2].v[1]) + (rows[0].v[3] * other.rows[3].v[1]);
		t[2] = (rows[0].v[0] * other.rows[0].v[2]) + (rows[0].v[1] * other.rows[1].v[2]) + (rows[0].v[2] * other.rows[2].v[2]) + (rows[0].v[3] * other.rows[3].v[2]);
		t[3] = (rows[0].v[0] * other.rows[0].v[3]) + (rows[0].v[1] * other.rows[1].v[3]) + (rows[0].v[2] * other.rows[2].v[3]) + (rows[0].v[3] * other.rows[3].v[3]);

		t[4] = (rows[1].v[0] * other.rows[0].v[0]) + (rows[1].v[1] * other.rows[1].v[0]) + (rows[1].v[2] * other.rows[2].v[0]) + (rows[1].v[3] * other.rows[3].v[0]);
		t[5] = (rows[1].v[0] * other.rows[0].v[1]) + (rows[1].v[1] * other.rows[1].v[1]) + (rows[1].v[2] * other.rows[2].v[1]) + (rows[1].v[3] * other.rows[3].v[1]);
		t[6] = (rows[1].v[0] * other.rows[0].v[2]) + (rows[1].v[1] * other.rows[1].v[2]) + (rows[1].v[2] * other.rows[2].v[2]) + (rows[1].v[3] * other.rows[3].v[2]);
		t[7] = (rows[1].v[0] * other.rows[0].v[3]) + (rows[1].v[1] * other.rows[1].v[3]) + (rows[1].v[2] * other.rows[2].v[3]) + (rows[1].v[3] * other.rows[3].v[3]);

		t[8] = (rows[2].v[0] * other.rows[0].v[0]) + (rows[2].v[1] * other.rows[1].v[0]) + (rows[2].v[2] * other.rows[2].v[0]) + (rows[2].v[3] * other.rows[3].v[0]);
		t[9] = (rows[2].v[0] * other.rows[0].v[1]) + (rows[2].v[1] * other.rows[1].v[1]) + (rows[2].v[2] * other.rows[2].v[1]) + (rows[2].v[3] * other.rows[3].v[1]);
		t[10] = (rows[2].v[0] * other.rows[0].v[2]) + (rows[2].v[1] * other.rows[1].v[2]) + (rows[2].v[2] * other.rows[2].v[2]) + (rows[2].v[3] * other.rows[3].v[2]);
		t[11] = (rows[2].v[0] * other.rows[0].v[3]) + (rows[2].v[1] * other.rows[1].v[3]) + (rows[2].v[2] * other.rows[2].v[3]) + (rows[2].v[3] * other.rows[3].v[3]);

		t[12] = (rows[3].v[0] * other.rows[0].v[0]) + (rows[3].v[1] * other.rows[1].v[0]) + (rows[3].v[2] * other.rows[2].v[0]) + (rows[3].v[3] * other.rows[3].v[0]);
		t[13] = (rows[3].v[0] * other.rows[0].v[1]) + (rows[3].v[1] * other.rows[1].v[1]) + (rows[3].v[2] * other.rows[2].v[1]) + (rows[3].v[3] * other.rows[3].v[1]);
		t[14] = (rows[3].v[0] * other.rows[0].v[2]) + (rows[3].v[1] * other.rows[1].v[2]) + (rows[3].v[2] * other.rows[2].v[2]) + (rows[3].v[3] * other.rows[3].v[2]);
		t[15] = (rows[3].v[0] * other.rows[0].v[3]) + (rows[3].v[1] * other.rows[1].v[3]) + (rows[3].v[2] * other.rows[2].v[3]) + (rows[3].v[3] * other.rows[3].v[3]);

		memcpy(m, t, sizeof(float) * 16);
#endif
		return *this;
	}


	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	Matrix4& Matrix4::Multiply(float scalar)
	{
#if defined(SSE_INTRIN)
		__m128 scalars = _mm_set_ps1(scalar);
		sse128[0] = _mm_mul_ps(sse128[0], scalars);
		sse128[1] = _mm_mul_ps(sse128[1], scalars);
		sse128[2] = _mm_mul_ps(sse128[2], scalars);
		sse128[3] = _mm_mul_ps(sse128[3], scalars);
#else
		for (int i = 0; i < 16; i++)
			m[i] *= scalar;
#endif
		return *this;
	}


	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	Matrix4& Matrix4::Add(const Matrix4& other)
	{
#if defined(SSE_INTRIN)
		sse128[0] = _mm_add_ps(sse128[0], other.sse128[0]);
		sse128[1] = _mm_add_ps(sse128[1], other.sse128[1]);
		sse128[2] = _mm_add_ps(sse128[2], other.sse128[2]);
		sse128[3] = _mm_add_ps(sse128[3], other.sse128[3]);
#else
		for (int i = 0; i < 16; i++)
			m[i] += other.m[i];
#endif
		return *this;
	}


	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	Matrix4& Matrix4::Add(float scalar)
	{
#if defined(SSE_INTRIN)
		__m128 scalars = _mm_set_ps1(scalar);
		sse128[0] = _mm_add_ps(sse128[0], scalars);
		sse128[1] = _mm_add_ps(sse128[1], scalars);
		sse128[2] = _mm_add_ps(sse128[2], scalars);
		sse128[3] = _mm_add_ps(sse128[3], scalars);
#else
		for (int i = 0; i < 16; i++)
			m[i] += scalar;
#endif
		return *this;
	}


	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	Matrix4& Matrix4::Subtract(const Matrix4& other)
	{
#if defined(SSE_INTRIN)
		sse128[0] = _mm_sub_ps(sse128[0], other.sse128[0]);
		sse128[1] = _mm_sub_ps(sse128[1], other.sse128[1]);
		sse128[2] = _mm_sub_ps(sse128[2], other.sse128[2]);
		sse128[3] = _mm_sub_ps(sse128[3], other.sse128[3]);
#else
		for (int i = 0; i < 16; i++)
			m[i] -= other.m[i];
#endif

		return *this;
	}


	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	Matrix4& Matrix4::Subtract(float scalar)
	{
#if defined(SSE_INTRIN)
		__m128 scalars = _mm_set_ps1(scalar);
		sse128[0] = _mm_sub_ps(sse128[0], scalars);
		sse128[1] = _mm_sub_ps(sse128[1], scalars);
		sse128[2] = _mm_sub_ps(sse128[2], scalars);
		sse128[3] = _mm_sub_ps(sse128[3], scalars);
#else
		for (int i = 0; i < 16; i++)
			m[i] -= scalar;
#endif
		return *this;
	}


	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	Matrix4& Matrix4::Divide(float scalar)
	{
#if defined(SSE_INTRIN)
		__m128 scalars = _mm_set_ps1(scalar);
		sse128[0] = _mm_div_ps(sse128[0], scalars);
		sse128[1] = _mm_div_ps(sse128[1], scalars);
		sse128[2] = _mm_div_ps(sse128[2], scalars);
		sse128[3] = _mm_div_ps(sse128[3], scalars);
#else
		for (int i = 0; i < 16; i++)
			m[i] /= scalar;
#endif
		return *this;
	}


	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	bool Matrix4::Equals(const Matrix4& other) const
	{
		for (int i = 0; i < 16; i++)
		{
			if (m[i] != other.m[i])
				return false;
		}

		return true;
	}


	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	float* Matrix4::GetArray()
	{
		return m;
	}


	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	const float* Matrix4::GetArray() const
	{
		return m;
	}


	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	float Matrix4::GetElement(unsigned char r, unsigned char c) const
	{
		assert(r < 4);
		assert(c < 4);

		return rows[r].v[c];
	}


	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	float& Matrix4::GetElement(unsigned char r, unsigned char c)
	{
		assert(r < 4);
		assert(c < 4);

		return rows[r].v[c];
	}


	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	Vector4 Matrix4::GetRow(unsigned char r) const
	{
		assert(r < 4);

		return rows[r];
	}


	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	Vector4& Matrix4::GetRow(unsigned char r)
	{
		assert(r < 4);

		return rows[r];
	}


	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	Matrix4& Matrix4::Transpose()
	{
#if defined(SSE_INTRIN)
		_MM_TRANSPOSE4_PS(sse128[0], sse128[1], sse128[2], sse128[3]);
#else
		float temp[16];

		temp[0] = rows[0].v[0];
		temp[1] = rows[1].v[0];
		temp[2] = rows[2].v[0];
		temp[3] = rows[3].v[0];

		temp[4] = rows[0].v[1];
		temp[5] = rows[1].v[1];
		temp[6] = rows[2].v[1];
		temp[7] = rows[3].v[1];

		temp[8] = rows[0].v[2];
		temp[9] = rows[1].v[2];
		temp[10] = rows[2].v[2];
		temp[11] = rows[3].v[2];

		temp[12] = rows[0].v[3];
		temp[13] = rows[1].v[3];
		temp[14] = rows[2].v[3];
		temp[15] = rows[3].v[3];

		memcpy(m, temp, sizeof(float) * 16);
#endif
		return *this;
	}


	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	Matrix4 Matrix4::TransposeOf() const
	{
		return Matrix4(*this).Transpose();
	}


	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	Matrix4& Matrix4::Invert()
	{
#if defined(SSE_INTRIN)
		__m128 detA = _mm_set_ps1((m[0] * m[5]) - (m[1] * m[4]));
		__m128 detB = _mm_set_ps1((m[2] * m[7]) - (m[3] * m[6]));
		__m128 detD = _mm_set_ps1((m[10] * m[15]) - (m[11] * m[14]));
		__m128 detC = _mm_set_ps1((m[8] * m[13]) - (m[9] * m[12]));

		__m128 a = _mm_movelh_ps(sse128[0], sse128[1]);
		__m128 b = _mm_movehl_ps(sse128[1], sse128[0]);
		__m128 c = _mm_movelh_ps(sse128[2], sse128[3]);
		__m128 d = _mm_movehl_ps(sse128[3], sse128[2]);
		
		__m128 ab = Matrix2AdjMul(a, b);
		__m128 dc = Matrix2AdjMul(d, c);
		
		__m128 w = _mm_sub_ps(_mm_mul_ps(detA, d), Matrix2Mul(c, ab));
		__m128 x = _mm_sub_ps(_mm_mul_ps(detD, a), Matrix2Mul(b, dc));
		
		__m128 detM = _mm_mul_ps(detA, detD);

		__m128 y = _mm_sub_ps(_mm_mul_ps(detB, c), Matrix2MulAdj(d, ab));
		__m128 z = _mm_sub_ps(_mm_mul_ps(detC, b), Matrix2MulAdj(a, dc));

		detM = _mm_add_ps(detM, _mm_mul_ps(detB, detC));

		__m128 trace = _mm_mul_ps(ab, Swizzle128(dc, 0, 2, 1, 3));
		detM = _mm_sub_ps(detM, HorizonatalSum(trace));
		detM = _mm_div_ps(_mm_setr_ps(1.0f, -1.0f, -1.0f, 1.0f), detM);
		
		x = _mm_mul_ps(x, detM);
		y = _mm_mul_ps(y, detM);
		z = _mm_mul_ps(z, detM);
		w = _mm_mul_ps(w, detM);
		
		sse128[0] = _mm_shuffle_ps(x, y, _MM_SHUFFLE(1, 3, 1, 3));
		sse128[1] = _mm_shuffle_ps(x, y, _MM_SHUFFLE(0, 2, 0, 2));
		sse128[2] = _mm_shuffle_ps(z, w, _MM_SHUFFLE(1, 3, 1, 3));
		sse128[3] = _mm_shuffle_ps(z, w, _MM_SHUFFLE(0, 2, 0, 2));
#else
		float det = Determinant();

		if (det == 0.0f)
			*this = Matrix4::Nan();
		else
			*this = (Adjugate() / det);
#endif
		return *this;
	}


	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	Matrix4 Matrix4::InverseOf() const
	{
		return Matrix4(*this).Invert();
	}


	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	Matrix4 Matrix4::Cofactor() const
	{
		return Adjugate().Transpose();
	}


	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	Matrix4 Matrix4::Adjugate() const
	{
		Matrix4 adj;

#if defined(SSE_INTRIN)
		__m128 detA = _mm_set_ps1((m[0] * m[5]) - (m[1] * m[4]));
		__m128 detB = _mm_set_ps1((m[2] * m[7]) - (m[3] * m[6]));
		__m128 detD = _mm_set_ps1((m[10] * m[15]) - (m[11] * m[14]));
		__m128 detC = _mm_set_ps1((m[8] * m[13]) - (m[9] * m[12]));

		__m128 a = _mm_movelh_ps(sse128[0], sse128[1]);
		__m128 b = _mm_movehl_ps(sse128[1], sse128[0]);
		__m128 c = _mm_movelh_ps(sse128[2], sse128[3]);
		__m128 d = _mm_movehl_ps(sse128[3], sse128[2]);

		__m128 ab = Matrix2AdjMul(a, b);
		__m128 dc = Matrix2AdjMul(d, c);
		__m128 w = _mm_sub_ps(_mm_mul_ps(detA, d), Matrix2Mul(c, ab));
		__m128 y = _mm_sub_ps(_mm_mul_ps(detB, c), Matrix2MulAdj(d, ab));
		
		__m128 x = _mm_sub_ps(_mm_mul_ps(detD, a), Matrix2Mul(b, dc));
		__m128 z = _mm_sub_ps(_mm_mul_ps(detC, b), Matrix2MulAdj(a, dc));

		__m128 mask = _mm_setr_ps(1.0f, -1.0f, -1.0f, 1.0f);
		x = _mm_mul_ps(x, mask);
		y = _mm_mul_ps(y, mask);
		z = _mm_mul_ps(z, mask);
		w = _mm_mul_ps(w, mask);

		adj.sse128[0] = _mm_shuffle_ps(x, y, _MM_SHUFFLE(1, 3, 1, 3));
		adj.sse128[1] = _mm_shuffle_ps(x, y, _MM_SHUFFLE(0, 2, 0, 2));
		adj.sse128[2] = _mm_shuffle_ps(z, w, _MM_SHUFFLE(1, 3, 1, 3));
		adj.sse128[3] = _mm_shuffle_ps(z, w, _MM_SHUFFLE(0, 2, 0, 2));
#else
		//d11
		adj.rows[0].v[0] = (rows[1].v[1] * ((rows[2].v[2] * rows[3].v[3]) - (rows[2].v[3] * rows[3].v[2])));
		adj.rows[0].v[0] -= (rows[1].v[2] * ((rows[2].v[1] * rows[3].v[3]) - (rows[2].v[3] * rows[3].v[1])));
		adj.rows[0].v[0] += (rows[1].v[3] * ((rows[2].v[1] * rows[3].v[2]) - (rows[2].v[2] * rows[3].v[1])));
		//d12
		adj.rows[1].v[0] = (rows[1].v[0] * ((rows[2].v[2] * rows[3].v[3]) - (rows[2].v[3] * rows[3].v[2])));
		adj.rows[1].v[0] -= (rows[1].v[2] * ((rows[2].v[0] * rows[3].v[3]) - (rows[2].v[3] * rows[3].v[0])));
		adj.rows[1].v[0] += (rows[1].v[3] * ((rows[2].v[0] * rows[3].v[2]) - (rows[2].v[2] * rows[3].v[0])));
		adj.rows[1].v[0] *= -1.0f;
		//d13
		adj.rows[2].v[0] = (rows[1].v[0] * ((rows[2].v[1] * rows[3].v[3]) - (rows[2].v[3] * rows[3].v[1])));
		adj.rows[2].v[0] -= (rows[1].v[1] * ((rows[2].v[0] * rows[3].v[3]) - (rows[2].v[3] * rows[3].v[0])));
		adj.rows[2].v[0] += (rows[1].v[3] * ((rows[2].v[0] * rows[3].v[1]) - (rows[2].v[1] * rows[3].v[0])));
		//d14
		adj.rows[3].v[0] = (rows[1].v[0] * ((rows[2].v[1] * rows[3].v[2]) - (rows[2].v[2] * rows[3].v[1])));
		adj.rows[3].v[0] -= (rows[1].v[1] * ((rows[2].v[0] * rows[3].v[2]) - (rows[2].v[2] * rows[3].v[0])));
		adj.rows[3].v[0] += (rows[1].v[2] * ((rows[2].v[0] * rows[3].v[1]) - (rows[2].v[1] * rows[3].v[0])));
		adj.rows[3].v[0] *= -1.0f;

		//d21
		adj.rows[0].v[1] = (rows[0].v[1] * ((rows[2].v[2] * rows[3].v[3]) - (rows[2].v[3] * rows[3].v[2])));
		adj.rows[0].v[1] -= (rows[0].v[2] * ((rows[2].v[1] * rows[3].v[3]) - (rows[2].v[3] * rows[3].v[1])));
		adj.rows[0].v[1] += (rows[0].v[3] * ((rows[2].v[1] * rows[3].v[2]) - (rows[2].v[2] * rows[3].v[1])));
		adj.rows[0].v[1] *= -1.0f;
		//d22
		adj.rows[1].v[1] = (rows[0].v[0] * ((rows[2].v[2] * rows[3].v[3]) - (rows[2].v[3] * rows[3].v[2])));
		adj.rows[1].v[1] -= (rows[0].v[2] * ((rows[2].v[0] * rows[3].v[3]) - (rows[2].v[3] * rows[3].v[0])));
		adj.rows[1].v[1] += (rows[0].v[3] * ((rows[2].v[0] * rows[3].v[2]) - (rows[2].v[2] * rows[3].v[0])));
		//d23
		adj.rows[2].v[1] = (rows[0].v[0] * ((rows[2].v[1] * rows[3].v[3]) - (rows[2].v[3] * rows[3].v[1])));
		adj.rows[2].v[1] -= (rows[0].v[1] * ((rows[2].v[0] * rows[3].v[3]) - (rows[2].v[3] * rows[3].v[0])));
		adj.rows[2].v[1] += (rows[0].v[3] * ((rows[2].v[0] * rows[3].v[1]) - (rows[2].v[1] * rows[3].v[0])));
		adj.rows[2].v[1] *= -1.0f;
		//d24
		adj.rows[3].v[1] = (rows[0].v[0] * ((rows[2].v[1] * rows[3].v[2]) - (rows[2].v[2] * rows[3].v[1])));
		adj.rows[3].v[1] -= (rows[0].v[1] * ((rows[2].v[0] * rows[3].v[2]) - (rows[2].v[2] * rows[3].v[0])));
		adj.rows[3].v[1] += (rows[0].v[2] * ((rows[2].v[0] * rows[3].v[1]) - (rows[2].v[1] * rows[3].v[0])));

		//d31
		adj.rows[0].v[2] = (rows[0].v[1] * ((rows[1].v[2] * rows[3].v[3]) - (rows[1].v[3] * rows[3].v[2])));
		adj.rows[0].v[2] -= (rows[0].v[2] * ((rows[1].v[1] * rows[3].v[3]) - (rows[1].v[3] * rows[3].v[1])));
		adj.rows[0].v[2] += (rows[0].v[3] * ((rows[1].v[1] * rows[3].v[2]) - (rows[1].v[2] * rows[3].v[1])));
		//d32
		adj.rows[1].v[2] = (rows[0].v[0] * ((rows[1].v[2] * rows[3].v[3]) - (rows[1].v[3] * rows[3].v[2])));
		adj.rows[1].v[2] -= (rows[0].v[2] * ((rows[1].v[0] * rows[3].v[3]) - (rows[1].v[3] * rows[3].v[0])));
		adj.rows[1].v[2] += (rows[0].v[3] * ((rows[1].v[0] * rows[3].v[2]) - (rows[1].v[2] * rows[3].v[0])));
		adj.rows[1].v[2] *= -1.0f;
		//d33
		adj.rows[2].v[2] = (rows[0].v[0] * ((rows[1].v[1] * rows[3].v[3]) - (rows[1].v[3] * rows[3].v[1])));
		adj.rows[2].v[2] -= (rows[0].v[1] * ((rows[1].v[0] * rows[3].v[3]) - (rows[1].v[3] * rows[3].v[0])));
		adj.rows[2].v[2] += (rows[0].v[3] * ((rows[1].v[0] * rows[3].v[1]) - (rows[1].v[1] * rows[3].v[0])));
		//d34
		adj.rows[3].v[2] = (rows[0].v[0] * ((rows[1].v[1] * rows[3].v[2]) - (rows[1].v[2] * rows[3].v[1])));
		adj.rows[3].v[2] -= (rows[0].v[1] * ((rows[1].v[0] * rows[3].v[2]) - (rows[1].v[2] * rows[3].v[0])));
		adj.rows[3].v[2] += (rows[0].v[2] * ((rows[1].v[0] * rows[3].v[1]) - (rows[1].v[1] * rows[3].v[0])));
		adj.rows[3].v[2] *= -1.0f;

		//d41
		adj.rows[0].v[3] = (rows[0].v[1] * ((rows[1].v[2] * rows[2].v[3]) - (rows[1].v[3] * rows[2].v[2])));
		adj.rows[0].v[3] -= (rows[0].v[2] * ((rows[1].v[1] * rows[2].v[3]) - (rows[1].v[3] * rows[2].v[1])));
		adj.rows[0].v[3] += (rows[0].v[3] * ((rows[1].v[1] * rows[2].v[2]) - (rows[1].v[2] * rows[2].v[1])));
		adj.rows[0].v[3] *= -1.0f;
		//d42
		adj.rows[1].v[3] = (rows[0].v[0] * ((rows[1].v[2] * rows[2].v[3]) - (rows[1].v[3] * rows[2].v[2])));
		adj.rows[1].v[3] -= (rows[0].v[2] * ((rows[1].v[0] * rows[2].v[3]) - (rows[1].v[3] * rows[2].v[0])));
		adj.rows[1].v[3] += (rows[0].v[3] * ((rows[1].v[0] * rows[2].v[2]) - (rows[1].v[2] * rows[2].v[0])));
		//d43
		adj.rows[2].v[3] = (rows[0].v[0] * ((rows[1].v[1] * rows[2].v[3]) - (rows[1].v[3] * rows[2].v[1])));
		adj.rows[2].v[3] -= (rows[0].v[1] * ((rows[1].v[0] * rows[2].v[3]) - (rows[1].v[3] * rows[2].v[0])));
		adj.rows[2].v[3] += (rows[0].v[3] * ((rows[1].v[0] * rows[2].v[1]) - (rows[1].v[1] * rows[2].v[0])));
		adj.rows[2].v[3] *= -1.0f;
		//d44
		adj.rows[3].v[3] = (rows[0].v[0] * ((rows[1].v[1] * rows[2].v[2]) - (rows[1].v[2] * rows[2].v[1])));
		adj.rows[3].v[3] -= (rows[0].v[1] * ((rows[1].v[0] * rows[2].v[2]) - (rows[1].v[2] * rows[2].v[0])));
		adj.rows[3].v[3] += (rows[0].v[2] * ((rows[1].v[0] * rows[2].v[1]) - (rows[1].v[1] * rows[2].v[0])));
#endif
		return adj;
	}


	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	float Matrix4::Determinant() const
	{
#if defined(SSE_INTRIN)
		__m128 ab = _mm_mul_ps(_mm_movelh_ps(sse128[0], sse128[1]), _mm_movehl_ps(sse128[1], sse128[0]));
		__m128 dc = _mm_mul_ps(_mm_movelh_ps(sse128[2], sse128[3]), _mm_movehl_ps(sse128[3], sse128[2]));

		__m128 detA = _mm_set_ps1((m[0] * m[5]) - (m[1] * m[4]));
		__m128 detB = _mm_set_ps1((m[2] * m[7]) - (m[3] * m[6]));
		__m128 detC = _mm_set_ps1((m[8] * m[13]) - (m[9] * m[12]));
		__m128 detD = _mm_set_ps1((m[10] * m[15]) - (m[11] * m[14]));

		__m128 detM = _mm_mul_ps(detA, detD);
		detM = _mm_add_ps(detM, _mm_mul_ps(detB, detC));
		detM = _mm_sub_ps(detM, HorizonatalSum(_mm_mul_ps(ab, Swizzle128(dc, 0, 2, 1, 3))));

		return _mm_cvtss_f32(detM);
#else
		float det = 0.0f;

		//d11
		det = rows[0].v[0] * (
			(rows[1].v[1] * ((rows[2].v[2] * rows[3].v[3]) - (rows[2].v[3] * rows[3].v[2]))) -
			(rows[1].v[2] * ((rows[2].v[1] * rows[3].v[3]) - (rows[2].v[3] * rows[3].v[1]))) +
			(rows[1].v[3] * ((rows[2].v[1] * rows[3].v[2]) - (rows[2].v[2] * rows[3].v[1]))));
		//d12
		det -= rows[0].v[1] * (
			(rows[1].v[0] * ((rows[2].v[2] * rows[3].v[3]) - (rows[2].v[3] * rows[3].v[2]))) -
			(rows[1].v[2] * ((rows[2].v[0] * rows[3].v[3]) - (rows[2].v[3] * rows[3].v[0]))) +
			(rows[1].v[3] * ((rows[2].v[0] * rows[3].v[2]) - (rows[2].v[2] * rows[3].v[0]))));
		//d13
		det += rows[0].v[2] * (
			(rows[1].v[0] * ((rows[2].v[1] * rows[3].v[3]) - (rows[2].v[3] * rows[3].v[1]))) -
			(rows[1].v[1] * ((rows[2].v[0] * rows[3].v[3]) - (rows[2].v[3] * rows[3].v[0]))) +
			(rows[1].v[3] * ((rows[2].v[0] * rows[3].v[1]) - (rows[2].v[1] * rows[3].v[0]))));
		//d14
		det -= rows[0].v[3] * (
			(rows[1].v[0] * ((rows[2].v[1] * rows[3].v[2]) - (rows[2].v[2] * rows[3].v[1]))) -
			(rows[1].v[1] * ((rows[2].v[0] * rows[3].v[2]) - (rows[2].v[2] * rows[3].v[0]))) +
			(rows[1].v[2] * ((rows[2].v[0] * rows[3].v[1]) - (rows[2].v[1] * rows[3].v[0]))));

		return det;
#endif
	}


	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	std::string Matrix4::ToString() const
	{
		return std::string(rows[0].ToString() + '\n' + rows[1].ToString() + '\n' + rows[2].ToString() + '\n' + rows[3].ToString());
	}


	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	bool Matrix4::operator==(const Matrix4& other) const
	{
		return Equals(other);
	}


	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	bool Matrix4::operator!=(const Matrix4& other) const
	{
		return !Equals(other);
	}


	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	float Matrix4::operator()(unsigned int r, unsigned int c) const
	{
		return GetElement(r, c);
	}


	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	float& Matrix4::operator()(unsigned int r, unsigned int c)
	{
		return GetElement(r, c);
	}


	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	Vector4 Matrix4::operator()(unsigned int r) const
	{
		return GetRow(r);
	}


	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	Vector4& Matrix4::operator()(unsigned int r)
	{
		return GetRow(r);
	}


	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	Vector4 operator*(const Vector4& left, const Matrix4& right)
	{
		return right.Multiply(left);
	}


	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	Vector4& operator*=(Vector4& left, const Matrix4& right)
	{
		return left = right.Multiply(left);
	}


	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	Matrix4 operator+(Matrix4 left, const Matrix4& right)
	{
		return left.Add(right);
	}


	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	Matrix4 operator-(Matrix4 left, const Matrix4& right)
	{
		return left.Subtract(right);
	}


	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	Matrix4 operator*(Matrix4 left, const Matrix4& right)
	{
		return left.Multiply(right);
	}


	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	Matrix4 operator+(Matrix4 left, float right)
	{
		return left.Add(right);
	}


	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	Matrix4 operator-(Matrix4 left, float right)
	{
		return left.Subtract(right);
	}


	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	Matrix4 operator*(Matrix4 left, float right)
	{
		return left.Multiply(right);
	}


	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	Matrix4 operator/(Matrix4 left, float right)
	{
		return left.Divide(right);
	}


	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	Matrix4 operator*(float left, Matrix4 right)
	{
		return right.Multiply(left);
	}


	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	Matrix4& Matrix4::operator=(const Matrix4& other)
	{
		if (this != &other)
			memcpy(m, other.m, sizeof(float) * 16);

		return *this;
	}


	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	Matrix4& Matrix4::operator+=(const Matrix4& right)
	{
		return Add(right);
	}


	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	Matrix4& Matrix4::operator-=(const Matrix4& right)
	{
		return Subtract(right);
	}


	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	Matrix4& Matrix4::operator*=(const Matrix4& right)
	{
		return Multiply(right);
	}


	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	Matrix4& Matrix4::operator+=(float right)
	{
		return Add(right);
	}


	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	Matrix4& Matrix4::operator-=(float right)
	{
		return Subtract(right);
	}


	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	Matrix4& Matrix4::operator*=(float right)
	{
		return Multiply(right);
	}


	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	Matrix4& Matrix4::operator/=(float right)
	{
		return Divide(right);
	}


	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	Matrix4 Matrix4::Nan()
	{
		return Matrix4(Vector4::Nan(), Vector4::Nan(), Vector4::Nan(), Vector4::Nan());
	}


	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	Matrix4 Matrix4::Identity()
	{
		return Matrix4(1.0f);
	}


	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	Matrix4 Matrix4::Perspective(float fovRad, float aspectWiHe, float farZ, float nearZ)
	{
		if (fovRad < (PI / 180.0) || fovRad >(PI - (PI / 180.0)))
			return Matrix4::Nan();

		Matrix4 m;

		float yScale = 1.0f / tan(fovRad * 0.5f);
		float xScale = yScale / aspectWiHe;

		m.rows[0].v[0] = xScale;
		m.rows[1].v[1] = yScale;
		m.rows[2].v[2] = farZ / (farZ - nearZ);
		m.rows[3].v[2] = (-nearZ * farZ) / (farZ - nearZ);
		m.rows[2].v[3] = 1.0f;

		return m;
	}


	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	Matrix4 Matrix4::Orthographic(float width, float height, float farZ, float nearZ)
	{
		Matrix4 m(1.0f);

		m.rows[0].v[0] = 2.0f / width;
		m.rows[1].v[1] = 2.0f / height;
		m.rows[2].v[2] = 1.0f / (farZ - nearZ);
		m.rows[3].v[2] = -nearZ / (farZ - nearZ);
		m.rows[3].v[3] = 1.0f;

		return m;
	}


	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	Matrix4 Matrix4::Orthographic(float left, float right, float top, float bottom, float farZ, float nearZ)
	{
		Matrix4 m(1.0f);

		m.rows[0].v[0] = 2.0f / (right - left);
		m.rows[3].v[0] = (left + right) / (left - right);
		m.rows[1].v[1] = 2.0f / (top - bottom);
		m.rows[3].v[1] = (top + bottom) / (bottom - top);
		m.rows[2].v[2] = 1.0f / (farZ - nearZ);
		m.rows[3].v[2] = nearZ / (nearZ - farZ);
		m.rows[3].v[3] = 1.0f;

		return m;
	}


	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	Matrix4 Matrix4::Orthographic(float aspectWiHe, float farZ, float nearZ)
	{
		Matrix4 m(1.0f);

		m.rows[0].v[0] = 2.0f / aspectWiHe;
		m.rows[1].v[1] = 2.0f;
		m.rows[2].v[2] = 1.0f / (farZ - nearZ);
		m.rows[3].v[2] = -nearZ / (farZ - nearZ);
		m.rows[3].v[3] = 1.0f;

		return m;
	}


	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	Matrix4 Matrix4::LookAt(const Vector3& up, const Vector3& at, const Vector3& position)
	{
		Matrix4 m(1.0f);
		Vector3 z((at - position).Normalize());
		Vector3 x((up.UnitVector().Cross(z)).Normalize());
		Vector3 y(z.Cross(x));

		m.rows[0].v[0] = x.x;
		m.rows[1].v[0] = x.y;
		m.rows[2].v[0] = x.z;
		m.rows[3].v[0] = -x.Dot(position);

		m.rows[0].v[1] = y.x;
		m.rows[1].v[1] = y.y;
		m.rows[2].v[1] = y.z;
		m.rows[3].v[1] = -y.Dot(position);

		m.rows[0].v[2] = z.x;
		m.rows[1].v[2] = z.y;
		m.rows[2].v[2] = z.z;
		m.rows[3].v[2] = -z.Dot(position);

		m.rows[3].v[3] = 1.0f;

		return m;
	}


	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	Matrix4 Matrix4::Translation(const Vector3& translation)
	{
		Matrix4 m(1.0f);

		m.rows[3].v[0] = translation.x;
		m.rows[3].v[1] = translation.y;
		m.rows[3].v[2] = translation.z;

		return m;
	}


	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	Matrix4 Matrix4::Rotation(const Vector3& axis, float angleRad)
	{
		Matrix4 r(1.0f);
		Vector3 unitAxis(axis.UnitVector());

		float cosine = cosf(angleRad);
		float sine = sinf(angleRad);
		float c = 1 - cosine;

		r.rows[0].v[0] = (unitAxis.x * unitAxis.x * c) + cosine;
		r.rows[0].v[1] = (unitAxis.x * unitAxis.y * c) - (sine * unitAxis.z);
		r.rows[0].v[2] = (unitAxis.x * unitAxis.z * c) + (sine * unitAxis.y);

		r.rows[1].v[0] = (unitAxis.y * unitAxis.x * c) + (sine * unitAxis.z);
		r.rows[1].v[1] = (unitAxis.y * unitAxis.y * c) + cosine;
		r.rows[1].v[2] = (unitAxis.y * unitAxis.z * c) - (sine * unitAxis.x);

		r.rows[2].v[0] = (unitAxis.z * unitAxis.x * c) - (sine * unitAxis.y);
		r.rows[2].v[1] = (unitAxis.z * unitAxis.y * c) + (sine * unitAxis.x);
		r.rows[2].v[2] = (unitAxis.z * unitAxis.z * c) + cosine;

		r.rows[3].v[3] = 1.0f;

		return r;
	}


	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	Matrix4 Matrix4::RotationX(float angleRad)
	{
		Matrix4 r(1.0f);

		float cosine = cosf(angleRad);
		float sine = sinf(angleRad);

		r.rows[1].v[1] = cosine;
		r.rows[1].v[2] = -sine;
		r.rows[2].v[1] = sine;
		r.rows[2].v[2] = cosine;

		return r;
	}


	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	Matrix4 Matrix4::RotationY(float angleRad)
	{
		Matrix4 r(1.0f);

		float cosine = cosf(angleRad);
		float sine = sinf(angleRad);

		r.rows[0].v[0] = cosine;
		r.rows[0].v[2] = sine;
		r.rows[2].v[0] = -sine;
		r.rows[2].v[2] = cosine;

		return r;
	}


	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	Matrix4 Matrix4::RotationZ(float angleRad)
	{
		Matrix4 r(1.0f);

		float cosine = cosf(angleRad);
		float sine = sinf(angleRad);

		r.rows[0].v[0] = cosine;
		r.rows[0].v[1] = -sine;
		r.rows[1].v[0] = sine;
		r.rows[1].v[1] = cosine;

		return r;
	}


	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	Matrix4 Matrix4::Rotation(float angleRadX, float angleRadY, float angleRadZ)
	{
		Matrix4 rot(1.0f);

		float sinX = sinf(angleRadX);
		float sinY = sinf(angleRadY);
		float sinZ = sinf(angleRadZ);
		
		float cosX = cosf(angleRadX);
		float cosY = cosf(angleRadY);
		float cosZ = cosf(angleRadZ);

		rot.rows[0].v[0] = (cosY * cosZ) - (sinX * sinY * sinZ);
		rot.rows[0].v[1] = -(cosX * sinZ);
		rot.rows[0].v[2] = (cosZ * sinY) + (sinX * sinY * sinZ);

		rot.rows[1].v[0] = (cosZ * sinX * sinY) + (cosY * sinZ);
		rot.rows[1].v[1] = cosX * cosZ;
		rot.rows[1].v[2] = (sinY * sinZ) -(cosY * cosZ * sinX);

		rot.rows[2].v[0] = -(cosX * sinY);
		rot.rows[2].v[1] = sinX;
		rot.rows[2].v[2] = cosX * cosY;

		return rot;
	}


	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	Matrix4 Matrix4::Scale(const Vector3& scale)
	{
		Matrix4 s(1.0f);

		s.rows[0].v[0] = scale.x;
		s.rows[1].v[1] = scale.y;
		s.rows[2].v[2] = scale.z;

		return s;
	}


	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	Matrix4 Matrix4::Scale(float scale)
	{
		Matrix4 s(scale);
		s.m[15] = 1;

		return s;
	}
}