#pragma once
#include "Float2.h"

namespace Math
{
	///Non SIMD version
	///Should be used when sending data to shaders and generally for storage purposes
	struct VECTORALIGN Mat2f
	{
	public:
		forceinline bool vectorcall operator==(Mat2f other) const;
		forceinline bool vectorcall operator!=(Mat2f other) const;

		friend forceinline std::string vectorcall ToString(Mat2f f);

	public:
		float m[4];
	};

	forceinline bool vectorcall Mat2f::operator==(Mat2f other) const
	{
		for (int i = 0; i < 4; i++)
		{
			if (m[i] != other.m[i])
				return false;
		}
		return true;
	}

	forceinline bool vectorcall Mat2f::operator!=(Mat2f other) const
	{
		return !(*this == other);
	}

	forceinline std::string vectorcall ToString(Mat2f f)
	{
		return
			'[' + std::to_string(f.m[0]) + ", " + std::to_string(f.m[1]) + ']' + 
			'[' + std::to_string(f.m[2]) + ", " + std::to_string(f.m[3]) + ']';
	}


	///SIMD version
	struct MATRIXALIGN Float2x2
	{
	public:
		forceinline explicit Float2x2(float diagonal = 1.0f);
		forceinline explicit Float2x2(Float2 r0, Float2 r1);
		forceinline explicit Float2x2(SSE_VARIABLE(xmm));

		forceinline Float2		vectorcall Multiply(Float2 vector) const;
		forceinline Float2x2&	vectorcall Multiply(Float2x2 other);
		forceinline Float2x2&	vectorcall Multiply(float scalar);
		forceinline Float2x2&	vectorcall Add(Float2x2 other);
		forceinline Float2x2&	vectorcall Add(float scalar);
		forceinline Float2x2&	vectorcall Subtract(Float2x2 other);
		forceinline Float2x2&	vectorcall Subtract(float scalar);
		forceinline Float2x2&	vectorcall Divide(float scalar);

		forceinline bool vectorcall Equals(Float2x2 other) const;

		Float2x2 vectorcall GetTranspose() const;
		Float2x2 vectorcall GetInverse() const;
		Float2x2& vectorcall Transpose();
		Float2x2& vectorcall Invert();

		Float2x2 vectorcall Cofactor() const;
		Float2x2 vectorcall Adjugate() const;

		float vectorcall Determinant() const;

		forceinline void SetRow(uint32 r, float x, float y);
		forceinline float GetElement(uint32 r, uint32 c) const;
		forceinline Float2 GetRow(uint32 r) const;

		forceinline bool vectorcall operator==(Float2x2 other) const;
		forceinline bool vectorcall operator!=(Float2x2 other) const;

		forceinline float vectorcall operator()(uint32 r, uint32 c) const;
		forceinline Float2 vectorcall operator()(uint32 r) const;

		forceinline Float2x2& vectorcall operator=(Float2x2 other);
		forceinline Float2x2& vectorcall operator+=(Float2x2 right);
		forceinline Float2x2& vectorcall operator-=(Float2x2 right);
		forceinline Float2x2& vectorcall operator*=(Float2x2 right);
		forceinline Float2x2& vectorcall operator+=(float right);
		forceinline Float2x2& vectorcall operator-=(float right);
		forceinline Float2x2& vectorcall operator*=(float right);
		forceinline Float2x2& vectorcall operator/=(float right);

		friend forceinline std::string	vectorcall ToString(Float2x2 f);
		friend forceinline Mat2f		vectorcall Store(Float2x2 f);
		friend forceinline Float2x2		vectorcall Load(Mat2f storage);
		friend forceinline Float2		vectorcall operator*(Float2 left, Float2x2 right);
		friend forceinline Float2&		vectorcall operator*=(Float2& left, Float2x2 right);
		friend forceinline Float2x2		vectorcall operator+(Float2x2 left, Float2x2 right);
		friend forceinline Float2x2		vectorcall operator-(Float2x2 left, Float2x2 right);
		friend forceinline Float2x2		vectorcall operator*(Float2x2 left, Float2x2 right);
		friend forceinline Float2x2		vectorcall operator+(Float2x2 left, float right);
		friend forceinline Float2x2		vectorcall operator-(Float2x2 left, float right);
		friend forceinline Float2x2		vectorcall operator*(Float2x2 left, float right);
		friend forceinline Float2x2		vectorcall operator/(Float2x2 left, float right);
		friend forceinline Float2x2		vectorcall operator*(float left, Float2x2 right);
		friend forceinline Float2x2		vectorcall operator/(float left, Float2x2 right);

