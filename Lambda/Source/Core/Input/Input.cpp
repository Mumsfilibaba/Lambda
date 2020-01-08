#include "LambdaPch.h"

#include "Core/Input/Input.h"

namespace Lambda
{
	//------
	//CInput
	//------

	IInputController* CInput::s_pController = nullptr;

	/*////////////////////////////////////////////////////////////////////////////////////////////////*/
	void CInput::Initialize(EInputType)
	{
		//LAMBDA_ASSERT_PRINT(s_pController == nullptr, "Input can only be initialized once");

		IInputController* pController = nullptr;
		s_pController = pController;
		if (s_pController)
		{
			s_pController->Init();
		}
	}

	/*////////////////////////////////////////////////////////////////////////////////////////////////*/
	void CInput::Release()
	{
		//LAMBDA_ASSERT_PRINT(s_pController != nullptr, "Input not initialized");

		//s_pController->Release();
		//s_pController = nullptr;
	}
}
