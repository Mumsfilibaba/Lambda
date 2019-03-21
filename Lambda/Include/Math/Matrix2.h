#pragma once
#include "Vector2.h"

namespace Math
{
	struct MATRIXALIGN Matrix2
	{
	public:
#if defined(SSE_INTRIN)
		Matrix2(__m128 xmm);
#endif
		Matrix2();
		Matrix2(float value);
		Matrix2(const Vector2& r1, const Vector2& r2);
		Matrix2(const Matrix2& other);

		Vector2 Multiply(const Vector2& vector) const;

		Matrix2& Multiply(const Matrix2& other);
		Matrix2& Multiply(float scalar);
		Matrix2& Add(const Matrix2& other);
		Matrix2& Add(float scalar);
		Matrix2& Subtract(const Matrix2& other);
		Matrix2& Subtract(float scalar);
		Matrix2& Divide(float scalar);

		bool Equals(const Matrix2& other) const;

		Matrix2& Transpose();
		Matrix2& Invert();
		Matrix2 TransposeOf() const;
		Matrix2 InverseOf() const;

		Matrix2 Cofactor() const;
		Matrix2 Adjugate() const;

		float Determinant() const;

		float* GetArray();
		const float* GetArray() const;
		float& GetElement(unsigned char r, unsigned char c);
		float GetElement(unsigned char r, unsigned char c) const;
		Vector2& GetRow(unsigned char r);
		Vector2 GetRow(unsigned char r) const;

		std::string ToString() const;
	
		bool operator==(const Matrix2& other) const;
		bool operator!=(const Matrix2& other) const;

		float& operator()(unsigned char r, unsigned char c);
		float operator()(unsigned char r, unsigned char c) const;
		Vector2& operator()(unsigned char r);
		Vector2 operator()(unsigned char r) const;

		Matrix2& operator=(const Matrix2& other);
		Matrix2& operator+=(const Matrix2& right);
		Matrix2& operator-=(const Matrix2& right);
		Matrix2& operator*=(const Matrix2& right);
		Matrix2& operator+=(float right);
		Matrix2& operator-=(float right);
		Matrix2& operator*=(float right);
		Matrix2& operator/=(float right);

		friend Vector2& operator*=(Vector2& left, const Matrix2& right);
		friend Vector2 operator*(Vector2 left, const Matrix2& right);

		friend Matrix2 operator+(Matrix2 left, const Matrix2& right);
		friend Matrix2 operator-(Matrix2 left, const Matrix2& right);
		friend Matrix2 operator*(Matrix2 left, const Matrix2& right);
		friend Matrix2 operator+(Matrix2 left, float right);
		friend Matrix2 operator-(Matrix2 left, float right);
		friend Matrix2 operator*(Matrix2 left, float right);
		friend Matrix2 operator/(Matrix2 left, float right);
		friend Matrix2 operator*(float left, Matrix2 right);
		friend Matrix2 operator/(float left, Matrix2 right);

	private:
		union
		{
			float m[4];
			Vector2 Rows[2];
			SSE_VARIABLE(Xmm);
		};

	public:
		static Matrix2 Nan();
		static Matrix2 Scale(const Vector2& scale);
		static Matrix2 Scale(float scale);
		static Matrix2 Identity();
		static Matrix2 Rotation(float angleRad);
	};

#if defined(SSE_INTRIN)
	inline Matrix2::Matrix2(__m128 xmm)
		: Xmm(xmm)
	{
	}
#endif

	inline Matrix2::Matrix2()
	{
		memset(m, 0, sizeof(float) * 4);
	}

	inline Matrix2::Matrix2(float diagonal)
	{
		memset(m, 0, sizeof(float) * 4);

		Rows[0][0] = diagonal;
		Rows[1][1] = diagonal;
	}

	inline Matrix2::Matrix2(const Vector2& r1, const Vector2& r2)
	{
		Rows[0] = r1;
		Rows[1] = r2;
	}

	inline Matrix2::Matrix2(const Matrix2& other)
	{
		*this = other;
	}

	inline Vector2 Matrix2::Multiply(const Vector2& vector) const
	{
		float t[2];

		t[0] = (vector[0] * Rows[0][0]) + (vector[1] * Rows[1][0]);

		t[1] = (vector[0] * Rows[0][1]) + (vector[1] * Rows[1][1]);

		return Vector2(t[0], t[1]);
	}

