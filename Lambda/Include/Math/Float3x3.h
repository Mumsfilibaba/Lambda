#pragma once
#include "Float3.h"

namespace Math
{
	///Non SIMD version
	///Should be used when sending data to shaders and generally for storage purposes
	struct Mat3f
	{
	public:
		forceinline bool vectorcall operator==(Mat3f other) const;
		forceinline bool vectorcall operator!=(Mat3f other) const;

		friend forceinline std::string vectorcall ToString(Mat3f f);

	public:
		float m[9];
	};

    
	forceinline bool vectorcall Mat3f::operator==(Mat3f other) const
	{
		for (int i = 0; i < 9; i++)
		{
			if (m[i] != other.m[i])
				return false;
		}
		return true;
	}
    

	forceinline bool vectorcall Mat3f::operator!=(Mat3f other) const
	{
		return !(*this == other);
	}

    
	forceinline std::string vectorcall ToString(Mat3f f)
	{
		return
			'[' + std::to_string(f.m[0]) + ", " + std::to_string(f.m[1]) + ", " + std::to_string(f.m[2]) + ']' +
			'[' + std::to_string(f.m[3]) + ", "	+ std::to_string(f.m[4]) + ", " + std::to_string(f.m[5]) + ']' +
			'[' + std::to_string(f.m[6]) + ", " + std::to_string(f.m[7]) + ", " + std::to_string(f.m[8]) + ']';
	}


	///Should be SIMD version
	struct Float3x3
	{
	public:
		forceinline explicit Float3x3(float diagonal = 1.0f);
		forceinline explicit Float3x3(Float3 r0, Float3 r1, Float3 r2);

		forceinline Float3		vectorcall Multiply(Float3 vector) const;
		forceinline Float3x3&	vectorcall Multiply(const Float3x3& other);
		forceinline Float3x3&	vectorcall Multiply(float scalar);
		forceinline Float3x3&	vectorcall Add(const Float3x3& other);
		forceinline Float3x3&	vectorcall Add(float scalar);
		forceinline Float3x3&	vectorcall Subtract(const Float3x3& other);
		forceinline Float3x3&	vectorcall Subtract(float scalar);
		forceinline Float3x3&	vectorcall Divide(float scalar);

		forceinline bool vectorcall Equals(const Float3x3& other) const;

		forceinline void SetRow(uint32 r, float x, float y, float z);
		forceinline float GetElement(uint32 r, uint32 c) const;
		forceinline Float3 GetRow(uint32 r) const;

		Float3x3 vectorcall GetTranspose() const;
		Float3x3 vectorcall GetInverse() const;
		Float3x3& vectorcall Transpose();
		Float3x3& vectorcall Invert();

		Float3x3 vectorcall Cofactor() const;
		Float3x3 vectorcall Adjugate() const;

		float vectorcall Determinant() const;

		forceinline bool vectorcall operator==(const Float3x3& other) const;
		forceinline bool vectorcall operator!=(const Float3x3& other) const;

		forceinline float vectorcall operator()(uint32 r, uint32 c) const;
		forceinline Float3 vectorcall operator()(uint32 r) const;

		friend forceinline Float3 vectorcall operator*(Float3 left, const Float3x3& right);
		friend forceinline Float3& vectorcall operator*=(Float3& left, const Float3x3& right);

		forceinline Float3x3& vectorcall operator=(const Float3x3& other);
		forceinline Float3x3& vectorcall operator+=(const Float3x3& right);
		forceinline Float3x3& vectorcall operator-=(const Float3x3& right);
		forceinline Float3x3& vectorcall operator*=(const Float3x3& right);
		forceinline Float3x3& vectorcall operator+=(float right);
		forceinline Float3x3& vectorcall operator-=(float right);
		forceinline Float3x3& vectorcall operator*=(float right);
		forceinline Float3x3& vectorcall operator/=(float right);

