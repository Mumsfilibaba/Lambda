#pragma once
#include "Vector2.h"
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
		Vector2 GenerateVector2() const;
		Vector3 GenerateVector3() const;
		Vector4 GenerateVector4() const;
		Color GenerateColor() const;

	private:
		int32 m_Seed;

	public:
		static const Random& Default();

	private:
		static const Random s_Default;
	};

	static const Random s_Default = Random();

	inline Random::Random()
		: m_Seed(static_cast<int32>(time(0)))
	{
	}

	inline Random::Random(int32 seed)
		: m_Seed(seed)
	{
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
		return rand();
	}
	
	inline int32 Random::GenerateInt(int32 min, int32 max) const
	{
		return (min < max) ? (min + GenerateInt() % (max - min + 1)) : 0;
	}

	inline float Random::GenerateFloat() const
	{
		return static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
	}

	inline float Random::GenerateFloat(float min, float max) const
	{
		return (min < max) ? min + GenerateFloat() * (max - min) : 0;
	}

	inline double Random::GenerateDouble() const
	{
		return static_cast<double>(rand()) / static_cast<double>(RAND_MAX);
	}

	inline double Random::GenerateDouble(double min, double max) const
	{
		return (min < max) ? min + GenerateDouble() * (max - min) : 0;
	}

	inline Math::Vector2 Random::GenerateVector2() const
	{
		return Math::Vector2(static_cast<float>(rand()) / static_cast<float>(RAND_MAX),
			static_cast<float>(rand()) / static_cast<float>(RAND_MAX));
	}
	
	inline Math::Vector3 Random::GenerateVector3() const
	{
		return Math::Vector3(static_cast<float>(rand()) / static_cast<float>(RAND_MAX),
			static_cast<float>(rand()) / static_cast<float>(RAND_MAX),
			static_cast<float>(rand()) / static_cast<float>(RAND_MAX));
	}

	inline Math::Vector4 Random::GenerateVector4() const
	{
		return Math::Vector4(static_cast<float>(rand()) / static_cast<float>(RAND_MAX),
			static_cast<float>(rand()) / static_cast<float>(RAND_MAX),
			static_cast<float>(rand()) / static_cast<float>(RAND_MAX),
			static_cast<float>(rand()) / static_cast<float>(RAND_MAX));
	}
	
	inline Color Random::GenerateColor() const
	{
		return Color(rand() % 256, rand() % 256, rand() % 256, rand() % 256);
	}

	inline const Random& Random::Default()
	{
		return s_Default;
	}
}