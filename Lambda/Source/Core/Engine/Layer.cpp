#include "LambdaPch.h"

#include "Core/Engine/Layer.h"

namespace Lambda
{
	//-----
	//Layer
	//-----

	/*////////////////////////////////////////////////////////////////////////////////////////////////*/
	Layer::Layer(const char*)
	{
	}

	/*////////////////////////////////////////////////////////////////////////////////////////////////*/
	void Layer::OnUpdate(const Time& deltaTime)
	{
	}

	/*////////////////////////////////////////////////////////////////////////////////////////////////*/
	bool Layer::OnSystemEvent(const SystemEvent&)
	{
		return false;
	}
}