		friend forceinline std::string	vectorcall ToString(Float3x3 f);
		friend forceinline Mat3f		vectorcall Store(Float3x3 f);
		friend forceinline Float3x3		vectorcall Load(Mat3f storage);
		friend forceinline Float3x3		vectorcall operator+(Float3x3 left, const Float3x3& right);
		friend forceinline Float3x3		vectorcall operator-(Float3x3 left, const Float3x3& right);
		friend forceinline Float3x3		vectorcall operator*(Float3x3 left, const Float3x3& right);
		friend forceinline Float3x3		vectorcall operator*(Float3x3 left, float right);
		friend forceinline Float3x3		vectorcall operator*(float left, Float3x3 right);
		friend forceinline Float3x3		vectorcall operator+(Float3x3 left, float right);
		friend forceinline Float3x3		vectorcall operator-(Float3x3 left, float right);
		friend forceinline Float3x3		vectorcall operator/(Float3x3 left, float right);
		friend forceinline Float3x3		vectorcall operator/(float left, Float3x3 right);

	private:
		float m_Xmm[9];

	public:
		static Float3x3 vectorcall Identity();

		static Float3x3 vectorcall Translation(Vec2f translation);

		static Float3x3 vectorcall RotationRollPitchYaw(float angleRadZ, float angleRadX, float angleRadY);
		static Float3x3 vectorcall Rotation(Vec3f axis, float angleRad);
		static Float3x3 vectorcall RotationX(float angleRad);
		static Float3x3 vectorcall RotationY(float angleRad);
		static Float3x3 vectorcall RotationZ(float angleRad);

		static Float3x3 vectorcall Scale(Vec3f scale);
		static Float3x3 vectorcall Scale(float scale);
	};

    
	forceinline Float3x3::Float3x3(float diagonal)
	{
		memset(m_Xmm, 0, sizeof(m_Xmm));

		m_Xmm[0] = diagonal;
		m_Xmm[4] = diagonal;
		m_Xmm[8] = diagonal;
	}

    
	forceinline Float3x3::Float3x3(Float3 r0, Float3 r1, Float3 r2)
	{
		m_Xmm[0] = r0.GetX();
		m_Xmm[1] = r0.GetY();
		m_Xmm[2] = r0.GetZ();

		m_Xmm[3] = r1.GetX();
		m_Xmm[4] = r1.GetY();
		m_Xmm[5] = r1.GetZ();

		m_Xmm[6] = r2.GetX();
		m_Xmm[7] = r2.GetY();
		m_Xmm[8] = r2.GetZ();
	}
    

