#include "LambdaPch.h"
#include "Events/EventDispatcher.h"
#include "System/Log.h"

namespace Lambda
{
	//---------------
	//EventDispatcher
	//---------------

	std::vector<EventLayer*>										EventDispatcher::s_LayerStack;
	std::unordered_map<EventType, EventDispatcher::CallbackList>	EventDispatcher::s_CallbackTable;

	bool EventDispatcher::DispatchEvent(const Event& event)
	{
		//First send events to all the layers
		for (auto& pLayer : s_LayerStack)
		{
			//LOG_DEBUG_INFO("%s:\n", iter->pName);
			
			if (event.GetCategoryFlags() & pLayer->GetRecivableCategories())
			{
				if (pLayer->OnEvent(event))
				{
					event.SetIsHandled(true);
					break;
				}
			}
		}

		//Then send the event to the callbacks
		auto tableEntry = s_CallbackTable.find(event.GetType());
		if (tableEntry != s_CallbackTable.end())
		{
			//There are actually callback functions to recive this event
			auto& callbackList = tableEntry->second;
			for (auto& callback : callbackList)
			{
				if (callback->Callback(event));
				{
					event.SetIsHandled(true);
					//Should we break here?
				}
			}
		}

		//Did someone respond to the event?
		return event.IsHandled();
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


	void EventDispatcher::PushCallback(EventType key, IEventCallback* pCallback)
	{
		//Check if list exists
		auto tableEntry = s_CallbackTable.find(key);
		if (tableEntry == s_CallbackTable.end())
		{
			//If not insert new element
			auto element = s_CallbackTable.insert(std::pair<EventType, CallbackList>(key, CallbackList()));
			//Did insertion succeed
			if (element.second)
			{
				tableEntry = element.first;
			}
			else
			{
				return;
			}
		}

		//Insert new callback
		auto& callbackList = tableEntry->second;
		callbackList.push_back(pCallback);
	}
}
