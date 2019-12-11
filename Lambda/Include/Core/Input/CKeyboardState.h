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

        _forceinline bool IsKeyDown(EKey key) 
		{
			return m_Keys[key] == true;
		}
			
		_forceinline bool IsKeyUp(EKey key) 
		{
			return m_Keys[key] == false;
		}
	private:
		bool m_Keys[KEY_LAST];
	};
}
