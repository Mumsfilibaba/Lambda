#pragma once
#include "Vector3.h"
#include "Vector4.h"

namespace Math
{
	struct MATRIXALIGN Matrix4
	{
	public:
		Matrix4();
		Matrix4(float value);
		Matrix4(const Matrix4& other);
		Matrix4(const Vector4& r1, const Vector4& r2, const Vector4& r3, const Vector4& r4);

		Vector4 Multiply(const Vector4& vector) const;

		Matrix4& Multiply(const Matrix4& other);
		Matrix4& Multiply(float scalar);
		Matrix4& Add(const Matrix4& other);
		Matrix4& Add(float scalar);
		Matrix4& Subtract(const Matrix4& other);
		Matrix4& Subtract(float scalar);
		Matrix4& Divide(float scalar);

		bool Equals(const Matrix4& other) const;

		float* GetArray();
		const float* GetArray() const;
		float GetElement(uint32 r, uint32 c) const;
		float& GetElement(uint32 r, uint32 c);
		Vector4 GetRow(uint32 r) const;
		Vector4& GetRow(uint32 r);

		Matrix4& Transpose();
		Matrix4& Invert();
		Matrix4 TransposeOf() const;
		Matrix4 InverseOf() const;

		Matrix4 Cofactor() const;
		Matrix4 Adjugate() const;

		float Determinant() const;

		std::string ToString() const;

		bool operator==(const Matrix4& other) const;
		bool operator!=(const Matrix4& other) const;

		float operator()(uint32 r, uint32 c) const;
		float& operator()(uint32 r, uint32 c);
		Vector4 operator()(uint32 r) const;
		Vector4& operator()(uint32 r);

		Matrix4& operator=(const Matrix4& other);
		Matrix4& operator+=(const Matrix4& right);
		Matrix4& operator-=(const Matrix4& right);
		Matrix4& operator*=(const Matrix4& right);
		Matrix4& operator+=(float right);
		Matrix4& operator-=(float right);
		Matrix4& operator*=(float right);
		Matrix4& operator/=(float right);

		friend Vector4& operator*=(Vector4& left, const Matrix4& right);
		friend Vector4 operator*(const Vector4& left, const Matrix4& right);

		friend Matrix4 operator+(Matrix4 left, const Matrix4& right);
		friend Matrix4 operator-(Matrix4 left, const Matrix4& right);
		friend Matrix4 operator*(Matrix4 left, const Matrix4& right);
		friend Matrix4 operator+(Matrix4 left, float right);
		friend Matrix4 operator-(Matrix4 left, float right);
		friend Matrix4 operator*(Matrix4 left, float right);
		friend Matrix4 operator/(Matrix4 left, float right);
		friend Matrix4 operator*(float left, Matrix4 right);

	private:
		union
		{
			float m[16];
			Vector4 Rows[4];
			SSE_ARRAY(Xmm, 4);
		};

	public:
		static Matrix4 Nan();
		static Matrix4 Identity();

		static Matrix4 Perspective(float fovRad, float aspectWiHe, float farZ, float nearZ);
		static Matrix4 Orthographic(float aspectWiHe, float farZ, float nearZ);
		static Matrix4 Orthographic(float width, float height, float farZ, float nearZ);
		static Matrix4 Orthographic(float leftX, float rightX, float topY, float bottomY, float farZ, float nearZ);
		static Matrix4 LookAt(const Vector3& up, const Vector3& at, const Vector3& position);

		static Matrix4 Translation(const Vector3& translation);

		static Matrix4 RotationYawPitchRoll(float angleRadX, float angleRadY, float angleRadZ);
		static Matrix4 Rotation(const Vector3& axis, float angleRad);
		static Matrix4 RotationX(float angleRad);
		static Matrix4 RotationY(float angleRad);
		static Matrix4 RotationZ(float angleRad);

		static Matrix4 Scale(const Vector3& scale);
		static Matrix4 Scale(float scale);
	};

	inline Matrix4::Matrix4()
	{
		memset(m, 0, sizeof(float) * 16);
	}

	inline Matrix4::Matrix4(float diagonal)
	{
		memset(m, 0, sizeof(float) * 16);

		Rows[0][0] = diagonal;
		Rows[1][1] = diagonal;
		Rows[2][2] = diagonal;
		Rows[3][3] = diagonal;
	}

	inline Matrix4::Matrix4(const Vector4& r1, const Vector4& r2, const Vector4& r3, const Vector4& r4)
	{
		Rows[0] = r1;
		Rows[1] = r2;
		Rows[2] = r3;
		Rows[3] = r4;
	}

	inline Matrix4::Matrix4(const Matrix4& other)
	{
		*this = other;
	}

