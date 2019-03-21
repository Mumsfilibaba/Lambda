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

#include "RayEngine.h"
#include "../../Include/Math/RandomGenerator.h"

namespace Math
{
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	int32 RandomGenerator::s_CurrentSeed = 0;


	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	RandomGenerator::RandomGenerator()
		: m_Seed(static_cast<int32>(time(0)))
	{
		SetStaticSeed();
	}


	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	RandomGenerator::RandomGenerator(int32 seed)
		: m_Seed(seed)
	{
		SetStaticSeed();
	}


	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	void RandomGenerator::SetSeed(int32 seed)
	{
		m_Seed = seed;
	}


	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	int32 RandomGenerator::GetSeed() const
	{
		return m_Seed;
	}


	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	int32 RandomGenerator::GenerateInt() const
	{
		SetStaticSeed();
		return rand();
	}


	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	int32 RandomGenerator::GenerateInt(int32 min, int32 max) const
	{
		return (min < max) ? (min + GenerateInt() % (max - min + 1)) : 0;
	}


	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	float RandomGenerator::GenerateFloat() const
	{
		SetStaticSeed();
		return static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
	}


	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	float RandomGenerator::GenerateFloat(float min, float max) const
	{
		return (min < max) ? min + GenerateFloat() * (max - min) : 0;
	}


	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	double RandomGenerator::GenerateDouble() const
	{
		SetStaticSeed();
		return static_cast<double>(rand()) / static_cast<double>(RAND_MAX);
	}


	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	double RandomGenerator::GenerateDouble(double min, double max) const
	{
		return (min < max) ? min + GenerateDouble() * (max - min) : 0;
	}


	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	Math::Vector2 RandomGenerator::GenerateVector2() const
	{
		SetStaticSeed();
		return Math::Vector2(static_cast<float>(rand()) / static_cast<float>(RAND_MAX), 
							 static_cast<float>(rand()) / static_cast<float>(RAND_MAX));
	}


	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	Math::Vector3 RandomGenerator::GenerateVector3() const
	{
		SetStaticSeed();
		return Math::Vector3(static_cast<float>(rand()) / static_cast<float>(RAND_MAX),
							 static_cast<float>(rand()) / static_cast<float>(RAND_MAX),
							 static_cast<float>(rand()) / static_cast<float>(RAND_MAX));
	}


	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	Math::Vector4 RandomGenerator::GenerateVector4() const
	{
		SetStaticSeed();
		return Math::Vector4(static_cast<float>(rand()) / static_cast<float>(RAND_MAX),
							 static_cast<float>(rand()) / static_cast<float>(RAND_MAX),
							 static_cast<float>(rand()) / static_cast<float>(RAND_MAX),
							 static_cast<float>(rand()) / static_cast<float>(RAND_MAX));
	}


	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	Color RandomGenerator::GenerateColor() const
	{
		SetStaticSeed();
		return Color(rand() % 256, rand() % 256, rand() % 256, rand() % 256);
	}


	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	ColorF RandomGenerator::GenerateColorF() const
	{
		SetStaticSeed();
		return ColorF(static_cast<float>(rand()) / static_cast<float>(RAND_MAX),
					  static_cast<float>(rand()) / static_cast<float>(RAND_MAX),
					  static_cast<float>(rand()) / static_cast<float>(RAND_MAX),
					  static_cast<float>(rand()) / static_cast<float>(RAND_MAX));
	}


	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	void RandomGenerator::SetStaticSeed() const
	{
		if (m_Seed == s_CurrentSeed)
			return;

		s_CurrentSeed = m_Seed;

		srand(m_Seed);
	}
}