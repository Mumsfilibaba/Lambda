#include "SandBox.h"

#include "Core/LambdaEntryPoint.h"

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

	CLayer* CreateGameLayer(IEngine* pEngine)
	{
		return DBG_NEW CSandBoxLayer(pEngine);
	}

	//-------------
	//CSandBoxLayer
	//-------------

	CSandBoxLayer::CSandBoxLayer(IEngine* pEngine)
		: CLayer("SandBoxLayer")
	{
	}


	bool CSandBoxLayer::OnSystemEvent(const SSystemEvent& event)
	{
		return false;
	}
}