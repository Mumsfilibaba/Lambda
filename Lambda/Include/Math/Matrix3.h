#pragma once
#include "Vector2.h"
#include "Vector3.h"

namespace Math
{
	struct Matrix3
	{
	public:
		Matrix3();
		Matrix3(float value);
		Matrix3(const Vector3& r1, const Vector3& r2, const Vector3& r3);
		Matrix3(const Matrix3& other);

		Vector3 Multiply(const Vector3& vector) const;

		Matrix3& Multiply(const Matrix3& other);
		Matrix3& Multiply(float scalar);
		Matrix3& Add(const Matrix3& other);
		Matrix3& Add(float scalar);
		Matrix3& Subtract(const Matrix3& other);
		Matrix3& Subtract(float scalar);
		Matrix3& Divide(float scalar);

		bool Equals(const Matrix3& other) const;

		float* GetArray();
		const float* GetArray() const;
		float GetElement(unsigned char r, unsigned char c) const;
		float& GetElement(unsigned char r, unsigned char c);
		Vector3 GetRow(unsigned char r) const;
		Vector3& GetRow(unsigned char r);

		Matrix3& Transpose();
		Matrix3& Invert();
		Matrix3 TransposeOf() const;
		Matrix3 InverseOf() const;

		Matrix3 Cofactor() const;
		Matrix3 Adjugate() const;

		float Determinant() const;

		std::string ToString() const;

		bool operator==(const Matrix3& other) const;
		bool operator!=(const Matrix3& other) const;

		float operator()(unsigned int r, unsigned int c) const;
		float& operator()(unsigned int r, unsigned int c);
		Vector3 operator()(unsigned int r) const;
		Vector3& operator()(unsigned int r);

		friend Vector3& operator*=(Vector3& left, const Matrix3& right);
		friend Vector3 operator*(Vector3 left, const Matrix3& right);

		Matrix3& operator=(const Matrix3& other);
		Matrix3& operator+=(const Matrix3& right);
		Matrix3& operator-=(const Matrix3& right);
		Matrix3& operator*=(const Matrix3& right);
		Matrix3& operator+=(float right);
		Matrix3& operator-=(float right);
		Matrix3& operator*=(float right);
		Matrix3& operator/=(float right);

		friend Matrix3 operator+(Matrix3 left, const Matrix3& right);
		friend Matrix3 operator-(Matrix3 left, const Matrix3& right);
		friend Matrix3 operator*(Matrix3 left, const Matrix3& right);
		friend Matrix3 operator*(Matrix3 left, float right);
		friend Matrix3 operator*(float left, Matrix3 right);
		friend Matrix3 operator+(Matrix3 left, float right);
		friend Matrix3 operator-(Matrix3 left, float right);
		friend Matrix3 operator/(Matrix3 left, float right);
		friend Matrix3 operator/(float left, Matrix3 right);

	private:
		union
		{
			float m[9];
			Vector3 rows[3];
		};

	public:
		static Matrix3 Nan();
		static Matrix3 Identity();

		static Matrix3 Translation(const Vector2& translation);

		static Matrix3 Rotation(const Vector3& axis, float angleRad);
		static Matrix3 Rotation(float angleRadZ, float angleRadX, float angleRadY);
		static Matrix3 RotationX(float angleRad);
		static Matrix3 RotationY(float  angleRad);
		static Matrix3 RotationZ(float angleRad);

		static Matrix3 Scale(const Vector3& scale);
		static Matrix3 Scale(float scale);
	};

	inline Matrix3::Matrix3()
	{
		memset(m, 0, sizeof(float) * 9);
	}

	inline Matrix3::Matrix3(float diagonal)
	{
		memset(m, 0, sizeof(float) * 9);

		rows[0][0] = diagonal;
		rows[1][1] = diagonal;
		rows[2][2] = diagonal;
	}

	inline Matrix3::Matrix3(const Vector3& r1, const Vector3& r2, const Vector3& r3)
	{
		rows[0] = r1;
		rows[1] = r2;
		rows[2] = r3;
	}

	inline Matrix3::Matrix3(const Matrix3& other)
	{
		memcpy(m, other.m, sizeof(float) * 9);
	}

	inline Vector3 Matrix3::Multiply(const Vector3& vector) const
	{
		float t[3];

		t[0] = (vector[0] * rows[0][0]) + (vector[1] * rows[1][0]) + (vector[2] * rows[2][0]);

		t[1] = (vector[0] * rows[0][1]) + (vector[1] * rows[1][1]) + (vector[2] * rows[2][1]);

		t[2] = (vector[0] * rows[0][2]) + (vector[1] * rows[1][2]) + (vector[2] * rows[2][2]);

		return Vector3(t[0], t[1], t[2]);
	}

