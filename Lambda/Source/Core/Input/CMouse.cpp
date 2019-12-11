#include "LambdaPch.h"
#include "Core/Input/CMouse.h"
#include "Core/Input/IMouseController.h"
#include "Core/CEnvironment.h"

namespace Lambda
{
    //------
    //CMouse
    //------

    CMouseState CMouse::s_LastState = CMouseState();
    CMouseState CMouse::s_CurrentState = CMouseState();

    void CMouse::Update()
    {
        CEnvironment& environment = CEnvironment::Get();
        IMouseController* pMouseController = environment.GetMouseController();
        
        s_LastState		= s_CurrentState;
        s_CurrentState	= pMouseController->GetMouseState();
    }
	
	
	bool CMouse::IsMouseButtonDown(EMouseButton button)
	{
		return s_CurrentState.IsButtonDown(button);
	}
	
	
	bool CMouse::IsMouseButtonUp(EMouseButton button)
	{
		return s_CurrentState.IsButtonUp(button);;
	}
	
	
	bool CMouse::IsMouseButtonPressed(EMouseButton button)
	{
		return s_LastState.IsButtonUp(button) && s_CurrentState.IsButtonDown(button);
	}
	
	
	void CMouse::SetMousePosition(const glm::ivec2& position)
	{
		CEnvironment& environment = CEnvironment::Get();
		IMouseController* pMouseController = environment.GetMouseController();
		pMouseController->SetPosition(position);
	}
	
	
	glm::ivec2 CMouse::GetMousePosition()
	{
		CEnvironment& environment = CEnvironment::Get();
		IMouseController* pMouseController = environment.GetMouseController();
		return pMouseController->GetPosition();
	}
	
	
	const CMouseState& CMouse::GetMouseState()
	{
		return s_CurrentState;
	}
}
