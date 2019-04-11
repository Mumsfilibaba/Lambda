#pragma once
#include "Float2.h"
#include "Color.h"

namespace Math
{
	class Random
	{
	public:
		Random();
		Random(int32 seed);
		
		void SetSeed(int32 seed);
		int32 GetSeed() const;

		int32 GenerateInt() const;
		int32 GenerateInt(int32 min, int32 max) const;
		float GenerateFloat() const;
		float GenerateFloat(float min, float max) const;
		double GenerateDouble() const;
		double GenerateDouble(double min, double max) const;
		Vec2f GenerateVector2() const;
		Vec3f GenerateVector3() const;
		Vec4f GenerateVector4() const;
		Color GenerateColor() const;

	private:
		int32 Rand() const;

	private:
		int32 m_Seed;
	};

	inline Random::Random()
		: m_Seed(static_cast<int32>(time(0)))
	{
		srand(m_Seed);
	}

	inline Random::Random(int32 seed)
		: m_Seed(seed)
	{
	}

	inline int32 Random::Rand() const
	{
		return rand();
	}

	inline void Random::SetSeed(int32 seed)
	{
		m_Seed = seed;
	}

	inline int32 Random::GetSeed() const
	{
		return m_Seed;
	}

	inline int32 Random::GenerateInt() const
	{
		return Rand();
	}
	
	inline int32 Random::GenerateInt(int32 min, int32 max) const
	{
		return (min < max) ? (min + GenerateInt() % (max - min + 1)) : 0;
	}

	inline float Random::GenerateFloat() const
	{
		return static_cast<float>(Rand()) / static_cast<float>(RAND_MAX);
	}

	inline float Random::GenerateFloat(float min, float max) const
	{
		return (min < max) ? min + GenerateFloat() * (max - min) : 0;
	}

	inline double Random::GenerateDouble() const
	{
		return static_cast<double>(Rand()) / static_cast<double>(RAND_MAX);
	}

	inline double Random::GenerateDouble(double min, double max) const
	{
		return (min < max) ? min + GenerateDouble() * (max - min) : 0;
	}

	inline Math::Vec2f Random::GenerateVector2() const
	{
		return	Vec2f(
					GenerateFloat(),
					GenerateFloat()
				);
	}
	
	inline Math::Vec3f Random::GenerateVector3() const
	{
		return	Vec3f(
					GenerateFloat(),
					GenerateFloat(),
					GenerateFloat()
				);
	}

	inline Math::Vec4f Random::GenerateVector4() const
	{
		return	Vec4f(
					GenerateFloat(),
					GenerateFloat(),
					GenerateFloat(),
					GenerateFloat()
				);
	}
	
	inline Color Random::GenerateColor() const
	{
		return Color(Rand() % 256, Rand() % 256, Rand() % 256, Rand() % 256);
	}
}