	forceinline Float3 vectorcall Float3x3::Multiply(Float3 vector) const
	{
		float t[3];
		t[0] = (vector[0] * m_Xmm[0]) + (vector[1] * m_Xmm[3]) + (vector[2] * m_Xmm[6]);
		t[1] = (vector[0] * m_Xmm[1]) + (vector[1] * m_Xmm[4]) + (vector[2] * m_Xmm[7]);
		t[2] = (vector[0] * m_Xmm[2]) + (vector[1] * m_Xmm[5]) + (vector[2] * m_Xmm[8]);
		return Float3(t[0], t[1], t[2]);
	}

    
	forceinline Float3x3& vectorcall Float3x3::Multiply(const Float3x3& other)
	{
		float t[9];

		t[0] = (m_Xmm[0] * other.m_Xmm[0]) + (m_Xmm[1] * other.m_Xmm[3]) + (m_Xmm[2] * other.m_Xmm[6]);
		t[1] = (m_Xmm[0] * other.m_Xmm[1]) + (m_Xmm[1] * other.m_Xmm[4]) + (m_Xmm[2] * other.m_Xmm[7]);
		t[2] = (m_Xmm[0] * other.m_Xmm[2]) + (m_Xmm[1] * other.m_Xmm[5]) + (m_Xmm[2] * other.m_Xmm[8]);

		t[3] = (m_Xmm[3] * other.m_Xmm[0]) + (m_Xmm[4] * other.m_Xmm[3]) + (m_Xmm[5] * other.m_Xmm[6]);
		t[4] = (m_Xmm[3] * other.m_Xmm[1]) + (m_Xmm[4] * other.m_Xmm[4]) + (m_Xmm[5] * other.m_Xmm[7]);
		t[5] = (m_Xmm[3] * other.m_Xmm[2]) + (m_Xmm[4] * other.m_Xmm[5]) + (m_Xmm[5] * other.m_Xmm[8]);

		t[6] = (m_Xmm[6] * other.m_Xmm[0]) + (m_Xmm[7] * other.m_Xmm[3]) + (m_Xmm[8] * other.m_Xmm[6]);
		t[7] = (m_Xmm[6] * other.m_Xmm[1]) + (m_Xmm[7] * other.m_Xmm[4]) + (m_Xmm[8] * other.m_Xmm[7]);
		t[8] = (m_Xmm[6] * other.m_Xmm[2]) + (m_Xmm[7] * other.m_Xmm[5]) + (m_Xmm[8] * other.m_Xmm[8]);


		memcpy(m_Xmm, t, sizeof(m_Xmm));

		return *this;
	}

    
	forceinline Float3x3& vectorcall Float3x3::Multiply(float scalar)
	{
		for (int i = 0; i < 9; i++)
			m_Xmm[i] *= scalar;

		return *this;
	}

    
	forceinline Float3x3& vectorcall Float3x3::Add(const Float3x3& other)
	{
		for (int i = 0; i < 9; i++)
			m_Xmm[i] += other.m_Xmm[i];

		return *this;
	}

    
	forceinline Float3x3& vectorcall Float3x3::Add(float scalar)
	{
		for (int i = 0; i < 9; i++)
			m_Xmm[i] += scalar;

		return *this;
	}
    

	forceinline Float3x3& vectorcall Float3x3::Subtract(const Float3x3& other)
	{
		for (int i = 0; i < 9; i++)
			m_Xmm[i] -= other.m_Xmm[i];

		return *this;
	}

    
	forceinline Float3x3& vectorcall Float3x3::Subtract(float scalar)
	{
		for (int i = 0; i < 9; i++)
			m_Xmm[i] -= scalar;

		return *this;
	}

    
	forceinline Float3x3& vectorcall Float3x3::Divide(float scalar)
	{
		for (int i = 0; i < 9; i++)
			m_Xmm[i] /= scalar;

		return *this;
	}

    
	forceinline bool vectorcall Float3x3::Equals(const Float3x3& other) const
	{
		for (int i = 0; i < 9; i++)
		{
			if (m_Xmm[i] != other.m_Xmm[i])
				return false;
		}

		return true;
	}
	
    
	forceinline void Float3x3::SetRow(uint32 r, float x, float y, float z)
	{
		assert(r < 3);
		m_Xmm[(r * 3) + 0] = x;
		m_Xmm[(r * 3) + 1] = y;
		m_Xmm[(r * 3) + 2] = z;
	}
    

	forceinline float Float3x3::GetElement(uint32 r, uint32 c) const
	{
		assert(r < 3);
		assert(c < 3);
		return m_Xmm[(3 * r) + c];
	}

    
	forceinline Float3 Float3x3::GetRow(uint32 r) const
	{
		assert(r < 3);
		return Float3(m_Xmm[(3 * r) + 0], m_Xmm[(3 * r) + 1], m_Xmm[(3 * r) + 2]);
	}

    
	inline Float3x3& vectorcall Float3x3::Transpose()
	{
		float temp[9];

		temp[0] = m_Xmm[0];
		temp[1] = m_Xmm[3];
		temp[2] = m_Xmm[6];

		temp[3] = m_Xmm[1];
		temp[4] = m_Xmm[4];
		temp[5] = m_Xmm[7];

		temp[6] = m_Xmm[2];
		temp[7] = m_Xmm[5];
		temp[8] = m_Xmm[8];

		memcpy(m_Xmm, temp, sizeof(m_Xmm));

		return *this;
	}
    

