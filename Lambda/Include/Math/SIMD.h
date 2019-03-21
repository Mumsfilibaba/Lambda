#pragma once
#if defined(SSE_INTRIN)
	extern "C"
	{
		#include <xmmintrin.h>
	}

	#define SSE_VARIABLE(name)				__m128 name
	#define SSE_ARRAY(name, count)			__m128 name[count]

	#define Swizzle128(vec, x, y, z, w)		_mm_shuffle_ps(vec, vec, (x | (y << 2) | (z << 4) | (w << 6)))
	#define Matrix2Mul(m1, m2)				_mm_add_ps(_mm_mul_ps(m1, Swizzle128(m2, 0, 3, 0, 3)), _mm_mul_ps(Swizzle128(m1, 1, 0, 3, 2), Swizzle128(m2, 2, 1, 2, 1)))
	#define Matrix2AdjMul(m1, m2)			_mm_sub_ps(_mm_mul_ps(Swizzle128(m1, 3, 3, 0, 0), m2), _mm_mul_ps(Swizzle128(m1, 1, 1, 2, 2), Swizzle128(m2, 2, 3, 0, 1)))
	#define Matrix2MulAdj(m1, m2)			_mm_sub_ps(_mm_mul_ps(m1, Swizzle128(m2, 3, 3, 0, 0)), _mm_mul_ps(Swizzle128(m1, 1, 0, 3, 2), Swizzle128(m2, 2, 1, 2, 1)))

	forceinline __m128 HorizonatalSum(__m128 vec)
	{
		__m128 shuffled = _mm_shuffle_ps(vec, vec, _MM_SHUFFLE(2, 3, 0, 1));
		__m128 summed = _mm_add_ps(vec, shuffled);
		shuffled = _mm_movehl_ps(shuffled, summed);
		summed = _mm_add_ss(shuffled, summed);
		return _mm_shuffle_ps(summed, summed, 0);
	}
#else
	#define SSE_VARIABLE(name)
	#define SSE_ARRAY(name, count)

	#define Swizzle128(vec, x, y, z, w)
	#define Matrix2Mul(m1, m2)
	#define Matrix2AdjMul(m1, m2)
	#define Matrix2MulAdj(m1, m2)
#endif