	inline Vector4 Matrix4::Multiply(const Vector4& vector) const
	{
#if defined(SSE_INTRIN)
		__m128	r1 = _mm_mul_ps(_mm_shuffle_ps(vector.Xmm, vector.Xmm, 0), Xmm[0]);
		r1 = _mm_add_ps(r1, _mm_mul_ps(_mm_shuffle_ps(vector.Xmm, vector.Xmm, 0x55), Xmm[1]));
		r1 = _mm_add_ps(r1, _mm_mul_ps(_mm_shuffle_ps(vector.Xmm, vector.Xmm, 0xaa), Xmm[2]));
		r1 = _mm_add_ps(r1, _mm_mul_ps(_mm_shuffle_ps(vector.Xmm, vector.Xmm, 0xff), Xmm[3]));
		return r1;
#else
		float t[4];

		t[0] = (vector[0] * Rows[0][0]) + (vector[1] * Rows[1][0]) + (vector[2] * Rows[2][0]) + (vector[3] * Rows[3][0]);
		t[1] = (vector[0] * Rows[0][1]) + (vector[1] * Rows[1][1]) + (vector[2] * Rows[2][1]) + (vector[3] * Rows[3][1]);
		t[2] = (vector[0] * Rows[0][2]) + (vector[1] * Rows[1][2]) + (vector[2] * Rows[2][2]) + (vector[3] * Rows[3][2]);
		t[3] = (vector[0] * Rows[0][3]) + (vector[1] * Rows[1][3]) + (vector[2] * Rows[2][3]) + (vector[3] * Rows[3][3]);

		return Vector4(t[0], t[1], t[2], t[3]);
#endif
	}

	inline Matrix4& Matrix4::Multiply(const Matrix4& other)
	{
#if defined(SSE_INTRIN)
		__m128	r0 = _mm_mul_ps(_mm_shuffle_ps(Xmm[0], Xmm[0], 0), other.Xmm[0]);
		r0 = _mm_add_ps(r0, _mm_mul_ps(_mm_shuffle_ps(Xmm[0], Xmm[0], 0x55), other.Xmm[1]));
		r0 = _mm_add_ps(r0, _mm_mul_ps(_mm_shuffle_ps(Xmm[0], Xmm[0], 0xaa), other.Xmm[2]));
		r0 = _mm_add_ps(r0, _mm_mul_ps(_mm_shuffle_ps(Xmm[0], Xmm[0], 0xff), other.Xmm[3]));

		__m128	r1 = _mm_mul_ps(_mm_shuffle_ps(Xmm[1], Xmm[1], 0), other.Xmm[0]);
		r1 = _mm_add_ps(r1, _mm_mul_ps(_mm_shuffle_ps(Xmm[1], Xmm[1], 0x55), other.Xmm[1]));
		r1 = _mm_add_ps(r1, _mm_mul_ps(_mm_shuffle_ps(Xmm[1], Xmm[1], 0xaa), other.Xmm[2]));
		r1 = _mm_add_ps(r1, _mm_mul_ps(_mm_shuffle_ps(Xmm[1], Xmm[1], 0xff), other.Xmm[3]));

		__m128	r2 = _mm_mul_ps(_mm_shuffle_ps(Xmm[2], Xmm[2], 0), other.Xmm[0]);
		r2 = _mm_add_ps(r2, _mm_mul_ps(_mm_shuffle_ps(Xmm[2], Xmm[2], 0x55), other.Xmm[1]));
		r2 = _mm_add_ps(r2, _mm_mul_ps(_mm_shuffle_ps(Xmm[2], Xmm[2], 0xaa), other.Xmm[2]));
		r2 = _mm_add_ps(r2, _mm_mul_ps(_mm_shuffle_ps(Xmm[2], Xmm[2], 0xff), other.Xmm[3]));

		__m128	r3 = _mm_mul_ps(_mm_shuffle_ps(Xmm[3], Xmm[3], 0), other.Xmm[0]);
		r3 = _mm_add_ps(r3, _mm_mul_ps(_mm_shuffle_ps(Xmm[3], Xmm[3], 0x55), other.Xmm[1]));
		r3 = _mm_add_ps(r3, _mm_mul_ps(_mm_shuffle_ps(Xmm[3], Xmm[3], 0xaa), other.Xmm[2]));
		r3 = _mm_add_ps(r3, _mm_mul_ps(_mm_shuffle_ps(Xmm[3], Xmm[3], 0xff), other.Xmm[3]));

		Xmm[0] = r0;
		Xmm[1] = r1;
		Xmm[2] = r2;
		Xmm[3] = r3;
		return *this;
#else
		float t[16];

		t[0] = (Rows[0][0] * other.Rows[0][0]) + (Rows[0][1] * other.Rows[1][0]) + (Rows[0][2] * other.Rows[2][0]) + (Rows[0][3] * other.Rows[3][0]);
		t[1] = (Rows[0][0] * other.Rows[0][1]) + (Rows[0][1] * other.Rows[1][1]) + (Rows[0][2] * other.Rows[2][1]) + (Rows[0][3] * other.Rows[3][1]);
		t[2] = (Rows[0][0] * other.Rows[0][2]) + (Rows[0][1] * other.Rows[1][2]) + (Rows[0][2] * other.Rows[2][2]) + (Rows[0][3] * other.Rows[3][2]);
		t[3] = (Rows[0][0] * other.Rows[0][3]) + (Rows[0][1] * other.Rows[1][3]) + (Rows[0][2] * other.Rows[2][3]) + (Rows[0][3] * other.Rows[3][3]);

		t[4] = (Rows[1][0] * other.Rows[0][0]) + (Rows[1][1] * other.Rows[1][0]) + (Rows[1][2] * other.Rows[2][0]) + (Rows[1][3] * other.Rows[3][0]);
		t[5] = (Rows[1][0] * other.Rows[0][1]) + (Rows[1][1] * other.Rows[1][1]) + (Rows[1][2] * other.Rows[2][1]) + (Rows[1][3] * other.Rows[3][1]);
		t[6] = (Rows[1][0] * other.Rows[0][2]) + (Rows[1][1] * other.Rows[1][2]) + (Rows[1][2] * other.Rows[2][2]) + (Rows[1][3] * other.Rows[3][2]);
		t[7] = (Rows[1][0] * other.Rows[0][3]) + (Rows[1][1] * other.Rows[1][3]) + (Rows[1][2] * other.Rows[2][3]) + (Rows[1][3] * other.Rows[3][3]);

		t[8] = (Rows[2][0] * other.Rows[0][0]) + (Rows[2][1] * other.Rows[1][0]) + (Rows[2][2] * other.Rows[2][0]) + (Rows[2][3] * other.Rows[3][0]);
		t[9] = (Rows[2][0] * other.Rows[0][1]) + (Rows[2][1] * other.Rows[1][1]) + (Rows[2][2] * other.Rows[2][1]) + (Rows[2][3] * other.Rows[3][1]);
		t[10] = (Rows[2][0] * other.Rows[0][2]) + (Rows[2][1] * other.Rows[1][2]) + (Rows[2][2] * other.Rows[2][2]) + (Rows[2][3] * other.Rows[3][2]);
		t[11] = (Rows[2][0] * other.Rows[0][3]) + (Rows[2][1] * other.Rows[1][3]) + (Rows[2][2] * other.Rows[2][3]) + (Rows[2][3] * other.Rows[3][3]);

		t[12] = (Rows[3][0] * other.Rows[0][0]) + (Rows[3][1] * other.Rows[1][0]) + (Rows[3][2] * other.Rows[2][0]) + (Rows[3][3] * other.Rows[3][0]);
		t[13] = (Rows[3][0] * other.Rows[0][1]) + (Rows[3][1] * other.Rows[1][1]) + (Rows[3][2] * other.Rows[2][1]) + (Rows[3][3] * other.Rows[3][1]);
		t[14] = (Rows[3][0] * other.Rows[0][2]) + (Rows[3][1] * other.Rows[1][2]) + (Rows[3][2] * other.Rows[2][2]) + (Rows[3][3] * other.Rows[3][2]);
		t[15] = (Rows[3][0] * other.Rows[0][3]) + (Rows[3][1] * other.Rows[1][3]) + (Rows[3][2] * other.Rows[2][3]) + (Rows[3][3] * other.Rows[3][3]);

		memcpy(m, t, sizeof(float) * 16);
		return *this;
#endif
	}

