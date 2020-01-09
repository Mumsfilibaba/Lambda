#include "LambdaPch.h"

#include "Core/Input/Input.h"

namespace Lambda
{
	//-----
	//Input
	//-----

	IInputController* Input::s_pController = nullptr;

	/*////////////////////////////////////////////////////////////////////////////////////////////////*/
	void Input::Initialize(EInputType)
	{
		//LAMBDA_ASSERT_PRINT(s_pController == nullptr, "Input can only be initialized once");

		IInputController* pController = nullptr;
		s_pController = pController;
	}

	/*////////////////////////////////////////////////////////////////////////////////////////////////*/
	void Input::Release()
	{
		//LAMBDA_ASSERT_PRINT(s_pController != nullptr, "Input not initialized");

		//s_pController->Release();
		//s_pController = nullptr;
	}
}
