#include "LambdaPch.h"
#include "Core/Input/CDefaultMouseController.h"
#include "Core/Event/CEventDispatcher.h"
#include "Core/Event/CMouseEvent.h"

namespace Lambda
{
	//-----------------------
	//CDefaultMouseController 
	//-----------------------

	CDefaultMouseController::CDefaultMouseController()
		: m_MouseState()
	{
	}
	
	
	void CDefaultMouseController::SetPosition(const glm::ivec2& position)
	{
		m_MouseState.SetPosition(position);
	}


	glm::ivec2 CDefaultMouseController::GetPosition() const
	{
		return m_MouseState.GetPosition();
	}


	CMouseState CDefaultMouseController::GetMouseState() const
	{
		return m_MouseState;
	}


	bool CDefaultMouseController::OnEvent(const CEvent& event)
	{
		CEventDispatcher dispatcher;
		dispatcher.Dispatch(this, &CDefaultMouseController::OnMouseMove, event);
		dispatcher.Dispatch(this, &CDefaultMouseController::OnMouseScroll, event);
		dispatcher.Dispatch(this, &CDefaultMouseController::OnMouseButtonPressed, event);
		dispatcher.Dispatch(this, &CDefaultMouseController::OnMouseButtonReleased, event);
		return false;
	}


	bool CDefaultMouseController::OnMouseMove(const CMouseMovedEvent& event)
	{
		m_MouseState.SetPosition(glm::ivec2(event.GetX(), event.GetY()));
		return false;
	}


	bool CDefaultMouseController::OnMouseScroll(const CMouseScrolledEvent& event)
	{
		m_MouseState.SetVerticalScollValue(event.GetVerticalValue());
		m_MouseState.SetHorizontalScollValue(event.GetHorizontalValue());
		return false;
	}


	bool CDefaultMouseController::OnMouseButtonPressed(const CMouseButtonPressedEvent& event)
	{
		m_MouseState.SetButtonDown(event.GetButton());
		return false;
	}


	bool CDefaultMouseController::OnMouseButtonReleased(const CMouseButtonReleasedEvent& event)
	{
		m_MouseState.SetButtonUp(event.GetButton());
		return false;
	}
}