	private:
		SSE_VARIABLE(m_Xmm);

	public:
		static forceinline Float2x2 vectorcall Scale(Vec2f scale);
		static forceinline Float2x2 vectorcall Scale(float scale);
		static forceinline Float2x2 vectorcall Identity();
		static forceinline Float2x2 vectorcall Rotation(float angleRad);
	};

#if defined(SSE_INTRIN)
	forceinline Float2x2::Float2x2(__m128 xmm)
	{
		m_Xmm = xmm;
	}
    

	forceinline Float2x2::Float2x2(float diagonal)
	{
		m_Xmm = _mm_set_ps(diagonal, 0.0f, 0.0f, diagonal);
	}
    

	forceinline Float2x2::Float2x2(Float2 r0, Float2 r1)
	{
		m_Xmm = _mm_set_ps(r1.GetY(), r1.GetX(), r0.GetY(), r0.GetX());
	}
    

	forceinline Float2x2& vectorcall Float2x2::Multiply(Float2x2 other)
	{
		__m128 m1 = _mm_shuffle_ps(m_Xmm, m_Xmm, _MM_SHUFFLE(2, 2, 0, 0));
		m1 = _mm_mul_ps(m1, _mm_shuffle_ps(other.m_Xmm, other.m_Xmm, _MM_SHUFFLE(1, 0, 1, 0)));

		__m128 m2 = _mm_shuffle_ps(m_Xmm, m_Xmm, _MM_SHUFFLE(3, 3, 1, 1));
		m2 = _mm_mul_ps(m2, _mm_shuffle_ps(other.m_Xmm, other.m_Xmm, _MM_SHUFFLE(3, 2, 3, 2)));

		m_Xmm = _mm_add_ps(m1, m2);
		return *this;
	}
    

	forceinline Float2x2& vectorcall Float2x2::Multiply(float scalar)
	{
		m_Xmm = _mm_mul_ps(m_Xmm, _mm_set_ps1(scalar));
		return *this;
	}
    

	forceinline Float2x2& vectorcall Float2x2::Add(Float2x2 other)
	{
		m_Xmm = _mm_add_ps(m_Xmm, other.m_Xmm);
		return *this;
	}
    

	forceinline Float2x2& vectorcall Float2x2::Add(float scalar)
	{
		m_Xmm = _mm_add_ps(m_Xmm, _mm_set_ps1(scalar));
		return *this;
	}

    
	forceinline Float2x2& vectorcall Float2x2::Subtract(Float2x2 other)
	{
		m_Xmm = _mm_sub_ps(m_Xmm, other.m_Xmm);
		return *this;
	}
    

	forceinline Float2x2& vectorcall Float2x2::Subtract(float scalar)
	{
		m_Xmm = _mm_sub_ps(m_Xmm, _mm_set_ps1(scalar));
		return *this;
	}

    
	forceinline Float2x2& vectorcall Float2x2::Divide(float scalar)
	{
		m_Xmm = _mm_div_ps(m_Xmm, _mm_set_ps1(scalar));
		return *this;
	}

    
	forceinline bool vectorcall Float2x2::Equals(Float2x2 other) const
	{
		int32 mask = _mm_movemask_ps(_mm_cmpeq_ps(m_Xmm, other.m_Xmm));
		return 0xf == (mask & 0xf);
	}

    
	forceinline Float2x2& vectorcall Float2x2::Transpose()
	{
		m_Xmm = _mm_shuffle_ps(m_Xmm, m_Xmm, _MM_SHUFFLE(3, 1, 2, 0));
		return *this;
	}
    

	forceinline Float2x2& vectorcall Float2x2::Invert()
	{
		__m128	a = _mm_shuffle_ps(m_Xmm, m_Xmm, _MM_SHUFFLE(0, 1, 2, 3));
		__m128	b = _mm_mul_ps(m_Xmm, a);
				a = _mm_shuffle_ps(b, b, _MM_SHUFFLE(3, 2, 1, 1));

		__m128 det = _mm_sub_ss(b, a);
		det = _mm_shuffle_ps(det, det, _MM_SHUFFLE(0, 0, 0, 0));
		det = _mm_div_ps(_mm_set_ps(1.0f, -1.0f, -1.0f, 1.0f), det);

		m_Xmm = _mm_shuffle_ps(m_Xmm, m_Xmm, _MM_SHUFFLE(0, 2, 1, 3));
		m_Xmm = _mm_mul_ps(m_Xmm, det);
		return *this;
	}
    

