#pragma once
#include "EKey.h"

namespace Lambda
{
    //--------------
    //CKeyboardState
    //--------------
	class LAMBDA_API CKeyboardState final
	{
    public:
        CKeyboardState();
        CKeyboardState(const bool bKeyStates[static_cast<int32>(EKey::KEY_LAST) + 1]);
        CKeyboardState(const CKeyboardState& other) noexcept;
        CKeyboardState(CKeyboardState&& other) noexcept;
        ~CKeyboardState() = default;
        
        CKeyboardState& operator=(const CKeyboardState& other) noexcept;
        CKeyboardState& operator=(CKeyboardState&& other) noexcept;
        
		_forceinline bool IsKeyUp(EKey key) const
		{
			return m_bKeyStates[static_cast<int32>(key)] == false;
		}

        _forceinline bool IsKeyDown(EKey key) const 
		{
			return m_bKeyStates[static_cast<int32>(key)] == true;
		}	
	private:
		bool m_bKeyStates[static_cast<int32>(EKey::KEY_LAST)+1];
	};
}
