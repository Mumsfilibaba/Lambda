#include "SandBox.h"
#include "LambdaEntryPoint.h"
#include "Core/Input/CKeyboard.h"
#include "Core/Input/CMouse.h"
#include "Core/Input/CGamepad.h"
#include "Core/CLogManager.h"
#include "Core/CEnvironment.h"
#include "Core/Event/CMouseEvent.h"
#include "Core/Event/CEventDispatcher.h"
#include "Graphics/CMeshFactory.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/euler_angles.hpp>

#define SINGLE_CUBE
#define RGB_F(r, g, b) float(r) / 255.0f, float(g) / 255.0f, float(b) / 255.0f

namespace Lambda
{
	//------------------------
	//Define CreateGameLayer()
	//------------------------

	CLayer* CreateGameLayer(CEnvironment* pEnvironment)
	{
		return DBG_NEW SandBoxLayer();
	}

	//------------
	//SandBoxLayer
	//------------

	SandBoxLayer::SandBoxLayer()
         : CLayer("SandBoxLayer")
	{
		CEnvironment& environment = CEnvironment::Get();
		environment.AddEventListener(this);
	}


	bool SandBoxLayer::OnMouseMove(const CMouseMovedEvent& event)
	{
		LOG_DEBUG(LOG_CHANNEL_ALL_CHANNELS, LOG_SEVERITY_INFO, "Mouse moved: x=%d, y=%d\n", event.GetX(), event.GetY());
		return false;
	}


	bool SandBoxLayer::OnEvent(const CEvent& event)
	{
		CEventDispatcher dispatcher;
		dispatcher.Dispatch(this, &SandBoxLayer::OnMouseMove, event);
		return false;
	}


	void SandBoxLayer::OnLoad()
	{
	}


	void SandBoxLayer::OnUpdate(const CTime&)
	{
		if (CKeyboard::IsKeyPressed(KEY_A))
			CMouse::SetMouseVisisble(!CMouse::IsMouseVisible());

		if (CMouse::IsButtonDown(MOUSEBUTTON_LEFT))
			LOG_DEBUG(LOG_CHANNEL_ALL_CHANNELS, LOG_SEVERITY_INFO, "Left Button is down\n");


		CGamepadState gamepad = CGamepad::GetState(GAMEPAD_1);
		if (gamepad.IsConnected())
		{
			//LOG_DEBUG(LOG_CHANNEL_ALL_CHANNELS, LOG_SEVERITY_INFO, "Left trigger %.4f, Right trigger %.4f\n", gamepad.GetTrigger(GAMEPAD_TRIGGER_LEFT), gamepad.GetTrigger(GAMEPAD_TRIGGER_RIGHT));
		}
	}


	void SandBoxLayer::OnRender(Renderer3D&, const CTime&)
	{
	}


	void SandBoxLayer::OnRelease()
	{
		CLayer::OnRelease();

		CEnvironment& environment = CEnvironment::Get();
		environment.RemoveEventListener(this);
	}


    void SandBoxLayer::OnRenderUI(const CTime&)
    {
    }
}
