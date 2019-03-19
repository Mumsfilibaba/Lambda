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

#pragma once
#include "Vector2.h"
#include "Vector3.h"

namespace Math
{
	class RAYENGINE_API Matrix3
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

	private:
		union
		{
			float m[9];
			Vector3 rows[3];
		};
	};
}