	inline Matrix4& Matrix4::Multiply(float scalar)
	{
#if defined(SSE_INTRIN)
		__m128 scalars = _mm_set_ps1(scalar);
		Xmm[0] = _mm_mul_ps(Xmm[0], scalars);
		Xmm[1] = _mm_mul_ps(Xmm[1], scalars);
		Xmm[2] = _mm_mul_ps(Xmm[2], scalars);
		Xmm[3] = _mm_mul_ps(Xmm[3], scalars);
		return *this;
#else
		for (int i = 0; i < 16; i++)
			m[i] *= scalar;
		return *this;
#endif
	}

	inline Matrix4& Matrix4::Add(const Matrix4& other)
	{
#if defined(SSE_INTRIN)
		Xmm[0] = _mm_add_ps(Xmm[0], other.Xmm[0]);
		Xmm[1] = _mm_add_ps(Xmm[1], other.Xmm[1]);
		Xmm[2] = _mm_add_ps(Xmm[2], other.Xmm[2]);
		Xmm[3] = _mm_add_ps(Xmm[3], other.Xmm[3]);
		return *this;
#else
		for (int i = 0; i < 16; i++)
			m[i] += other.m[i];
		return *this;
#endif
	}

	inline Matrix4& Matrix4::Add(float scalar)
	{
#if defined(SSE_INTRIN)
		__m128 scalars = _mm_set_ps1(scalar);
				Xmm[0] = _mm_add_ps(Xmm[0], scalars);
				Xmm[1] = _mm_add_ps(Xmm[1], scalars);
				Xmm[2] = _mm_add_ps(Xmm[2], scalars);
				Xmm[3] = _mm_add_ps(Xmm[3], scalars);
		return *this;
#else
		for (int i = 0; i < 16; i++)
			m[i] += scalar;
		return *this;
#endif
	}

	inline Matrix4& Matrix4::Subtract(const Matrix4& other)
	{
#if defined(SSE_INTRIN)
		Xmm[0] = _mm_sub_ps(Xmm[0], other.Xmm[0]);
		Xmm[1] = _mm_sub_ps(Xmm[1], other.Xmm[1]);
		Xmm[2] = _mm_sub_ps(Xmm[2], other.Xmm[2]);
		Xmm[3] = _mm_sub_ps(Xmm[3], other.Xmm[3]);
		return *this;
#else
		for (int i = 0; i < 16; i++)
			m[i] -= other.m[i];
		return *this;
#endif
	}

	inline Matrix4& Matrix4::Subtract(float scalar)
	{
#if defined(SSE_INTRIN)
		__m128 scalars = _mm_set_ps1(scalar);
				Xmm[0] = _mm_sub_ps(Xmm[0], scalars);
				Xmm[1] = _mm_sub_ps(Xmm[1], scalars);
				Xmm[2] = _mm_sub_ps(Xmm[2], scalars);
				Xmm[3] = _mm_sub_ps(Xmm[3], scalars);
		return *this;
#else
		for (int i = 0; i < 16; i++)
			m[i] -= scalar;
		return *this;
#endif
	}

