#pragma once
#if defined(_WIN32)
#if defined(SSE_INTRIN)
	#define forceinline __forceinline
	#define vectorcall __vectorcall
#else
	#define forceinline inline
	#define vectorcall
#endif
#elif defined(__linux__)
	#define forceinline __attribute__((always_inline)) inline
	#define vectorcall
#elif defined(__APPLE__)
    #define forceinline inline
    #define vectorcall
#else
	#define forceinline
	#define vectorcall
#endif

constexpr double PI = 3.1415926535898;
constexpr double E = 2.7182818284590;
constexpr double TWO_PI = PI * 2.0;
constexpr float PI_F = 3.141592f;
constexpr float E_F = 2.718281f;
constexpr float TWO_PI_F = 2.0f * PI_F;

namespace Math
{
	forceinline double ToDegrees(double radians)
	{
		return (180.0 * radians) / PI;
	}

	forceinline double ToRadians(double degrees)
	{
		return (PI * degrees) / 180.0;
	}

	forceinline float ToDegreesF(float radians)
	{
		return (180.0f * radians) / PI_F;
	}

	forceinline float ToRadiansF(float degrees)
	{
		return (PI_F * degrees) / 180.0f;
	}

	template<typename type>
	forceinline type Max(type f1, type f2)
	{
		return (f1 > f2) ? f1 : f2;
	}

	template<typename type>
	forceinline type Min(type f1, type f2)
	{
		return (f1 < f2) ? f1 : f2;
	}

	template<typename type>
	forceinline type AlignUp(type value, uint64 alignment)
	{
		uint64 mask = alignment - 1;
		return (type)(((uint32)value + mask) & ~mask);
	}

	template<typename type>
	forceinline type AlignDown(type value, uint64 alignment)
	{
		uint64 mask = alignment - 1;
		return (type)((uint32)value & ~mask);
	}
}
