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
#include "MathUtilities.h"

#if defined(_WIN32) && defined(__ANDROID__)
#undef _WIN32
#endif

#if defined(_WIN32)
#define forceinline _forceinline
#elif defined(__linux__)
#define forceinline __attribute__((always_inline)) inline
#else
#define forceinline 
#endif

#if defined(_WIN32)
#define VECTORALIGN __declspec(align(16))
#elif defined(__linux__)
#define VECTORALIGN __attribute__((aligned(16)))
#else
#define VECTORALIGN
#endif

#if defined(_WIN32) && !defined(NO_SIMD)
#define SSE_INTRIN
#elif defined(__linux__) && !defined(NO_SIMD)
#define SSE_INTRIN
#endif

#if defined(SSE_INTRIN)
extern "C"
{
#include <xmmintrin.h>
}

#define Swizzle128(vec, x, y, z, w) _mm_shuffle_ps(vec, vec, (x | (y << 2) | (z << 4) | (w << 6)))
#define Matrix2Mul(m1, m2) _mm_add_ps(_mm_mul_ps(m1, Swizzle128(m2, 0, 3, 0, 3)), _mm_mul_ps(Swizzle128(m1, 1, 0, 3, 2), Swizzle128(m2, 2, 1, 2, 1)))
#define Matrix2AdjMul(m1, m2) _mm_sub_ps(_mm_mul_ps(Swizzle128(m1, 3, 3, 0, 0), m2), _mm_mul_ps(Swizzle128(m1, 1, 1, 2, 2), Swizzle128(m2, 2, 3, 0, 1)))
#define Matrix2MulAdj(m1, m2) _mm_sub_ps(_mm_mul_ps(m1, Swizzle128(m2, 3, 3, 0, 0)), _mm_mul_ps(Swizzle128(m1, 1, 0, 3, 2), Swizzle128(m2, 2, 1, 2, 1)))

forceinline __m128 HorizonatalSum(__m128 vec)
{
	__m128 shuffled = _mm_shuffle_ps(vec, vec, _MM_SHUFFLE(2, 3, 0, 1));
	__m128 summed = _mm_add_ps(vec, shuffled);
	shuffled = _mm_movehl_ps(shuffled, summed);
	summed = _mm_add_ss(shuffled, summed);
	return _mm_shuffle_ps(summed, summed, 0);
}
#endif

namespace Math
{
	typedef char int8;
	typedef short int16;
	typedef int int32;
	typedef long long int64;
	typedef unsigned char uint8;
	typedef unsigned short uint16;
	typedef unsigned int uint32;
	typedef unsigned long long uint64;
}