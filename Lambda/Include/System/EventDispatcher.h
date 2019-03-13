#pragma once
#include "Event.h"
#include <vector>

namespace Lambda
{
	typedef bool(*EventCallback)(const Event& event);


	struct EventLayer
	{
		EventCallback OnEvent;
		const char* pName;
	};


	class LAMBDA_API EventDispatcher
	{
	public:
		LAMBDA_STATIC_CLASS(EventDispatcher);

		static bool SendEvent(const Event& event);
		static void PushEventLayer(const EventLayer& layer);
		static void PopEventLayer();

	private:
		static std::vector<Event> s_EventQueue;
		static std::vector<EventLayer> s_LayerStack;
	};
}