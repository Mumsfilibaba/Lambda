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
        CKeyboardState(const CKeyboardState& other) noexcept;
        CKeyboardState(CKeyboardState&& other) noexcept;
        ~CKeyboardState() = default;
        
        CKeyboardState& operator=(const CKeyboardState& other) noexcept;
        CKeyboardState& operator=(CKeyboardState&& other) noexcept;
        
		void SetKeyStateDown(EKey key);
		void SetKeyStateUp(EKey key);

		_forceinline bool IsKeyUp(EKey key) const
		{
			return m_Keys[key] == false;
		}

        _forceinline bool IsKeyDown(EKey key) const 
		{
			return m_Keys[key] == true;
		}	
	private:
		bool m_Keys[KEY_LAST+1];
	};
}
