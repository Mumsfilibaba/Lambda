#include "LambdaPch.h"
#include "Events/EventDispatcher.h"
#include "System/Log.h"

namespace Lambda
{
	std::vector<EventLayer> EventDispatcher::s_LayerStack;

	bool EventDispatcher::SendEvent(const Event& event)
	{
		for (auto& layer : s_LayerStack)
		{
			//LOG_DEBUG_INFO("%s:\n", iter->pName);
			
			if (layer.OnEvent(event))
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