	inline Float3x3 vectorcall Float3x3::GetTranspose() const
	{
		Float3x3 transpose = *this;
		transpose.Transpose();
		return transpose;
	}
    

	inline Float3x3& vectorcall Float3x3::Invert()
	{
		float det = Determinant();

		if (det == 0.0f)
			*this = Float3x3(0.0f);
		else
			*this = (Adjugate() / det);

		return*this;
	}
    

	inline Float3x3 vectorcall Float3x3::GetInverse() const
	{
		Float3x3 inverse = *this;
		inverse.Invert();
		return inverse;
	}

    
	inline Float3x3 vectorcall Float3x3::Cofactor() const
	{
		return Adjugate().Transpose();
	}
    

	inline Float3x3 vectorcall Float3x3::Adjugate() const
	{
		Float3x3 adjugate;

		//d11 - d12 - d13
		adjugate.m_Xmm[0] = ((m_Xmm[4] * m_Xmm[8]) - (m_Xmm[5] * m_Xmm[7]));
		adjugate.m_Xmm[3] = -((m_Xmm[3] * m_Xmm[8]) - (m_Xmm[5] * m_Xmm[6]));
		adjugate.m_Xmm[6] = ((m_Xmm[3] * m_Xmm[7]) - (m_Xmm[4] * m_Xmm[6]));

		//d21 - d22 - 23
		adjugate.m_Xmm[1] = -((m_Xmm[1] * m_Xmm[8]) - (m_Xmm[2] * m_Xmm[7]));
		adjugate.m_Xmm[4] = ((m_Xmm[0] * m_Xmm[8]) - (m_Xmm[2] * m_Xmm[6]));
		adjugate.m_Xmm[7] = -((m_Xmm[0] * m_Xmm[7]) - (m_Xmm[1] * m_Xmm[6]));

		//d31 - 32 - 33
		adjugate.m_Xmm[2] = ((m_Xmm[1] * m_Xmm[5]) - (m_Xmm[2] * m_Xmm[4]));
		adjugate.m_Xmm[5] = -((m_Xmm[0] * m_Xmm[5]) - (m_Xmm[2] * m_Xmm[3]));
		adjugate.m_Xmm[8] = ((m_Xmm[0] * m_Xmm[4]) - (m_Xmm[1] * m_Xmm[3]));

		return adjugate;
	}

    
	inline float vectorcall Float3x3::Determinant() const
	{
		float det = 0;

		det = (m_Xmm[0] * ((m_Xmm[4] * m_Xmm[8]) - (m_Xmm[5] * m_Xmm[7])));
		det -= (m_Xmm[1] * ((m_Xmm[3] * m_Xmm[8]) - (m_Xmm[5] * m_Xmm[6])));
		det += (m_Xmm[2] * ((m_Xmm[3] * m_Xmm[7]) - (m_Xmm[4] * m_Xmm[6])));

		return det;
	}

    
	forceinline std::string	vectorcall ToString(Float3x3 f)
	{
		return
			'[' + std::to_string(f.m_Xmm[0]) + ", " + std::to_string(f.m_Xmm[1]) + ", " + std::to_string(f.m_Xmm[2]) + "]\n" +
			'[' + std::to_string(f.m_Xmm[3]) + ", " + std::to_string(f.m_Xmm[4]) + ", " + std::to_string(f.m_Xmm[5]) + "]\n" +
			'[' + std::to_string(f.m_Xmm[6]) + ", " + std::to_string(f.m_Xmm[7]) + ", " + std::to_string(f.m_Xmm[8]) + "]\n";
	}
    

	forceinline Mat3f vectorcall Store(Float3x3 f)
	{
		Mat3f result;
		memcpy(result.m, f.m_Xmm, sizeof(f.m_Xmm));
		return result;
	}

    
	forceinline Float3x3 vectorcall Load(Mat3f storage)
	{
		Float3x3 result;
		memcpy(result.m_Xmm, storage.m, sizeof(result.m_Xmm));
		return result;
	}
    

