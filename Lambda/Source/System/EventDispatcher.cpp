#include "LambdaPch.h"
#include "Events/EventDispatcher.h"
#include "System/Log.h"

namespace Lambda
{
	//---------------
	//EventDispatcher
	//---------------

	std::vector<EventLayer*> EventDispatcher::s_LayerStack;

	bool EventDispatcher::DispatchEvent(const Event& event)
	{
		for (auto& pLayer : s_LayerStack)
		{
			//LOG_DEBUG_INFO("%s:\n", iter->pName);
			
			uint32 categories = pLayer->GetRecivableCategories();
			if (categories & event.Category)
			{
				if (pLayer->OnEvent(event))
					return true;
			}
		}

		return false;
	}


	void EventDispatcher::PushEventLayer(EventLayer* pLayer)
	{
		LAMBDA_ASSERT(pLayer != nullptr);
		s_LayerStack.push_back(pLayer);
		pLayer->OnPush();

		LOG_DEBUG_INFO("Lambda Engine: Pushed eventlayer %d '%s'.\n", s_LayerStack.size(), pLayer->GetName());
	}
    

	void EventDispatcher::PopEventLayer()
	{
		auto pLayer = s_LayerStack.back();
		s_LayerStack.pop_back();

		pLayer->OnPop();

		LOG_DEBUG_INFO("Lambda Engine: Poped eventlayer '%s'.\n", pLayer->GetName());
	}
}