	inline Matrix3& Matrix3::Multiply(const Matrix3& other)
	{
		float t[9];

		t[0] = (rows[0][0] * other.rows[0][0]) + (rows[0][1] * other.rows[1][0]) + (rows[0][2] * other.rows[2][0]);
		t[1] = (rows[0][0] * other.rows[0][1]) + (rows[0][1] * other.rows[1][1]) + (rows[0][2] * other.rows[2][1]);
		t[2] = (rows[0][0] * other.rows[0][2]) + (rows[0][1] * other.rows[1][2]) + (rows[0][2] * other.rows[2][2]);

		t[3] = (rows[1][0] * other.rows[0][0]) + (rows[1][1] * other.rows[1][0]) + (rows[1][2] * other.rows[2][0]);
		t[4] = (rows[1][0] * other.rows[0][1]) + (rows[1][1] * other.rows[1][1]) + (rows[1][2] * other.rows[2][1]);
		t[5] = (rows[1][0] * other.rows[0][2]) + (rows[1][1] * other.rows[1][2]) + (rows[1][2] * other.rows[2][2]);

		t[6] = (rows[2][0] * other.rows[0][0]) + (rows[2][1] * other.rows[1][0]) + (rows[2][2] * other.rows[2][0]);
		t[7] = (rows[2][0] * other.rows[0][1]) + (rows[2][1] * other.rows[1][1]) + (rows[2][2] * other.rows[2][1]);
		t[8] = (rows[2][0] * other.rows[0][2]) + (rows[2][1] * other.rows[1][2]) + (rows[2][2] * other.rows[2][2]);


		memcpy(m, t, sizeof(float) * 9);

		return *this;
	}

	inline Matrix3& Matrix3::Multiply(float scalar)
	{
		for (int i = 0; i < 9; i++)
			m[i] *= scalar;

		return *this;
	}

	inline Matrix3& Matrix3::Add(const Matrix3& other)
	{
		for (int i = 0; i < 9; i++)
			m[i] += other.m[i];

		return *this;
	}

	inline Matrix3& Matrix3::Add(float scalar)
	{
		for (int i = 0; i < 9; i++)
			m[i] += scalar;

		return *this;
	}

	inline Matrix3& Matrix3::Subtract(const Matrix3& other)
	{
		for (int i = 0; i < 9; i++)
			m[i] -= other.m[i];

		return *this;
	}

	inline Matrix3& Matrix3::Subtract(float scalar)
	{
		for (int i = 0; i < 9; i++)
			m[i] -= scalar;

		return *this;
	}

	inline Matrix3& Matrix3::Divide(float scalar)
	{
		for (int i = 0; i < 9; i++)
			m[i] /= scalar;

		return *this;
	}

	inline bool Matrix3::Equals(const Matrix3& other) const
	{
		for (int i = 0; i < 9; i++)
		{
			if (m[i] != other.m[i])
				return false;
		}

		return true;
	}

	inline float* Matrix3::GetArray()
	{
		return m;
	}

	inline const float* Matrix3::GetArray() const
	{
		return m;
	}

	inline float Matrix3::GetElement(unsigned char r, unsigned char c) const
	{
		assert(r < 3);
		assert(c < 3);
		return rows[r][c];
	}

	inline float& Matrix3::GetElement(unsigned char r, unsigned char c)
	{
		assert(r < 3);
		assert(c < 3);
		return rows[r][c];
	}

	inline Vector3 Matrix3::GetRow(unsigned char r) const
	{
		assert(r < 3);
		return rows[r];
	}

	inline Vector3& Matrix3::GetRow(unsigned char  r)
	{
		assert(r < 3);

		return rows[r];
	}

	inline Matrix3& Matrix3::Transpose()
	{
		float temp[9];

		temp[0] = rows[0][0];
		temp[1] = rows[1][0];
		temp[2] = rows[2][0];

		temp[3] = rows[0][1];
		temp[4] = rows[1][1];
		temp[5] = rows[2][1];

		temp[6] = rows[0][2];
		temp[7] = rows[1][2];
		temp[8] = rows[2][2];

		memcpy(m, temp, sizeof(float) * 9);

		return *this;
	}

	inline Matrix3 Matrix3::TransposeOf() const
	{
		return Matrix3(*this).Transpose();
	}

	inline Matrix3& Matrix3::Invert()
	{
		float det = Determinant();

		if (det == 0.0f)
			*this = Matrix3::Nan();
		else
			*this = (Adjugate() / det);

		return*this;
	}