	forceinline bool vectorcall Float3x3::operator==(const Float3x3& other) const
	{
		return Equals(other);
	}

    
	forceinline bool vectorcall Float3x3::operator!=(const Float3x3& other) const
	{
		return !Equals(other);
	}

    
	forceinline float vectorcall Float3x3::operator()(uint32 r, uint32 c) const
	{
		return GetElement(r, c);
	}

    
	forceinline Float3 vectorcall Float3x3::operator()(uint32 r) const
	{
		return GetRow(r);
	}

    
	forceinline Float3& vectorcall operator*=(Float3& left, const Float3x3& right)
	{
		return left = right.Multiply(left);
	}

    
	forceinline Float3 vectorcall operator*(Float3 left, const Float3x3& right)
	{
		return right.Multiply(left);
	}

    
	forceinline Float3x3 vectorcall operator+(Float3x3 left, const Float3x3& right)
	{
		return left.Add(right);
	}

    
	forceinline Float3x3 vectorcall operator-(Float3x3 left, const Float3x3& right)
	{
		return left.Subtract(right);
	}
    

	forceinline Float3x3 vectorcall operator*(Float3x3 left, const Float3x3& right)
	{
		return left.Multiply(right);
	}

    
	forceinline Float3x3 vectorcall operator+(Float3x3 left, float right)
	{
		return left.Add(right);
	}
    

	forceinline Float3x3 vectorcall operator-(Float3x3 left, float right)
	{
		return left.Subtract(right);
	}
    

	forceinline Float3x3 vectorcall operator*(Float3x3 left, float right)
	{
		return left.Multiply(right);
	}
    

	forceinline Float3x3 vectorcall operator/(Float3x3 left, float right)
	{
		return left.Divide(right);
	}
    

	forceinline Float3x3 vectorcall operator*(float left, Float3x3 right)
	{
		return right.Multiply(left);
	}
    

	forceinline Float3x3 vectorcall operator/(float left, Float3x3 right)
	{
		return right.Divide(left);
	}
    

	forceinline Float3x3& vectorcall Float3x3::operator=(const Float3x3& other)
	{
		if (this != &other)
			memcpy(m_Xmm, other.m_Xmm, sizeof(float) * 9);

		return *this;
	}

    
	forceinline Float3x3& vectorcall Float3x3::operator+=(const Float3x3& right)
	{
		return Add(right);
	}
    

	forceinline Float3x3& vectorcall Float3x3::operator-=(const Float3x3& right)
	{
		return Subtract(right);
	}
    

	forceinline Float3x3& vectorcall Float3x3::operator*=(const Float3x3& right)
	{
		return Multiply(right);
	}

    
	forceinline Float3x3& vectorcall Float3x3::operator+=(float right)
	{
		return Add(right);
	}

    
	forceinline Float3x3& vectorcall Float3x3::operator-=(float right)
	{
		return Subtract(right);
	}
    

	forceinline Float3x3& vectorcall Float3x3::operator*=(float right)
	{
		return Multiply(right);
	}
    