	forceinline float vectorcall Float2x2::Determinant() const
	{
		__m128	a = _mm_shuffle_ps(m_Xmm, m_Xmm, _MM_SHUFFLE(0, 1, 2, 3));
		__m128	b = _mm_mul_ps(m_Xmm, a);
		a = _mm_shuffle_ps(b, b, _MM_SHUFFLE(3, 2, 1, 1));

		__m128 det = _mm_sub_ss(b, a);
		return _mm_cvtss_f32(det);
	}
    

	forceinline Float2x2 vectorcall Float2x2::Adjugate() const
	{
		Float2x2 a;
		a.m_Xmm = _mm_shuffle_ps(m_Xmm, m_Xmm, _MM_SHUFFLE(0, 2, 1, 3));
		a.m_Xmm = _mm_mul_ps(a.m_Xmm, _mm_set_ps(1.0f, -1.0f, -1.0f, 1.0f));
		return a;
	}
    

	forceinline void Float2x2::SetRow(uint32 r, float x, float y)
	{
		assert(r < 2);
		if (r == 0)
		{
			//--Set x--
			m_Xmm = _mm_move_ss(m_Xmm, _mm_set_ss(x));

			//--Set y--
			//t = [y, this->y, this->z, this->w]
			__m128 t = _mm_move_ss(m_Xmm, _mm_set_ss(y));
			//t = [y, y, this->z, this->w]
			t = _mm_shuffle_ps(t, t, _MM_SHUFFLE(3, 2, 0, 0));
			//m_Xmm = [this->x, y, this->z, this->w]
			m_Xmm = _mm_move_ss(t, m_Xmm);
		}
		else
		{
			//t = [x, this->y, this->z, this->w]
			__m128 t = _mm_move_ss(m_Xmm, _mm_set_ss(x));
			//t = [x, this->y, x, this->w]
			t = _mm_shuffle_ps(t, t, _MM_SHUFFLE(3, 0, 1, 0));
			//m_Xmm = [this->x, this->y, x, this->w]
			m_Xmm = _mm_move_ss(t, m_Xmm);

			//t = [y, this->y, this->z, this->w]
			t = _mm_move_ss(m_Xmm, _mm_set_ss(y));
			//t = [y, this->y, this->z, y]
			t = _mm_shuffle_ps(t, t, _MM_SHUFFLE(0, 2, 1, 0));
			//m_Xmm = [this->x, this->y, this->z, y]
			m_Xmm = _mm_move_ss(t, m_Xmm);
		}
	}

    
	forceinline Float2x2& vectorcall Float2x2::operator=(Float2x2 other)
	{
		m_Xmm = other.m_Xmm;
		return *this;
	}

    
	forceinline Mat2f vectorcall Store(Float2x2 f)
	{
		Mat2f result;
		_mm_store_ps(result.m, f.m_Xmm);
		return result;
	}

    
	forceinline Float2x2 vectorcall Load(Mat2f storage)
	{
		Float2x2 result;
		result.m_Xmm = _mm_load_ps(storage.m);
		return result;
	}

    
	forceinline Float2 Float2x2::GetRow(uint32 r) const
	{
		assert(r < 2);
		if (r == 0)
			return Float2(m_Xmm);

		__m128 row = _mm_shuffle_ps(m_Xmm, m_Xmm, _MM_SHUFFLE(3, 2, 3, 2));
		return Float2(row);
	}

#else
	forceinline Float2x2::Float2x2(float diagonal)
	{
		memset(m_Xmm, 0, sizeof(m_Xmm));
		m_Xmm[0] = diagonal;
		m_Xmm[3] = diagonal;
	}
	
    
	forceinline Float2x2::Float2x2(Float2 r0, Float2 r1)
	{
		m_Xmm[0] = r0.GetX();
		m_Xmm[1] = r0.GetY();
		m_Xmm[2] = r1.GetX();
		m_Xmm[3] = r1.GetY();
	}

    
	forceinline Float2x2::Float2x2(SSE_VARIABLE(xmm))
	{
		memcpy(m_Xmm, xmm, sizeof(m_Xmm));
	}
    

	forceinline Float2x2& vectorcall Float2x2::Multiply(Float2x2 other)
	{
		float t[4];

		t[0] = (m_Xmm[0] * other.m_Xmm[0]) + (m_Xmm[1] * other.m_Xmm[2]);
		t[1] = (m_Xmm[0] * other.m_Xmm[1]) + (m_Xmm[1] * other.m_Xmm[3]);

		t[2] = (m_Xmm[2] * other.m_Xmm[0]) + (m_Xmm[3] * other.m_Xmm[2]);
		t[3] = (m_Xmm[2] * other.m_Xmm[1]) + (m_Xmm[3] * other.m_Xmm[3]);

		memcpy(m_Xmm, t, sizeof(m_Xmm));
		return *this;
	}

    
	forceinline Float2x2& vectorcall Float2x2::Multiply(float scalar)
	{
		for (int i = 0; i < 4; i++)
			m_Xmm[i] *= scalar;
		return *this;
	}
    