	inline Matrix3 Matrix3::InverseOf() const
	{
		return Matrix3(*this).Invert();
	}

	inline Matrix3 Matrix3::Cofactor() const
	{
		return Adjugate().Transpose();
	}

	inline Matrix3 Matrix3::Adjugate() const
	{
		Matrix3 adj;

		//d11 - d12 - d13
		adj.rows[0][0] = ((rows[1][1] * rows[2][2]) - (rows[1][2] * rows[2][1]));
		adj.rows[1][0] = -((rows[1][0] * rows[2][2]) - (rows[1][2] * rows[2][0]));
		adj.rows[2][0] = ((rows[1][0] * rows[2][1]) - (rows[1][1] * rows[2][0]));

		//d21 - d22 - 23
		adj.rows[0][1] = -((rows[0][1] * rows[2][2]) - (rows[0][2] * rows[2][1]));
		adj.rows[1][1] = ((rows[0][0] * rows[2][2]) - (rows[0][2] * rows[2][0]));
		adj.rows[2][1] = -((rows[0][0] * rows[2][1]) - (rows[0][1] * rows[2][0]));

		//d31 - 32 - 33
		adj.rows[0][2] = ((rows[0][1] * rows[1][2]) - (rows[0][2] * rows[1][1]));
		adj.rows[1][2] = -((rows[0][0] * rows[1][2]) - (rows[0][2] * rows[1][0]));
		adj.rows[2][2] = ((rows[0][0] * rows[1][1]) - (rows[0][1] * rows[1][0]));

		return adj;
	}

	inline float Matrix3::Determinant() const
	{
		float det = 0;

		det = (rows[0][0] * ((rows[1][1] * rows[2][2]) - (rows[1][2] * rows[2][1])));
		det -= (rows[0][1] * ((rows[1][0] * rows[2][2]) - (rows[1][2] * rows[2][0])));
		det += (rows[0][2] * ((rows[1][0] * rows[2][1]) - (rows[1][1] * rows[2][0])));

		return det;
	}

	inline std::string Matrix3::ToString() const
	{
		return std::string(rows[0].ToString() + '\n' + rows[1].ToString() + '\n' + rows[2].ToString());
	}

	inline bool Matrix3::operator==(const Matrix3& other) const
	{
		return Equals(other);
	}

	inline bool Matrix3::operator!=(const Matrix3& other) const
	{
		return !Equals(other);
	}

	inline float Matrix3::operator()(unsigned int r, unsigned int c) const
	{
		return GetElement(r, c);
	}

	inline float& Matrix3::operator()(unsigned int r, unsigned int c)
	{
		return GetElement(r, c);
	}

	inline Vector3 Matrix3::operator()(unsigned int r) const
	{
		return GetRow(r);
	}

	inline Vector3& Matrix3::operator()(unsigned int r)
	{
		return GetRow(r);
	}

	inline Vector3& operator*=(Vector3& left, const Matrix3& right)
	{
		return left = right.Multiply(left);
	}

	inline Vector3 operator*(Vector3 left, const Matrix3& right)
	{
		return right.Multiply(left);
	}

	inline Matrix3 operator+(Matrix3 left, const Matrix3& right)
	{
		return left.Add(right);
	}

	inline Matrix3 operator-(Matrix3 left, const Matrix3& right)
	{
		return left.Subtract(right);
	}

	inline Matrix3 operator*(Matrix3 left, const Matrix3& right)
	{
		return left.Multiply(right);
	}

	inline Matrix3 operator+(Matrix3 left, float right)
	{
		return left.Add(right);
	}

	inline Matrix3 operator-(Matrix3 left, float right)
	{
		return left.Subtract(right);
	}

	inline Matrix3 operator*(Matrix3 left, float right)
	{
		return left.Multiply(right);
	}

	inline Matrix3 operator/(Matrix3 left, float right)
	{
		return left.Divide(right);
	}

	inline Matrix3 operator*(float left, Matrix3 right)
	{
		return right.Multiply(left);
	}

	inline Matrix3 operator/(float left, Matrix3 right)
	{
		return right.Divide(left);
	}

	inline Matrix3& Matrix3::operator=(const Matrix3& other)
	{
		if (this != &other)
			memcpy(m, other.m, sizeof(float) * 9);

		return *this;
	}

	inline Matrix3& Matrix3::operator+=(const Matrix3& right)
	{
		return Add(right);
	}

	inline Matrix3& Matrix3::operator-=(const Matrix3& right)
	{
		return Subtract(right);
	}

	inline Matrix3& Matrix3::operator*=(const Matrix3& right)
	{
		return Multiply(right);
	}

	inline Matrix3& Matrix3::operator+=(float right)
	{
		return Add(right);
	}