	inline Matrix2& Matrix2::Multiply(const Matrix2& other)
	{
#if defined(SSE_INTRIN)
		__m128 m1 = _mm_shuffle_ps(Xmm, Xmm, _MM_SHUFFLE(2, 2, 0, 0));
		m1 = _mm_mul_ps(m1, _mm_shuffle_ps(other.Xmm, other.Xmm, _MM_SHUFFLE(1, 0, 1, 0)));

		__m128 m2 = _mm_shuffle_ps(Xmm, Xmm, _MM_SHUFFLE(3, 3, 1, 1));
		m2 = _mm_mul_ps(m2, _mm_shuffle_ps(other.Xmm, other.Xmm, _MM_SHUFFLE(3, 2, 3, 2)));

		Xmm = _mm_add_ps(m1, m2);
		return *this;
#else
		float t[4];

		t[0] = (Rows[0][0] * other.Rows[0][0]) + (Rows[0][1] * other.Rows[1][0]);
		t[1] = (Rows[0][0] * other.Rows[0][1]) + (Rows[0][1] * other.Rows[1][1]);

		t[2] = (Rows[1][0] * other.Rows[0][0]) + (Rows[1][1] * other.Rows[1][0]);
		t[3] = (Rows[1][0] * other.Rows[0][1]) + (Rows[1][1] * other.Rows[1][1]);

		memcpy(m, t, sizeof(float) * 4);
		return *this;
#endif
	}

	inline Matrix2& Matrix2::Multiply(float scalar)
	{
#if defined(SSE_INTRIN)
		Xmm = _mm_mul_ps(Xmm, _mm_set_ps1(scalar));
		return *this;
#else
		for (int i = 0; i < 4; i++)
			m[i] *= scalar;
		return *this;
#endif
	}

	inline Matrix2& Matrix2::Add(const Matrix2& other)
	{
#if defined(SSE_INTRIN)
		Xmm = _mm_add_ps(Xmm, other.Xmm);
		return *this;
#else
		for (int i = 0; i < 4; i++)
			m[i] += other.m[i];
		return *this;
#endif
	}

	inline Matrix2& Matrix2::Add(float scalar)
	{
#if defined(SSE_INTRIN)
		Xmm = _mm_add_ps(Xmm, _mm_set_ps1(scalar));
#else
		for (int i = 0; i < 4; i++)
			m[i] += scalar;
#endif
		return *this;
	}

	inline Matrix2& Matrix2::Subtract(const Matrix2& other)
	{
#if defined(SSE_INTRIN)
		Xmm = _mm_sub_ps(Xmm, other.Xmm);
		return *this;
#else
		for (int i = 0; i < 4; i++)
			m[i] -= other.m[i];
		return *this;
#endif
	}

	inline Matrix2& Matrix2::Subtract(float scalar)
	{
#if defined(SSE_INTRIN)
		Xmm = _mm_sub_ps(Xmm, _mm_set_ps1(scalar));
		return *this;
#else
		for (int i = 0; i < 4; i++)
			m[i] -= scalar;
		return *this;
#endif
	}

	inline Matrix2& Matrix2::Divide(float scalar)
	{
#if defined(SSE_INTRIN)
		Xmm = _mm_div_ps(Xmm, _mm_set_ps1(scalar));
		return *this;
#else
		for (int i = 0; i < 4; i++)
			m[i] /= scalar;
		return *this;
#endif
	}

	inline bool Matrix2::Equals(const Matrix2& other) const
	{
		for (int i = 0; i < 4; i++)
		{
			if (m[i] != other.m[i])
				return false;
		}

		return true;
	}

	inline Matrix2& Matrix2::Transpose()
	{
#if defined(SSE_INTRIN)
		Xmm = _mm_shuffle_ps(Xmm, Xmm, _MM_SHUFFLE(3, 1, 2, 0));
		return *this;
#else
		float temp[4];

		temp[0] = Rows[0][0];
		temp[1] = Rows[1][0];

		temp[2] = Rows[0][1];
		temp[3] = Rows[1][1];

		memcpy(m, temp, sizeof(float) * 4);
		return *this;
#endif
	}

	inline Matrix2 Matrix2::TransposeOf() const
	{
		return Matrix2(*this).Transpose();
	}

	inline Matrix2& Matrix2::Invert()
	{
#if defined(SSE_INTRIN)
		__m128 det = _mm_set_ps1((Rows[0][0] * Rows[1][1]) - (Rows[0][1] * Rows[1][0]));
		det = _mm_div_ps(_mm_set_ps(1.0f, -1.0f, -1.0f, 1.0f), det);

		Xmm = _mm_shuffle_ps(Xmm, Xmm, _MM_SHUFFLE(0, 2, 1, 3));
		Xmm = _mm_mul_ps(Xmm, det);
		return *this;
#else
		float det = Determinant();

		if (det == 0)
			*this = Matrix2::Nan();
		else
			*this = Adjugate().Divide(det);
		return *this;
#endif
	}

	inline Matrix2 Matrix2::InverseOf() const
	{
		return Matrix2(*this).Invert();
	}

	inline Matrix2 Matrix2::Cofactor() const
	{
		return Adjugate().Transpose();
	}

	inline Matrix2 Matrix2::Adjugate() const
	{
#if defined(SSE_INTRIN)
		Matrix2 a;
		a.Xmm = _mm_shuffle_ps(Xmm, Xmm, _MM_SHUFFLE(0, 2, 1, 3));
		a.Xmm = _mm_mul_ps(a.Xmm, _mm_set_ps(1.0f, -1.0f, -1.0f, 1.0f));
		return *this;
#else
		a.Rows[0][0] = Rows[1][1];
		a.Rows[1][0] = -Rows[1][0];
		a.Rows[0][1] = -Rows[0][1];
		a.Rows[1][1] = Rows[0][0];
		return a;
#endif
	}