	inline Matrix4& Matrix4::Divide(float scalar)
	{
#if defined(SSE_INTRIN)
		__m128 scalars = _mm_set_ps1(scalar);
				Xmm[0] = _mm_div_ps(Xmm[0], scalars);
				Xmm[1] = _mm_div_ps(Xmm[1], scalars);
				Xmm[2] = _mm_div_ps(Xmm[2], scalars);
				Xmm[3] = _mm_div_ps(Xmm[3], scalars);
		return *this;
#else
		for (int i = 0; i < 16; i++)
			m[i] /= scalar;
		return *this;
#endif
	}

	inline bool Matrix4::Equals(const Matrix4& other) const
	{
		for (int i = 0; i < 16; i++)
		{
			if (m[i] != other.m[i])
				return false;
		}

		return true;
	}

	inline float* Matrix4::GetArray()
	{
		return m;
	}

	inline const float* Matrix4::GetArray() const
	{
		return m;
	}

	inline float Matrix4::GetElement(uint32 r, uint32 c) const
	{
		assert(r < 4);
		assert(c < 4);
		return Rows[r][c];
	}

	inline float& Matrix4::GetElement(uint32 r, uint32 c)
	{
		assert(r < 4);
		assert(c < 4);
		return Rows[r][c];
	}

	inline Vector4 Matrix4::GetRow(uint32 r) const
	{
		assert(r < 4);
		return Rows[r];
	}

	inline Vector4& Matrix4::GetRow(uint32 r)
	{
		assert(r < 4);
		return Rows[r];
	}

	inline Matrix4& Matrix4::Transpose()
	{
#if defined(SSE_INTRIN)
		_MM_TRANSPOSE4_PS(Xmm[0], Xmm[1], Xmm[2], Xmm[3]);
		return *this;
#else
		float temp[16];

		temp[0]		= Rows[0][0];
		temp[1]		= Rows[1][0];
		temp[2]		= Rows[2][0];
		temp[3]		= Rows[3][0];

		temp[4]		= Rows[0][1];
		temp[5]		= Rows[1][1];
		temp[6]		= Rows[2][1];
		temp[7]		= Rows[3][1];

		temp[8]		= Rows[0][2];
		temp[9]		= Rows[1][2];
		temp[10]	= Rows[2][2];
		temp[11]	= Rows[3][2];

		temp[12]	= Rows[0][3];
		temp[13]	= Rows[1][3];
		temp[14]	= Rows[2][3];
		temp[15]	= Rows[3][3];

		memcpy(m, temp, sizeof(float) * 16);
		return *this;
#endif
	}

	inline Matrix4 Matrix4::TransposeOf() const
	{
		return Matrix4(*this).Transpose();
	}

	inline Matrix4& Matrix4::Invert()
	{
#if defined(SSE_INTRIN)
		__m128 detA = _mm_set_ps1((m[0] * m[5]) - (m[1] * m[4]));
		__m128 detB = _mm_set_ps1((m[2] * m[7]) - (m[3] * m[6]));
		__m128 detD = _mm_set_ps1((m[10] * m[15]) - (m[11] * m[14]));
		__m128 detC = _mm_set_ps1((m[8] * m[13]) - (m[9] * m[12]));

		__m128 a = _mm_movelh_ps(Xmm[0], Xmm[1]);
		__m128 b = _mm_movehl_ps(Xmm[1], Xmm[0]);
		__m128 c = _mm_movelh_ps(Xmm[2], Xmm[3]);
		__m128 d = _mm_movehl_ps(Xmm[3], Xmm[2]);

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

		Xmm[0] = _mm_shuffle_ps(x, y, _MM_SHUFFLE(1, 3, 1, 3));
		Xmm[1] = _mm_shuffle_ps(x, y, _MM_SHUFFLE(0, 2, 0, 2));
		Xmm[2] = _mm_shuffle_ps(z, w, _MM_SHUFFLE(1, 3, 1, 3));
		Xmm[3] = _mm_shuffle_ps(z, w, _MM_SHUFFLE(0, 2, 0, 2));
		return *this;
#else
		float det = Determinant();

		if (det == 0.0f)
			*this = Matrix4::Nan();
		else
			*this = (Adjugate() / det);
		return *this;
#endif
	}

	inline Matrix4 Matrix4::InverseOf() const
	{
		return Matrix4(*this).Invert();
	}

	inline Matrix4 Matrix4::Cofactor() const
	{
		return Adjugate().Transpose();
	}