	inline Matrix3& Matrix3::operator-=(float right)
	{
		return Subtract(right);
	}

	inline Matrix3& Matrix3::operator*=(float right)
	{
		return Multiply(right);
	}

	inline Matrix3& Matrix3::operator/=(float right)
	{
		return Divide(right);
	}

	inline Matrix3 Matrix3::Nan()
	{
		return Matrix3(Vector3::Nan(), Vector3::Nan(), Vector3::Nan());
	}

	inline Matrix3 Matrix3::Identity()
	{
		return Matrix3(1.0f);
	}

	inline Matrix3 Matrix3::Translation(const Vector2& translation)
	{
		Matrix3 t(1.0f);

		t.rows[2][0] = translation.x;
		t.rows[2][1] = translation.y;

		return t;
	}

	inline Matrix3 Matrix3::Rotation(const Vector3& axis, float angleRad)
	{
		Matrix3 r(1.0f);
		Vector3 unitAxis(axis.UnitVector());

		float cosine = cosf(angleRad);
		float sine = sinf(angleRad);
		float c = 1 - cosine;

		r.rows[0][0] = (unitAxis.x * unitAxis.x * c) + cosine;
		r.rows[0][1] = (unitAxis.x * unitAxis.y * c) - (sine * unitAxis.z);
		r.rows[0][2] = (unitAxis.x * unitAxis.z * c) + (sine * unitAxis.y);

		r.rows[1][0] = (unitAxis.y * unitAxis.x * c) + (sine * unitAxis.z);
		r.rows[1][1] = (unitAxis.y * unitAxis.y * c) + cosine;
		r.rows[1][2] = (unitAxis.y * unitAxis.z * c) - (sine * unitAxis.x);

		r.rows[2][0] = (unitAxis.z * unitAxis.x * c) - (sine * unitAxis.y);
		r.rows[2][1] = (unitAxis.z * unitAxis.y * c) + (sine * unitAxis.x);
		r.rows[2][2] = (unitAxis.z * unitAxis.z * c) + cosine;

		return r;
	}

	inline Matrix3 Matrix3::RotationX(float angleRad)
	{
		Matrix3 r(1.0f);

		float cosine = cosf(angleRad);
		float sine = sinf(angleRad);

		r.rows[1][1] = cosine;
		r.rows[1][2] = -sine;

		r.rows[2][1] = sine;
		r.rows[2][2] = cosine;

		return r;
	}

	inline Matrix3 Matrix3::RotationY(float angleRad)
	{
		Matrix3 r(1.0f);

		float cosine = cosf(angleRad);
		float sine = sinf(angleRad);

		r.rows[0][0] = cosine;
		r.rows[0][2] = sine;
		r.rows[2][0] = -sine;
		r.rows[2][2] = cosine;

		return r;
	}

	inline Matrix3 Matrix3::RotationZ(float angleRad)
	{
		Matrix3 r(1.0f);

		float cosine = cosf(angleRad);
		float sine = sinf(angleRad);

		r.rows[0][0] = cosine;
		r.rows[0][1] = -sine;
		r.rows[1][0] = sine;
		r.rows[1][1] = cosine;

		return r;
	}

	inline Matrix3 Matrix3::Rotation(float angleRadZ, float angleRadX, float angleRadY)
	{
		Matrix3 rot(1.0f);

		float sinX = sinf(angleRadX);
		float sinY = sinf(angleRadY);
		float sinZ = sinf(angleRadZ);

		float cosX = cosf(angleRadX);
		float cosY = cosf(angleRadY);
		float cosZ = cosf(angleRadZ);

		rot.rows[0][0] = (cosY * cosZ) - (sinX * sinY * sinZ);
		rot.rows[0][1] = -(cosX * sinZ);
		rot.rows[0][2] = (cosZ * sinY) + (sinX * sinY * sinZ);

		rot.rows[1][0] = (cosZ * sinX * sinY) + (cosY * sinZ);
		rot.rows[1][1] = cosX * cosZ;
		rot.rows[1][2] = (sinY * sinZ) - (cosY * cosZ * sinX);

		rot.rows[2][0] = -(cosX * sinY);
		rot.rows[2][1] = sinX;
		rot.rows[2][2] = cosX * cosY;

		return rot;
	}

	inline Matrix3 Matrix3::Scale(const Vector3& scale)
	{
		Matrix3 s(1.0f);

		s.rows[0][0] = scale.x;
		s.rows[1][1] = scale.y;
		s.rows[2][2] = scale.z;

		return s;
	}

	inline Matrix3 Matrix3::Scale(float scale)
	{
		return Matrix3(scale);
	}
}