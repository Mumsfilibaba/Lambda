#pragma once
#include "Vector2.h"
#include "Color.h"

namespace Math
{
	class RandomGenerator
	{
	public:
		RandomGenerator();
		RandomGenerator(int32 seed);
		
		void SetSeed(int32 seed);
		int GetSeed() const;

		int32 GenerateInt() const;
		int32 GenerateInt(int32 min, int32 max) const;
		float GenerateFloat() const;
		float GenerateFloat(float min, float max) const;
		double GenerateDouble() const;
		double GenerateDouble(double min, double max) const;
		Vector2 GenerateVector2() const;
		Vector3 GenerateVector3() const;
		Vector4 GenerateVector4() const;
		Color GenerateColor() const;
		ColorF GenerateColorF() const;

	private:
		void SetStaticSeed() const;

	private:
		static int32 s_CurrentSeed;

	private:
		int32 m_Seed;
	};
}