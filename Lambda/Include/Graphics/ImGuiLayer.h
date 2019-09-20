#pragma once
#include "Events/Event.h"

namespace Lambda
{
	//----------
	//ImGuiLayer
	//----------

	class LAMBDA_API ImGuiLayer
	{
	public:
		LAMBDA_NO_COPY(ImGuiLayer);

		ImGuiLayer();
		~ImGuiLayer() = default;

	private:
		bool InternalOnEvent(const Event& event);

	private:
		static ImGuiLayer* s_pInstance;

	public:
		static bool OnEvent(const Event& event);
	};
}