	inline Matrix4 Matrix4::Adjugate() const
	{
#if defined(SSE_INTRIN)
		Matrix4 adj;
		__m128 detA = _mm_set_ps1((m[0] * m[5]) - (m[1] * m[4]));
		__m128 detB = _mm_set_ps1((m[2] * m[7]) - (m[3] * m[6]));
		__m128 detD = _mm_set_ps1((m[10] * m[15]) - (m[11] * m[14]));
		__m128 detC = _mm_set_ps1((m[8] * m[13]) - (m[9] * m[12]));

		__m128 a = _mm_movelh_ps(Xmm[0], Xmm[1]);
		__m128 b = _mm_movehl_ps(Xmm[1], Xmm[0]);
		__m128 c = _mm_movelh_ps(Xmm[2], Xmm[3]);
		__m128 d = _mm_movehl_ps(Xmm[3], Xmm[2]);

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

		adj.Xmm[0] = _mm_shuffle_ps(x, y, _MM_SHUFFLE(1, 3, 1, 3));
		adj.Xmm[1] = _mm_shuffle_ps(x, y, _MM_SHUFFLE(0, 2, 0, 2));
		adj.Xmm[2] = _mm_shuffle_ps(z, w, _MM_SHUFFLE(1, 3, 1, 3));
		adj.Xmm[3] = _mm_shuffle_ps(z, w, _MM_SHUFFLE(0, 2, 0, 2));
		return adj;
#else
		Matrix4 adj;
		//d11
		adj.Rows[0][0] = (Rows[1][1] * ((Rows[2][2] * Rows[3][3]) - (Rows[2][3] * Rows[3][2])));
		adj.Rows[0][0] -= (Rows[1][2] * ((Rows[2][1] * Rows[3][3]) - (Rows[2][3] * Rows[3][1])));
		adj.Rows[0][0] += (Rows[1][3] * ((Rows[2][1] * Rows[3][2]) - (Rows[2][2] * Rows[3][1])));
		//d12
		adj.Rows[1][0] = (Rows[1][0] * ((Rows[2][2] * Rows[3][3]) - (Rows[2][3] * Rows[3][2])));
		adj.Rows[1][0] -= (Rows[1][2] * ((Rows[2][0] * Rows[3][3]) - (Rows[2][3] * Rows[3][0])));
		adj.Rows[1][0] += (Rows[1][3] * ((Rows[2][0] * Rows[3][2]) - (Rows[2][2] * Rows[3][0])));
		adj.Rows[1][0] *= -1.0f;
		//d13
		adj.Rows[2][0] = (Rows[1][0] * ((Rows[2][1] * Rows[3][3]) - (Rows[2][3] * Rows[3][1])));
		adj.Rows[2][0] -= (Rows[1][1] * ((Rows[2][0] * Rows[3][3]) - (Rows[2][3] * Rows[3][0])));
		adj.Rows[2][0] += (Rows[1][3] * ((Rows[2][0] * Rows[3][1]) - (Rows[2][1] * Rows[3][0])));
		//d14
		adj.Rows[3][0] = (Rows[1][0] * ((Rows[2][1] * Rows[3][2]) - (Rows[2][2] * Rows[3][1])));
		adj.Rows[3][0] -= (Rows[1][1] * ((Rows[2][0] * Rows[3][2]) - (Rows[2][2] * Rows[3][0])));
		adj.Rows[3][0] += (Rows[1][2] * ((Rows[2][0] * Rows[3][1]) - (Rows[2][1] * Rows[3][0])));
		adj.Rows[3][0] *= -1.0f;

		//d21
		adj.Rows[0][1] = (Rows[0][1] * ((Rows[2][2] * Rows[3][3]) - (Rows[2][3] * Rows[3][2])));
		adj.Rows[0][1] -= (Rows[0][2] * ((Rows[2][1] * Rows[3][3]) - (Rows[2][3] * Rows[3][1])));
		adj.Rows[0][1] += (Rows[0][3] * ((Rows[2][1] * Rows[3][2]) - (Rows[2][2] * Rows[3][1])));
		adj.Rows[0][1] *= -1.0f;
		//d22
		adj.Rows[1][1] = (Rows[0][0] * ((Rows[2][2] * Rows[3][3]) - (Rows[2][3] * Rows[3][2])));
		adj.Rows[1][1] -= (Rows[0][2] * ((Rows[2][0] * Rows[3][3]) - (Rows[2][3] * Rows[3][0])));
		adj.Rows[1][1] += (Rows[0][3] * ((Rows[2][0] * Rows[3][2]) - (Rows[2][2] * Rows[3][0])));
		//d23
		adj.Rows[2][1] = (Rows[0][0] * ((Rows[2][1] * Rows[3][3]) - (Rows[2][3] * Rows[3][1])));
		adj.Rows[2][1] -= (Rows[0][1] * ((Rows[2][0] * Rows[3][3]) - (Rows[2][3] * Rows[3][0])));
		adj.Rows[2][1] += (Rows[0][3] * ((Rows[2][0] * Rows[3][1]) - (Rows[2][1] * Rows[3][0])));
		adj.Rows[2][1] *= -1.0f;
		//d24
		adj.Rows[3][1] = (Rows[0][0] * ((Rows[2][1] * Rows[3][2]) - (Rows[2][2] * Rows[3][1])));
		adj.Rows[3][1] -= (Rows[0][1] * ((Rows[2][0] * Rows[3][2]) - (Rows[2][2] * Rows[3][0])));
		adj.Rows[3][1] += (Rows[0][2] * ((Rows[2][0] * Rows[3][1]) - (Rows[2][1] * Rows[3][0])));

		//d31
		adj.Rows[0][2] = (Rows[0][1] * ((Rows[1][2] * Rows[3][3]) - (Rows[1][3] * Rows[3][2])));
		adj.Rows[0][2] -= (Rows[0][2] * ((Rows[1][1] * Rows[3][3]) - (Rows[1][3] * Rows[3][1])));
		adj.Rows[0][2] += (Rows[0][3] * ((Rows[1][1] * Rows[3][2]) - (Rows[1][2] * Rows[3][1])));
		//d32
		adj.Rows[1][2] = (Rows[0][0] * ((Rows[1][2] * Rows[3][3]) - (Rows[1][3] * Rows[3][2])));
		adj.Rows[1][2] -= (Rows[0][2] * ((Rows[1][0] * Rows[3][3]) - (Rows[1][3] * Rows[3][0])));
		adj.Rows[1][2] += (Rows[0][3] * ((Rows[1][0] * Rows[3][2]) - (Rows[1][2] * Rows[3][0])));
		adj.Rows[1][2] *= -1.0f;
		//d33
		adj.Rows[2][2] = (Rows[0][0] * ((Rows[1][1] * Rows[3][3]) - (Rows[1][3] * Rows[3][1])));
		adj.Rows[2][2] -= (Rows[0][1] * ((Rows[1][0] * Rows[3][3]) - (Rows[1][3] * Rows[3][0])));
		adj.Rows[2][2] += (Rows[0][3] * ((Rows[1][0] * Rows[3][1]) - (Rows[1][1] * Rows[3][0])));
		//d34
		adj.Rows[3][2] = (Rows[0][0] * ((Rows[1][1] * Rows[3][2]) - (Rows[1][2] * Rows[3][1])));
		adj.Rows[3][2] -= (Rows[0][1] * ((Rows[1][0] * Rows[3][2]) - (Rows[1][2] * Rows[3][0])));
		adj.Rows[3][2] += (Rows[0][2] * ((Rows[1][0] * Rows[3][1]) - (Rows[1][1] * Rows[3][0])));
		adj.Rows[3][2] *= -1.0f;

		//d41
		adj.Rows[0][3] = (Rows[0][1] * ((Rows[1][2] * Rows[2][3]) - (Rows[1][3] * Rows[2][2])));
		adj.Rows[0][3] -= (Rows[0][2] * ((Rows[1][1] * Rows[2][3]) - (Rows[1][3] * Rows[2][1])));
		adj.Rows[0][3] += (Rows[0][3] * ((Rows[1][1] * Rows[2][2]) - (Rows[1][2] * Rows[2][1])));
		adj.Rows[0][3] *= -1.0f;
		//d42
		adj.Rows[1][3] = (Rows[0][0] * ((Rows[1][2] * Rows[2][3]) - (Rows[1][3] * Rows[2][2])));
		adj.Rows[1][3] -= (Rows[0][2] * ((Rows[1][0] * Rows[2][3]) - (Rows[1][3] * Rows[2][0])));
		adj.Rows[1][3] += (Rows[0][3] * ((Rows[1][0] * Rows[2][2]) - (Rows[1][2] * Rows[2][0])));
		//d43
		adj.Rows[2][3] = (Rows[0][0] * ((Rows[1][1] * Rows[2][3]) - (Rows[1][3] * Rows[2][1])));
		adj.Rows[2][3] -= (Rows[0][1] * ((Rows[1][0] * Rows[2][3]) - (Rows[1][3] * Rows[2][0])));
		adj.Rows[2][3] += (Rows[0][3] * ((Rows[1][0] * Rows[2][1]) - (Rows[1][1] * Rows[2][0])));
		adj.Rows[2][3] *= -1.0f;
		//d44
		adj.Rows[3][3] = (Rows[0][0] * ((Rows[1][1] * Rows[2][2]) - (Rows[1][2] * Rows[2][1])));
		adj.Rows[3][3] -= (Rows[0][1] * ((Rows[1][0] * Rows[2][2]) - (Rows[1][2] * Rows[2][0])));
		adj.Rows[3][3] += (Rows[0][2] * ((Rows[1][0] * Rows[2][1]) - (Rows[1][1] * Rows[2][0])));
		return adj;
#endif
	}

