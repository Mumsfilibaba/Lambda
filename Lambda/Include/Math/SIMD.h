#pragma once
#if defined(SSE_INTRIN)
	extern "C"
	{
		#include <xmmintrin.h>
		#include <emmintrin.h>
		#include <pmmintrin.h>
	}

	#define SSE_VARIABLE_I(name)	__m128i name
	#define SSE_VARIABLE(name)		__m128 name
	#define SSE_ARRAY(name, count)	__m128 name[count]

	#define __m128Swizzle(vec, x, y, z, w)		_mm_shuffle_ps(vec, vec, (x | (y << 2) | (z << 4) | (w << 6)))
	#define __m128Swizzle1(vec, x)				__m128Swizzle(vec, x, x, x, x)
	#define __m128Float2Mul(m1, m2)				_mm_add_ps(_mm_mul_ps(m1, __m128Swizzle(m2, 0, 3, 0, 3)), _mm_mul_ps(__m128Swizzle(m1, 1, 0, 3, 2), __m128Swizzle(m2, 2, 1, 2, 1)))
	#define __m128Float2AdjMul(m1, m2)			_mm_sub_ps(_mm_mul_ps(__m128Swizzle(m1, 3, 3, 0, 0), m2), _mm_mul_ps(__m128Swizzle(m1, 1, 1, 2, 2), __m128Swizzle(m2, 2, 3, 0, 1)))
	#define __m128Float2MulAdj(m1, m2)			_mm_sub_ps(_mm_mul_ps(m1, __m128Swizzle(m2, 3, 0, 3, 0)), _mm_mul_ps(__m128Swizzle(m1, 1, 0, 3, 2), __m128Swizzle(m2, 2, 1, 2, 1)))

	forceinline __m128 __m128HorizonatalSum(__m128 vec)
	{
		__m128 shuffled		= _mm_shuffle_ps(vec, vec, _MM_SHUFFLE(2, 3, 0, 1));
		__m128 summed		= _mm_add_ps(vec, shuffled);
		shuffled			= _mm_movehl_ps(shuffled, summed);
		summed				= _mm_add_ss(shuffled, summed);
		return _mm_shuffle_ps(summed, summed, 0);
	}

	forceinline __m128 __m128Dot(__m128 a, __m128 b)
	{
		__m128 summed		= _mm_mul_ps(a, b);
		__m128 shuffle		= _mm_shuffle_ps(summed, summed, _MM_SHUFFLE(2, 3, 0, 1));
		summed				= _mm_add_ps(summed, shuffle);
		shuffle				= _mm_movehl_ps(shuffle, summed);
		return _mm_add_ps(summed, shuffle);
	}

	forceinline __m128 __m128RowAdd(const __m128 mat[4], __m128 row)
	{
		__m128	r0 = _mm_mul_ps(				_mm_shuffle_ps(row, row, 0x0),	mat[0]);
				r0 = _mm_add_ps(r0, _mm_mul_ps(	_mm_shuffle_ps(row, row, 0x55),	mat[1]));
				r0 = _mm_add_ps(r0, _mm_mul_ps(	_mm_shuffle_ps(row, row, 0xaa),	mat[2]));
				r0 = _mm_add_ps(r0, _mm_mul_ps(	_mm_shuffle_ps(row, row, 0xff),	mat[3]));
		return r0;
	}
#else
	#define SSE_VARIABLE_I(name)			int32 name[4]
	#define SSE_VARIABLE(name)				float name[4]
	#define SSE_ARRAY(name, count)			float name[4 * count]

	#define Swizzle128(vec, x, y, z, w)
	#define Matrix2Mul(m1, m2)
	#define Matrix2AdjMul(m1, m2)
	#define Matrix2MulAdj(m1, m2)
#endif