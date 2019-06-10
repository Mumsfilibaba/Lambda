#include "LambdaPch.h"
#include "System/EventDispatcher.h"
#include "System/Log.h"

namespace Lambda
{
	std::vector<EventLayer> EventDispatcher::s_LayerStack;

	bool EventDispatcher::SendEvent(const Event& event)
	{
		for (auto iter = s_LayerStack.begin(); iter < s_LayerStack.end(); iter++)
		{
			//LOG_DEBUG_INFO("%s:\n", iter->pName);
			
			if (iter->OnEvent(event))
				return true;
		}

		return false;
	}
	
	void EventDispatcher::PushEventLayer(const EventLayer& layer)
	{
		s_LayerStack.push_back(layer);
		LOG_DEBUG_INFO("Lambda Engine: Added eventlayer %d '%s'.\n", s_LayerStack.size(), layer.pName);
	}

	void EventDispatcher::PopEventLayer()
	{
		s_LayerStack.pop_back();
	}
}