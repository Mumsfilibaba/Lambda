#pragma once
#include "Defines.h"
#include "Types.h"

namespace Lambda 
{
	namespace Math
	{
		template<typename T>
		forceinline T AlignUp(T value, uint64 alignment)
		{
			uint64 mask = alignment - 1;
			return (T)(((uint32)value + mask) & ~mask);
		}

		template<typename T>
		forceinline T AlignDown(T value, uint64 alignment)
		{
			uint64 mask = alignment - 1;
			return (T)((uint32)value & ~mask);
		}
	}
}