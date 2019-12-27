#include "LambdaPch.h"
#include "Core/Input/CMouse.h"
#include "Core/Input/IMouseController.h"
#include "Core/CEngine.h"

namespace Lambda
{
    //------
    //CMouse
    //------

    CMouseState CMouse::s_LastState		= CMouseState();
    CMouseState CMouse::s_CurrentState	= CMouseState();

	bool CMouse::IsButtonDown(EMouseButton button)
	{
		IMouseController* pMouseController = CEngine::Get().GetMouseController();
		return pMouseController->IsButtonDown(button);
	}

	
	bool CMouse::IsMouseVisible()
	{
		IMouseController* pMouseController = CEngine::Get().GetMouseController();
		return pMouseController->IsMouseVisible();
	}


	void CMouse::SetMouseVisisble(bool bVisible)
	{
		IMouseController* pMouseController = CEngine::Get().GetMouseController();
		return pMouseController->SetMouseVisisble(bVisible);
	}
	
	
	bool CMouse::IsButtonUp(EMouseButton button)
	{
		IMouseController* pMouseController = CEngine::Get().GetMouseController();
		return pMouseController->IsButtonUp(button);
	}
	
	
	bool CMouse::IsButtonPressed(EMouseButton button)
	{
		IMouseController* pMouseController = CEngine::Get().GetMouseController();
		return s_LastState.IsButtonUp(button) && pMouseController->IsButtonDown(button);
	}
	
	
	void CMouse::SetPosition(const Point& position)
	{
		IMouseController* pMouseController = CEngine::Get().GetMouseController();
		pMouseController->SetPosition(position);
	}
	
	
	Point CMouse::GetPosition()
	{
		IMouseController* pMouseController = CEngine::Get().GetMouseController();
		return pMouseController->GetPosition();
	}
	
	
	const CMouseState& CMouse::GetState()
	{
		IMouseController* pMouseController = CEngine::Get().GetMouseController();
		s_CurrentState = pMouseController->GetState();
		return s_CurrentState;
	}


	void CMouse::UpdateState()
	{
		s_LastState = s_CurrentState;

		IMouseController* pMouseController = CEngine::Get().GetMouseController();
		s_CurrentState = pMouseController->GetState();
	}
}
