#pragma once

constexpr double PI = 3.1415926535898;
constexpr double E = 2.7182818284590;
constexpr double TWO_PI = PI * 2.0;
constexpr float PI_F = 3.141592f;
constexpr float E_F = 2.718281f;
constexpr float TWO_PI_F = 2.0f * PI_F;

namespace Math
{
	inline double ToDegrees(double radians)
	{
		return (180.0 * radians) / PI;
	}

	inline double ToRadians(double degrees)
	{
		return (PI * degrees) / 180.0;
	}

	inline float ToDegreesF(float radians)
	{
		return (180.0f * radians) / PI_F;
	}

	inline float ToRadiansF(float degrees)
	{
		return (PI_F * degrees) / 180.0f;
	}

	inline float MaxF(float f1, float f2)
	{
		return (f1 > f2) ? f1 : f2;
	}

	inline float MinF(float f1, float f2)
	{
		return (f1 < f2) ? f1 : f2;
	}

	inline double Max(double d1, double d2)
	{
		return (d1 > d2) ? d1 : d2;
	}

	inline double Min(double d1, double d2)
	{
		return (d1 < d2) ? d1 : d2;
	}
}