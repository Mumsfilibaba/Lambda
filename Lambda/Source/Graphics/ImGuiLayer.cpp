#include "LambdaPch.h"
#include "Graphics/ImGuiLayer.h"

namespace Lambda
{
	//----------
	//ImGuiLayer
	//----------

    ImGuiLayer::ImGuiLayer()
        : EventLayer("ImGuiLayer")
    {
    }


    void ImGuiLayer::OnPop()
    {
    }


    void ImGuiLayer::OnPush()
    {
    }


    bool ImGuiLayer::OnEvent(const Event& event)
    {
        return false;
    }


    uint32 ImGuiLayer::GetRecivableCategories() const
    {
        return 0;
    }
}