	inline float Matrix4::Determinant() const
	{
#if defined(SSE_INTRIN)
		__m128 ab = _mm_mul_ps(_mm_movelh_ps(Xmm[0], Xmm[1]), _mm_movehl_ps(Xmm[1], Xmm[0]));
		__m128 dc = _mm_mul_ps(_mm_movelh_ps(Xmm[2], Xmm[3]), _mm_movehl_ps(Xmm[3], Xmm[2]));

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
		det = Rows[0][0] * (
			(Rows[1][1] * ((Rows[2][2] * Rows[3][3]) - (Rows[2][3] * Rows[3][2]))) -
			(Rows[1][2] * ((Rows[2][1] * Rows[3][3]) - (Rows[2][3] * Rows[3][1]))) +
			(Rows[1][3] * ((Rows[2][1] * Rows[3][2]) - (Rows[2][2] * Rows[3][1]))));
		//d12
		det -= Rows[0][1] * (
			(Rows[1][0] * ((Rows[2][2] * Rows[3][3]) - (Rows[2][3] * Rows[3][2]))) -
			(Rows[1][2] * ((Rows[2][0] * Rows[3][3]) - (Rows[2][3] * Rows[3][0]))) +
			(Rows[1][3] * ((Rows[2][0] * Rows[3][2]) - (Rows[2][2] * Rows[3][0]))));
		//d13
		det += Rows[0][2] * (
			(Rows[1][0] * ((Rows[2][1] * Rows[3][3]) - (Rows[2][3] * Rows[3][1]))) -
			(Rows[1][1] * ((Rows[2][0] * Rows[3][3]) - (Rows[2][3] * Rows[3][0]))) +
			(Rows[1][3] * ((Rows[2][0] * Rows[3][1]) - (Rows[2][1] * Rows[3][0]))));
		//d14
		det -= Rows[0][3] * (
			(Rows[1][0] * ((Rows[2][1] * Rows[3][2]) - (Rows[2][2] * Rows[3][1]))) -
			(Rows[1][1] * ((Rows[2][0] * Rows[3][2]) - (Rows[2][2] * Rows[3][0]))) +
			(Rows[1][2] * ((Rows[2][0] * Rows[3][1]) - (Rows[2][1] * Rows[3][0]))));

		return det;
#endif
	}

