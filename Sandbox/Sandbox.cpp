#include "SandBox.h"

#include "Core/LambdaEntryPoint.h"

#include "Core/Event/SystemEvent.h"
#include "Core/Engine/Console.h"

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

	/*////////////////////////////////////////////////////////////////////////////////////////////////*/
	Layer* CreateGameLayer()
	{
		return DBG_NEW SandBoxLayer();
	}

	//-------------
	//SandBoxLayer
	//-------------

	/*////////////////////////////////////////////////////////////////////////////////////////////////*/
	SandBoxLayer::SandBoxLayer()
		: Layer("SandBoxLayer")
	{
	}

	/*////////////////////////////////////////////////////////////////////////////////////////////////*/
	void SandBoxLayer::OnUpdate(const Time& deltaTime)
	{
	}

	/*////////////////////////////////////////////////////////////////////////////////////////////////*/
	bool SandBoxLayer::OnSystemEvent(const SystemEvent& event)
	{
		if (event.EventType == ESystemEvent::SYSTEM_EVENT_KEY_PRESSED)
		{
			if (event.KeyEvent.Key == EKey::KEY_ESCAPE)
			{
				CEngine& engine = CEngine::Get();
				engine.Exit(0);

				return true;
			}
		}

		return false;
	}
}
