#pragma once
#include <string>
#include <cassert>
#include <ctime>
#include <cmath>

#if defined(_WIN32) && defined(__ANDROID__)
	#undef _WIN32
#endif

#if defined(_WIN32) && !defined(NO_SIMD)
	#define SSE_INTRIN
#elif defined(__linux__) && !defined(NO_SIMD)
	#define SSE_INTRIN
#endif

#if defined(_WIN32)
	#define VECTORALIGN __declspec(align(16))
	#define MATRIXALIGN __declspec(align(16))
#elif defined(__linux__)
	#define VECTORALIGN __attribute__((aligned(16)))
	#define MATRIXALIGN __attribute__((aligned(16)))
#else
	#define VECTORALIGN
	#define MATRIXALIGN
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

#include "MathUtilities.h"
#include "SIMD.h"