	inline std::string Matrix4::ToString() const
	{
		return std::string(Rows[0].ToString() + '\n' + Rows[1].ToString() + '\n' + Rows[2].ToString() + '\n' + Rows[3].ToString());
	}

	inline bool Matrix4::operator==(const Matrix4& other) const
	{
		return Equals(other);
	}

	inline bool Matrix4::operator!=(const Matrix4& other) const
	{
		return !Equals(other);
	}

	inline float Matrix4::operator()(uint32 r, uint32 c) const
	{
		return GetElement(r, c);
	}

	inline float& Matrix4::operator()(uint32 r, uint32 c)
	{
		return GetElement(r, c);
	}

	inline Vector4 Matrix4::operator()(uint32 r) const
	{
		return GetRow(r);
	}

	inline Vector4& Matrix4::operator()(uint32 r)
	{
		return GetRow(r);
	}

	inline Vector4 operator*(const Vector4& left, const Matrix4& right)
	{
		return right.Multiply(left);
	}

	inline Vector4& operator*=(Vector4& left, const Matrix4& right)
	{
		return left = right.Multiply(left);
	}

	inline Matrix4 operator+(Matrix4 left, const Matrix4& right)
	{
		return left.Add(right);
	}

	inline Matrix4 operator-(Matrix4 left, const Matrix4& right)
	{
		return left.Subtract(right);
	}

	inline Matrix4 operator*(Matrix4 left, const Matrix4& right)
	{
		return left.Multiply(right);
	}

	inline Matrix4 operator+(Matrix4 left, float right)
	{
		return left.Add(right);
	}

	inline Matrix4 operator-(Matrix4 left, float right)
	{
		return left.Subtract(right);
	}

	inline Matrix4 operator*(Matrix4 left, float right)
	{
		return left.Multiply(right);
	}

	inline Matrix4 operator/(Matrix4 left, float right)
	{
		return left.Divide(right);
	}

	inline Matrix4 operator*(float left, Matrix4 right)
	{
		return right.Multiply(left);
	}

	inline Matrix4& Matrix4::operator=(const Matrix4& other)
	{
		if (this != &other)
			memcpy(m, other.m, sizeof(float) * 16);

		return *this;
	}

	inline Matrix4& Matrix4::operator+=(const Matrix4& right)
	{
		return Add(right);
	}

	inline Matrix4& Matrix4::operator-=(const Matrix4& right)
	{
		return Subtract(right);
	}

	inline Matrix4& Matrix4::operator*=(const Matrix4& right)
	{
		return Multiply(right);
	}

	inline Matrix4& Matrix4::operator+=(float right)
	{
		return Add(right);
	}

	inline Matrix4& Matrix4::operator-=(float right)
	{
		return Subtract(right);
	}

	inline Matrix4& Matrix4::operator*=(float right)
	{
		return Multiply(right);
	}

	inline Matrix4& Matrix4::operator/=(float right)
	{
		return Divide(right);
	}

	inline Matrix4 Matrix4::Nan()
	{
		return Matrix4(Vector4::Nan(), Vector4::Nan(), Vector4::Nan(), Vector4::Nan());
	}

	inline Matrix4 Matrix4::Identity()
	{
		return Matrix4(1.0f);
	}

	inline Matrix4 Matrix4::Perspective(float fovRad, float aspectWiHe, float farZ, float nearZ)
	{
		if (fovRad < (PI / 180.0) || fovRad >(PI - (PI / 180.0)))
			return Matrix4::Nan();

		Matrix4 m;

		float yScale = 1.0f / tan(fovRad * 0.5f);
		float xScale = yScale / aspectWiHe;

		m.Rows[0][0] = xScale;
		m.Rows[1][1] = yScale;
		m.Rows[2][2] = farZ / (farZ - nearZ);
		m.Rows[3][2] = (-nearZ * farZ) / (farZ - nearZ);
		m.Rows[2][3] = 1.0f;

		return m;
	}

	inline Matrix4 Matrix4::Orthographic(float width, float height, float farZ, float nearZ)
	{
		Matrix4 m(1.0f);

		m.Rows[0][0] = 2.0f / width;
		m.Rows[1][1] = 2.0f / height;
		m.Rows[2][2] = 1.0f / (farZ - nearZ);
		m.Rows[3][2] = -nearZ / (farZ - nearZ);
		m.Rows[3][3] = 1.0f;

		return m;
	}

	inline Matrix4 Matrix4::Orthographic(float left, float right, float top, float bottom, float farZ, float nearZ)
	{
		Matrix4 m(1.0f);

		m.Rows[0][0] = 2.0f / (right - left);
		m.Rows[3][0] = (left + right) / (left - right);
		m.Rows[1][1] = 2.0f / (top - bottom);
		m.Rows[3][1] = (top + bottom) / (bottom - top);
		m.Rows[2][2] = 1.0f / (farZ - nearZ);
		m.Rows[3][2] = nearZ / (nearZ - farZ);
		m.Rows[3][3] = 1.0f;

		return m;
	}

	inline Matrix4 Matrix4::Orthographic(float aspectWiHe, float farZ, float nearZ)
	{
		Matrix4 m(1.0f);

		m.Rows[0][0] = 2.0f / aspectWiHe;
		m.Rows[1][1] = 2.0f;
		m.Rows[2][2] = 1.0f / (farZ - nearZ);
		m.Rows[3][2] = -nearZ / (farZ - nearZ);
		m.Rows[3][3] = 1.0f;

		return m;
	}

