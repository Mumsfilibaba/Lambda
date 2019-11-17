#include "LambdaPch.h"
#include "Events/Event.h"
#include "Events/EventDispatcher.h"
#include "Core/LogManager.h"

namespace Lambda
{
	//---------------
	//EventDispatcher
	//---------------

	bool EventDispatcher::DispatchEvent(const Event& event)
	{
		//Then send the event to the callbacks
		auto tableEntry = m_CallbackTable.find(event.GetType());
		if (tableEntry != m_CallbackTable.end())
		{
			//There are actually callback functions to recive this event
			auto& callbackList = tableEntry->second;
			for (auto& callback : callbackList)
			{
				if (callback->Callback(event))
				{
					event.SetIsHandled(true);
					//Should we break here?
				}
			}
		}

		//Did someone respond to the event?
		return event.IsHandled();
	}


	void EventDispatcher::Init()
	{
		//Does nothing yet
	}


	void EventDispatcher::Release()
	{
		//Release all callbacks
		for (auto& tableEntry : m_CallbackTable)
		{
			auto& callbackList = tableEntry.second;
			for (auto callback : callbackList)
			{
				SafeDelete(callback);
			}
		}
	}


	void EventDispatcher::PushCallback(EventType key, IEventCallback* pCallback)
	{
		//Check if list exists
		auto tableEntry = m_CallbackTable.find(key);
		if (tableEntry == m_CallbackTable.end())
		{
			//If not insert new element
			auto element = m_CallbackTable.insert(std::pair<EventType, CallbackList>(key, CallbackList()));
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