	inline float Matrix2::Determinant() const
	{
		return (Rows[0][0] * Rows[1][1]) - (Rows[0][1] * Rows[1][0]);
	}

	inline float* Matrix2::GetArray()
	{
		return m;
	}

	inline const float* Matrix2::GetArray() const
	{
		return m;
	}

	inline float& Matrix2::GetElement(unsigned char r, unsigned char c)
	{
		assert(r < 2);
		assert(c < 2);
		return Rows[r][c];
	}

	inline float Matrix2::GetElement(unsigned char r, unsigned char c) const
	{
		assert(r < 2);
		assert(c < 2);
		return Rows[r][c];
	}

	inline Vector2& Matrix2::GetRow(unsigned char r)
	{
		assert(r < 2);
		return Rows[r];
	}

	inline Vector2 Matrix2::GetRow(unsigned char r) const
	{
		assert(r < 2);
		return Rows[r];
	}

	inline std::string Matrix2::ToString() const
	{
		return std::string(Rows[0].ToString() + '\n' + Rows[1].ToString());
	}

	inline bool Matrix2::operator==(const Matrix2& other) const
	{
		return Equals(other);
	}

	inline bool Matrix2::operator!=(const Matrix2& other) const
	{
		return !Equals(other);
	}

	inline float Matrix2::operator()(unsigned char r, unsigned char c) const
	{
		return GetElement(r, c);
	}

	inline float& Matrix2::operator()(unsigned char r, unsigned char c)
	{
		return GetElement(r, c);
	}

	inline Vector2 Matrix2::operator()(unsigned char r) const
	{
		return GetRow(r);
	}

	inline Vector2& Matrix2::operator()(unsigned char r)
	{
		return GetRow(r);
	}

	inline Vector2 operator*(Vector2 left, const Matrix2& right)
	{
		return right.Multiply(left);
	}

	inline Vector2& operator*=(Vector2& left, const Matrix2& right)
	{
		return left = right.Multiply(left);
	}

	inline Matrix2 operator+(Matrix2 left, const Matrix2& right)
	{
		return left.Add(right);
	}

	inline Matrix2 operator-(Matrix2 left, const Matrix2& right)
	{
		return left.Subtract(right);
	}

	inline Matrix2 operator*(Matrix2 left, const Matrix2& right)
	{
		return left.Multiply(right);
	}

	inline Matrix2 operator+(Matrix2 left, float right)
	{
		return left.Add(right);
	}

	inline Matrix2 operator-(Matrix2 left, float right)
	{
		return left.Subtract(right);
	}

	inline Matrix2 operator*(Matrix2 left, float right)
	{
		return left.Multiply(right);
	}

	inline Matrix2 operator/(Matrix2 left, float right)
	{
		return left.Divide(right);
	}

	inline Matrix2 operator*(float left, Matrix2 right)
	{
		return right.Multiply(left);
	}

	inline Matrix2 operator/(float left, Matrix2 right)
	{
		return right.Divide(left);
	}

	inline Matrix2& Matrix2::operator=(const Matrix2& other)
	{
		if (this != &other)
			memcpy(m, other.m, sizeof(float) * 4);

		return *this;
	}

	inline Matrix2& Matrix2::operator+=(const Matrix2& right)
	{
		return Add(right);
	}

	inline Matrix2& Matrix2::operator-=(const Matrix2& right)
	{
		return Subtract(right);
	}

	inline Matrix2& Matrix2::operator*=(const Matrix2& right)
	{
		return Multiply(right);
	}

	inline Matrix2& Matrix2::operator+=(float right)
	{
		return Add(right);
	}

	inline Matrix2& Matrix2::operator-=(float right)
	{
		return Subtract(right);
	}

	inline Matrix2& Matrix2::operator*=(float right)
	{
		return Multiply(right);
	}

	inline Matrix2& Matrix2::operator/=(float right)
	{
		return Divide(right);
	}

	inline Matrix2 Matrix2::Nan()
	{
		return Matrix2(Vector2::Nan(), Vector2::Nan());
	}

	inline Matrix2 Matrix2::Identity()
	{
		return Matrix2(1.0f);
	}

	inline Matrix2 Matrix2::Rotation(float angleRad)
	{
		Matrix2 rot(1.0f);

		float cosine = cosf(angleRad);
		float sine = sinf(angleRad);

		rot.m[0] = cosine;
		rot.m[1] = -sine;
		rot.m[2] = sine;
		rot.m[3] = cosine;

		return rot;
	}

	inline Matrix2 Matrix2::Scale(const Vector2& scale)
	{
		Matrix2 s(1.0f);

		s.m[0] = scale.x;
		s.m[3] = scale.y;

		return s;
	}

	inline Matrix2 Matrix2::Scale(float scale)
	{
		return Matrix2(scale);
	}
}