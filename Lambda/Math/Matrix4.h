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
#include "Vector3.h"
#include "Vector4.h"

namespace Math
{
	class RAYENGINE_API VECTORALIGN Matrix4
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
		float GetElement(unsigned char r, unsigned char c) const;
		float& GetElement(unsigned char r, unsigned char c);
		Vector4 GetRow(unsigned char r) const;
		Vector4& GetRow(unsigned char r);

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

		float operator()(unsigned int r, unsigned int c) const;
		float& operator()(unsigned int r, unsigned int c);
		Vector4 operator()(unsigned int r) const;
		Vector4& operator()(unsigned int r);

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

	public:
		static Matrix4 Nan();
		static Matrix4 Identity();

		static Matrix4 Perspective(float fovRad, float aspectWiHe, float farZ, float nearZ);
		static Matrix4 Orthographic(float aspectWiHe, float farZ, float nearZ);
		static Matrix4 Orthographic(float width, float height, float farZ, float nearZ);
		static Matrix4 Orthographic(float leftX, float rightX, float topY, float bottomY, float farZ, float nearZ);
		static Matrix4 LookAt(const Vector3& up, const Vector3& at, const Vector3& position);

		static Matrix4 Translation(const Vector3& translation);

		static Matrix4 Rotation(const Vector3& axis, float angleRad);
		static Matrix4 Rotation(float angleRadX, float angleRadY, float angleRadZ);
		static Matrix4 RotationX(float angleRad);
		static Matrix4 RotationY(float angleRad);
		static Matrix4 RotationZ(float angleRad);

		static Matrix4 Scale(const Vector3& scale);
		static Matrix4 Scale(float scale);

	private:
		union
		{
			float m[16];
			Vector4 rows[4];
#if defined(SSE_INTRIN)
			__m128 sse128[4];
#endif
		};
	};
}