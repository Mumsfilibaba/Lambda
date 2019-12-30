#pragma once
#include "Defines.h"
#include "Types.h"

namespace Lambda 
{
	namespace Math
	{
		template<typename T>
		_forceinline T AlignUp(T value, size_t alignment)
		{
			size_t mask = alignment - 1;
			return (T)(((size_t)value + mask) & ~mask);
		}

		template<typename T>
		_forceinline T AlignDown(T value, size_t alignment)
		{
			size_t mask = alignment - 1;
			return (T)((size_t)value & ~mask);
		}
	}
}