	forceinline Float2x2& vectorcall Float2x2::Add(Float2x2 other)
	{
		for (int i = 0; i < 4; i++)
			m_Xmm[i] += other.m_Xmm[i];
		return *this;
	}

    
	forceinline Float2x2& vectorcall Float2x2::Add(float scalar)
	{
		for (int i = 0; i < 4; i++)
			m_Xmm[i] += scalar;
		return *this;
	}

    
	forceinline Float2x2& vectorcall Float2x2::Subtract(Float2x2 other)
	{
		for (int i = 0; i < 4; i++)
			m_Xmm[i] -= other.m_Xmm[i];
		return *this;
	}

    
	forceinline Float2x2& vectorcall Float2x2::Subtract(float scalar)
	{
		for (int i = 0; i < 4; i++)
			m_Xmm[i] -= scalar;
		return *this;
	}

    
	forceinline Float2x2& vectorcall Float2x2::Divide(float scalar)
	{
		for (int i = 0; i < 4; i++)
			m_Xmm[i] /= scalar;
		return *this;
	}
    

	forceinline bool vectorcall Float2x2::Equals(Float2x2 other) const
	{
		for (int i = 0; i < 4; i++)
		{
			if (m_Xmm[i] != other.m_Xmm[i])
				return false;
		}

		return true;
	}

    
	forceinline Float2x2& vectorcall Float2x2::Transpose()
	{
		float temp[4];

		temp[0] = m_Xmm[0];
		temp[1] = m_Xmm[2];

		temp[2] = m_Xmm[1];
		temp[3] = m_Xmm[3];

		memcpy(m_Xmm, temp, sizeof(m_Xmm));
		return *this;
	}

    
	forceinline Float2x2& vectorcall Float2x2::Invert()
	{
		float det = Determinant();

		if (det == 0)
			*this = Float2x2(0.0f);
		else
			*this = Adjugate() / det;

		return *this;
	}

    
	forceinline Float2x2 vectorcall Float2x2::Adjugate() const
	{
		Float2x2 adjugate;
		adjugate.m_Xmm[0] =  m_Xmm[3];
		adjugate.m_Xmm[2] = -m_Xmm[2];
		adjugate.m_Xmm[1] = -m_Xmm[1];
		adjugate.m_Xmm[3] =  m_Xmm[0];
		return adjugate;
	}

    
	forceinline void Float2x2::SetRow(uint32 r, float x, float y)
	{
		m_Xmm[(r * 2) + 0] = x;
		m_Xmm[(r * 2) + 1] = y;
	}

    
	forceinline Float2x2& vectorcall Float2x2::operator=(Float2x2 other)
	{
		memcpy(m_Xmm, other.m_Xmm, sizeof(m_Xmm));
		return *this;
	}
    

	forceinline Mat2f vectorcall Store(Float2x2 f)
	{
		Mat2f result;
		memcpy(result.m, f.m_Xmm, sizeof(f.m_Xmm));
		return result;
	}
    

	forceinline Float2x2 vectorcall Load(Mat2f storage)
	{
		Float2x2 result;
		memcpy(result.m_Xmm, storage.m, sizeof(result.m_Xmm));
		return result;
	}
    

