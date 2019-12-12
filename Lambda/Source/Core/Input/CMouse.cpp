#include "LambdaPch.h"
#include "Core/Input/CMouse.h"
#include "Core/Input/IMouseController.h"
#include "Core/CEnvironment.h"

#define GET_MOUSECONTROLLER Lambda::CEnvironment::Get().GetMouseController

namespace Lambda
{

    //------
    //CMouse
    //------

    CMouseState CMouse::s_LastState		= CMouseState();
    CMouseState CMouse::s_CurrentState	= CMouseState();

    void CMouse::Update()
    {
        s_LastState	= s_CurrentState;
        
        IMouseController* pMouseController = GET_MOUSECONTROLLER();
        s_CurrentState	= pMouseController->GetState();
    }
	
	
	bool CMouse::IsButtonDown(EMouseButton button)
	{
		IMouseController* pMouseController = GET_MOUSECONTROLLER();
		return pMouseController->IsButtonDown(button);
	}
	
	
	bool CMouse::IsButtonUp(EMouseButton button)
	{
		IMouseController* pMouseController = GET_MOUSECONTROLLER();
		return s_CurrentState.IsButtonUp(button);
	}
	
	
	bool CMouse::IsButtonPressed(EMouseButton button)
	{
		IMouseController* pMouseController = GET_MOUSECONTROLLER();
		return s_LastState.IsButtonUp(button) && pMouseController->IsButtonDown(button);
	}
	
	
	void CMouse::SetPosition(const Point& position)
	{
		IMouseController* pMouseController = GET_MOUSECONTROLLER();
		pMouseController->SetPosition(position);
	}
	
	
	Point CMouse::GetPosition()
	{
		IMouseController* pMouseController = GET_MOUSECONTROLLER();
		return pMouseController->GetPosition();
	}
	
	
	const CMouseState& CMouse::GetState()
	{
		IMouseController* pMouseController = GET_MOUSECONTROLLER();
		s_CurrentState = pMouseController->GetState();
		return s_CurrentState;
	}
}
