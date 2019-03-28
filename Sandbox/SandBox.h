#pragma once
#include <System/Application.h>
#include <Graphics/ICommandList.h>

namespace Lambda
{
	class SandBox final : public Application
	{
	public:
		SandBox();
		~SandBox();

		virtual void OnLoad() override final;
		virtual void OnUpdate(Time dt) override final;
		virtual void OnRender(Time dt) override final;
		virtual void OnRelease() override final;

	private:
		ICommandList* m_pList;
	};
}