	forceinline Float2 Float2x2::GetRow(uint32 r) const
	{
		assert(r < 2);
		return Float2(m_Xmm[r * 2 + 0], m_Xmm[r * 2 + 1]);
	}

    
	forceinline float vectorcall Float2x2::Determinant() const
	{
		return (m_Xmm[0] * m_Xmm[3]) - (m_Xmm[1] * m_Xmm[2]);
	}
#endif

    
	forceinline Float2 vectorcall Float2x2::Multiply(Float2 vector) const
	{
		float t[2];
		t[0] = (vector[0] * GetElement(0, 0)) + (vector[1] * GetElement(1, 0));
		t[1] = (vector[0] * GetElement(0, 1)) + (vector[1] * GetElement(1, 1));
		return Float2(t[0], t[1]);
	}

    
	forceinline Float2x2 vectorcall Float2x2::GetTranspose() const
	{
		Float2x2 transpose = *this;
		transpose.Transpose();
		return transpose;
	}

    
	forceinline Float2x2 vectorcall Float2x2::GetInverse() const
	{
		Float2x2 inverse = *this;
		inverse.Invert();
		return inverse;
	}

    
	forceinline Float2x2 vectorcall Float2x2::Cofactor() const
	{
		return Adjugate().Transpose();
	}

    
	forceinline float Float2x2::GetElement(uint32 r, uint32 c) const
	{
		assert(c < 2);
		return GetRow(r)[c];
	}

    
	forceinline std::string	vectorcall ToString(Float2x2 f)
	{
		return
			'[' + std::to_string(f.GetElement(0, 0)) + ", " + std::to_string(f.GetElement(0, 1)) + "]\n" +
			'[' + std::to_string(f.GetElement(1, 0)) + ", " + std::to_string(f.GetElement(1, 1)) + "]\n";
	}

    
	forceinline bool vectorcall Float2x2::operator==(Float2x2 other) const
	{
		return Equals(other);
	}

    
	forceinline bool vectorcall Float2x2::operator!=(Float2x2 other) const
	{
		return !Equals(other);
	}

    
	forceinline float vectorcall Float2x2::operator()(uint32 r, uint32 c) const
	{
		return GetElement(r, c);
	}

    
	forceinline Float2 vectorcall Float2x2::operator()(uint32 r) const
	{
		return GetRow(r);
	}

    
	forceinline Float2 vectorcall operator*(Float2 left, Float2x2 right)
	{
		return right.Multiply(left);
	}
    

	forceinline Float2& vectorcall operator*=(Float2& left, Float2x2 right)
	{
		return left = right.Multiply(left);
	}
    

	forceinline Float2x2 vectorcall operator+(Float2x2 left, Float2x2 right)
	{
		return left.Add(right);
	}

    
	forceinline Float2x2 vectorcall operator-(Float2x2 left, Float2x2 right)
	{
		return left.Subtract(right);
	}

    
	forceinline Float2x2 vectorcall operator*(Float2x2 left, Float2x2 right)
	{
		return left.Multiply(right);
	}

    
	forceinline Float2x2 vectorcall operator+(Float2x2 left, float right)
	{
		return left.Add(right);
	}

    
	forceinline Float2x2 vectorcall operator-(Float2x2 left, float right)
	{
		return left.Subtract(right);
	}

    
	forceinline Float2x2 vectorcall operator*(Float2x2 left, float right)
	{
		return left.Multiply(right);
	}

    
	forceinline Float2x2 vectorcall operator/(Float2x2 left, float right)
	{
		return left.Divide(right);
	}

    
	forceinline Float2x2 vectorcall operator*(float left, Float2x2 right)
	{
		return right.Multiply(left);
	}
    

	forceinline Float2x2 vectorcall operator/(float left, Float2x2 right)
	{
		return right.Divide(left);
	}

    
	forceinline Float2x2& vectorcall Float2x2::operator+=(Float2x2 right)
	{
		return Add(right);
	}

    
	forceinline Float2x2& vectorcall Float2x2::operator-=(Float2x2 right)
	{
		return Subtract(right);
	}

    
	forceinline Float2x2& vectorcall Float2x2::operator*=(Float2x2 right)
	{
		return Multiply(right);
	}

    
	forceinline Float2x2& vectorcall Float2x2::operator+=(float right)
	{
		return Add(right);
	}

    
	forceinline Float2x2& vectorcall Float2x2::operator-=(float right)
	{
		return Subtract(right);
	}

    
	forceinline Float2x2& vectorcall Float2x2::operator*=(float right)
	{
		return Multiply(right);
	}

    
	forceinline Float2x2& vectorcall Float2x2::operator/=(float right)
	{
		return Divide(right);
	}
    

	forceinline Float2x2 vectorcall Float2x2::Identity()
	{
		return Float2x2(1.0f);
	}
    

	forceinline Float2x2 vectorcall Float2x2::Rotation(float angleRad)
	{
		Float2x2 rot(1.0f);

		float cosine = cosf(angleRad);
		float sine = sinf(angleRad);

		rot.SetRow(0, cosine, sine);
		rot.SetRow(1, -sine, cosine);
		return rot;
	}
    

	forceinline Float2x2 vectorcall Float2x2::Scale(Vec2f scale)
	{
		Float2x2 s(1.0f);
		s.SetRow(0, scale.x, 0.0f);
		s.SetRow(1, 0.0f, scale.y);
		return s;
	}

    
	forceinline Float2x2 vectorcall Float2x2::Scale(float scale)
	{
		return Float2x2(scale);
	}
}
