#pragma once
#include "Events/EventDispatcher.h"

namespace Lambda
{
	//----------
	//ImGuiLayer
	//----------

	class LAMBDA_API ImGuiLayer : public EventLayer
	{
	public:
		LAMBDA_NO_COPY(ImGuiLayer);

		ImGuiLayer();
		~ImGuiLayer() = default;

        virtual void    OnPop() override final;
        virtual void    OnPush() override final;
        virtual bool    OnEvent(const Event& event) override final;
        virtual uint32  GetRecivableCategories() const override final;
	};
}
