#include "LambdaPch.h"

#include "Core/Input/Input.h"

namespace Lambda
{
	//------
	//CInput
	//------

	IInputController* CInput::s_pController = nullptr;

	/*////////////////////////////////////////////////////////////////////////////////////////////////*/
	bool CInput::Initialize(EInputType)
	{
		IInputController* pController = nullptr;

		s_pController = pController;
		if (s_pController)
		{
			return s_pController->Initialize();
		}

		return true;
	}

	/*////////////////////////////////////////////////////////////////////////////////////////////////*/
	void CInput::Release()
	{
		if (s_pController)
		{
			s_pController->Release();
			s_pController = nullptr;
		}
	}
}
