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
	forceinline double vectorcall ToDegrees(double radians)
	{
		return (180.0 * radians) / PI;
	}

	forceinline double vectorcall ToRadians(double degrees)
	{
		return (PI * degrees) / 180.0;
	}

	forceinline float vectorcall ToDegreesF(float radians)
	{
		return (180.0f * radians) / PI_F;
	}

	forceinline float vectorcall ToRadiansF(float degrees)
	{
		return (PI_F * degrees) / 180.0f;
	}

	forceinline float vectorcall MaxF(float f1, float f2)
	{
		return (f1 > f2) ? f1 : f2;
	}

	forceinline float vectorcall MinF(float f1, float f2)
	{
		return (f1 < f2) ? f1 : f2;
	}

	forceinline double vectorcall Max(double d1, double d2)
	{
		return (d1 > d2) ? d1 : d2;
	}

	forceinline double vectorcallMin(double d1, double d2)
	{
		return (d1 < d2) ? d1 : d2;
	}
}