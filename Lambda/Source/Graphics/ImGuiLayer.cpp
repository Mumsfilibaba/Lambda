#include "LambdaPch.h"
#include "Graphics/ImGuiLayer.h"

namespace Lambda
{
	//----------
	//ImGuiLayer
	//----------

	ImGuiLayer* ImGuiLayer::s_pInstance = nullptr;

	ImGuiLayer::ImGuiLayer()
	{
		LAMBDA_ASSERT(s_pInstance == nullptr);
		s_pInstance = this;
	}


	bool ImGuiLayer::InternalOnEvent(const Event& event)
	{
		return false;
	}
	
	
	bool ImGuiLayer::OnEvent(const Event& event)
	{
		return (s_pInstance) ? s_pInstance->InternalOnEvent(event) : false;
	}
}