	inline Matrix4 Matrix4::LookAt(const Vector3& up, const Vector3& at, const Vector3& position)
	{
		Matrix4 m(1.0f);
		Vector3 z((at - position).Normalize());
		Vector3 x((up.UnitVector().Cross(z)).Normalize());
		Vector3 y(z.Cross(x));

		m.Rows[0][0] = x.x;
		m.Rows[1][0] = x.y;
		m.Rows[2][0] = x.z;
		m.Rows[3][0] = -x.Dot(position);

		m.Rows[0][1] = y.x;
		m.Rows[1][1] = y.y;
		m.Rows[2][1] = y.z;
		m.Rows[3][1] = -y.Dot(position);

		m.Rows[0][2] = z.x;
		m.Rows[1][2] = z.y;
		m.Rows[2][2] = z.z;
		m.Rows[3][2] = -z.Dot(position);

		m.Rows[3][3] = 1.0f;

		return m;
	}

	inline Matrix4 Matrix4::Translation(const Vector3& translation)
	{
		Matrix4 m(1.0f);

		m.Rows[3][0] = translation.x;
		m.Rows[3][1] = translation.y;
		m.Rows[3][2] = translation.z;

		return m;
	}

	inline Matrix4 Matrix4::Rotation(const Vector3& axis, float angleRad)
	{
		Matrix4 r(1.0f);
		Vector3 unitAxis(axis.UnitVector());

		float cosine = cosf(angleRad);
		float sine = sinf(angleRad);
		float c = 1 - cosine;

		r.Rows[0][0] = (unitAxis.x * unitAxis.x * c) + cosine;
		r.Rows[0][1] = (unitAxis.x * unitAxis.y * c) - (sine * unitAxis.z);
		r.Rows[0][2] = (unitAxis.x * unitAxis.z * c) + (sine * unitAxis.y);

		r.Rows[1][0] = (unitAxis.y * unitAxis.x * c) + (sine * unitAxis.z);
		r.Rows[1][1] = (unitAxis.y * unitAxis.y * c) + cosine;
		r.Rows[1][2] = (unitAxis.y * unitAxis.z * c) - (sine * unitAxis.x);

		r.Rows[2][0] = (unitAxis.z * unitAxis.x * c) - (sine * unitAxis.y);
		r.Rows[2][1] = (unitAxis.z * unitAxis.y * c) + (sine * unitAxis.x);
		r.Rows[2][2] = (unitAxis.z * unitAxis.z * c) + cosine;

		r.Rows[3][3] = 1.0f;

		return r;
	}

	inline Matrix4 Matrix4::RotationX(float angleRad)
	{
		Matrix4 r(1.0f);

		float cosine = cosf(angleRad);
		float sine = sinf(angleRad);

		r.Rows[1][1] = cosine;
		r.Rows[1][2] = -sine;
		r.Rows[2][1] = sine;
		r.Rows[2][2] = cosine;

		return r;
	}

	inline Matrix4 Matrix4::RotationY(float angleRad)
	{
		Matrix4 r(1.0f);

		float cosine = cosf(angleRad);
		float sine = sinf(angleRad);

		r.Rows[0][0] = cosine;
		r.Rows[0][2] = sine;
		r.Rows[2][0] = -sine;
		r.Rows[2][2] = cosine;

		return r;
	}

	inline Matrix4 Matrix4::RotationZ(float angleRad)
	{
		Matrix4 r(1.0f);

		float cosine = cosf(angleRad);
		float sine = sinf(angleRad);

		r.Rows[0][0] = cosine;
		r.Rows[0][1] = -sine;
		r.Rows[1][0] = sine;
		r.Rows[1][1] = cosine;

		return r;
	}

	inline Matrix4 Matrix4::RotationYawPitchRoll(float angleRadX, float angleRadY, float angleRadZ)
	{
		Matrix4 rot(1.0f);

		float sinX = sinf(angleRadX);
		float sinY = sinf(angleRadY);
		float sinZ = sinf(angleRadZ);

		float cosX = cosf(angleRadX);
		float cosY = cosf(angleRadY);
		float cosZ = cosf(angleRadZ);

		rot.Rows[0][0] = (cosY * cosZ) - (sinX * sinY * sinZ);
		rot.Rows[0][1] = -(cosX * sinZ);
		rot.Rows[0][2] = (cosZ * sinY) + (sinX * sinY * sinZ);

		rot.Rows[1][0] = (cosZ * sinX * sinY) + (cosY * sinZ);
		rot.Rows[1][1] = cosX * cosZ;
		rot.Rows[1][2] = (sinY * sinZ) - (cosY * cosZ * sinX);

		rot.Rows[2][0] = -(cosX * sinY);
		rot.Rows[2][1] = sinX;
		rot.Rows[2][2] = cosX * cosY;

		return rot;
	}

	inline Matrix4 Matrix4::Scale(const Vector3& scale)
	{
		Matrix4 s(1.0f);

		s.Rows[0][0] = scale.x;
		s.Rows[1][1] = scale.y;
		s.Rows[2][2] = scale.z;

		return s;
	}

	inline Matrix4 Matrix4::Scale(float scale)
	{
		Matrix4 s(scale);
		s.m[15] = 1;

		return s;
	}
}