	forceinline Float3x3& vectorcall Float3x3::operator/=(float right)
	{
		return Divide(right);
	}

    
	forceinline Float3x3 vectorcall Float3x3::Identity()
	{
		return Float3x3(1.0f);
	}

    
	forceinline Float3x3 vectorcall Float3x3::Translation(Vec2f translation)
	{
		Float3x3 t(1.0f);
		t.m_Xmm[6] = translation.x;
		t.m_Xmm[7] = translation.y;
		return t;
	}

    
	forceinline Float3x3 vectorcall Float3x3::Rotation(Vec3f axis, float angleRad)
	{
		Float3x3 r(1.0f);
		Float3 ax = Load(axis).Normalize();
		Vec3f norm = Store(ax);

		float cosine = cosf(angleRad);
		float sine = sinf(angleRad);
		float c = 1 - cosine;

		r.m_Xmm[0] = (norm.x * norm.x * c) + cosine;
		r.m_Xmm[1] = (norm.y * norm.x * c) + (sine * norm.z);
		r.m_Xmm[2] = (norm.z * norm.x * c) - (sine * norm.y);

		r.m_Xmm[3] = (norm.x * norm.y * c) - (sine * norm.z);
		r.m_Xmm[4] = (norm.y * norm.y * c) + cosine;
		r.m_Xmm[5] = (norm.z * norm.y * c) + (sine * norm.x);

		r.m_Xmm[6] = (norm.x * norm.z * c) + (sine * norm.y);
		r.m_Xmm[7] = (norm.y * norm.z * c) - (sine * norm.x);
		r.m_Xmm[8] = (norm.z * norm.z * c) + cosine;

		return r;
	}

    
	//Row major (Transpose of colummajor)
	forceinline Float3x3 vectorcall Float3x3::RotationX(float angleRad)
	{
		Float3x3 r(1.0f);

		float cosine = cosf(angleRad);
		float sine = sinf(angleRad);

		r.m_Xmm[4] = cosine; 
		r.m_Xmm[5] = sine;

		r.m_Xmm[7] = -sine;
		r.m_Xmm[8] = cosine;

		return r;
	}

    
	//Row major (Transpose of colummajor)
	forceinline Float3x3 vectorcall Float3x3::RotationY(float angleRad)
	{
		Float3x3 r(1.0f);

		float cosine = cosf(angleRad);
		float sine = sinf(angleRad);

		r.m_Xmm[0] = cosine;
		r.m_Xmm[2] = -sine;
		r.m_Xmm[6] = sine;
		r.m_Xmm[8] = cosine;

		return r;
	}
    

	//Row major (Transpose of colummajor)
	forceinline Float3x3 vectorcall Float3x3::RotationZ(float angleRad)
	{
		Float3x3 r(1.0f);

		float cosine = cosf(angleRad);
		float sine = sinf(angleRad);

		r.m_Xmm[0] = cosine;
		r.m_Xmm[1] = sine;
		r.m_Xmm[3] = -sine;
		r.m_Xmm[4] = cosine;

		return r;
	}

    
	forceinline Float3x3 vectorcall Float3x3::RotationRollPitchYaw(float angleRadZ, float angleRadX, float angleRadY)
	{
		Float3x3 rot(1.0f);

		float sinX = sinf(angleRadX);
		float sinY = sinf(angleRadY);
		float sinZ = sinf(angleRadZ);

		float cosX = cosf(angleRadX);
		float cosY = cosf(angleRadY);
		float cosZ = cosf(angleRadZ);

		rot.m_Xmm[0] = (cosY * cosZ) + (sinX * sinY * sinZ);
		rot.m_Xmm[1] = cosX * sinZ;
		rot.m_Xmm[2] = (sinX * cosY * sinZ) - (sinY * cosZ);

		rot.m_Xmm[3] = (sinX * sinY * cosZ) - (cosY * sinZ);
		rot.m_Xmm[4] = cosX * cosZ;
		rot.m_Xmm[5] = (sinX * cosY * cosZ) + (sinY * sinZ);

		rot.m_Xmm[6] = cosX * sinY;
		rot.m_Xmm[7] = -sinX;
		rot.m_Xmm[8] = cosX * cosY;

		return rot;
	}

    
	forceinline Float3x3 vectorcall Float3x3::Scale(Vec3f scale)
	{
		Float3x3 s(1.0f);

		s.m_Xmm[0] = scale.x;
		s.m_Xmm[4] = scale.y;
		s.m_Xmm[8] = scale.z;

		return s;
	}
    

	forceinline Float3x3 vectorcall Float3x3::Scale(float scale)
	{
		return Float3x3(scale);
	}
}
