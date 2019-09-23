#pragma once
#include "Defines.h"
#include "Types.h"

namespace Lambda
{
    //-----------
    //HashCombine
    //-----------

    template<typename T>
    inline void HashCombine(size_t& hash, const T& value)
    {
        std::hash<T> hasher;
        hash ^= hasher(value) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
    }

    //----------
    //StringHash
    //----------

    constexpr size_t StringHash(const char* pString)
    {
        static_assert(sizeof(size_t) == 8 | sizeof(size_t) == 4, "Only supports 64- or 32-bit versions of size_t");
        
        size_t hash = sizeof(size_t) == 8 ? 1125899906842597L : 4294967291L;
        for (uint32 i = 0; pString[i] != 0; i++)
            hash = (hash * 31) + pString[i];
            
        return hash;
    }
}
