#pragma once
#include "../IObject.h"
#include <System/EventDispatcher.h>
#include <System/IWindow.h>

namespace Lambda
{
	enum GraphicsContextFlags
	{
		GRAPHICS_CONTEXT_FLAG_NONE = 0,
		GRAPHICS_CONTEXT_FLAG_DEBUG = (1 << 0),
		GRAPHICS_CONTEXT_FLAG_ALLOW_SOFTWARE_ADAPTER = (1 << 1),
	};

	class LAMBDA_API IGraphicsContext : public IObject
	{
	public:
		LAMBDA_INTERFACE(IGraphicsContext);

		IGraphicsContext() = default;
		~IGraphicsContext() = default;

	private:
		virtual bool InternalOnEvent(const Event& event) = 0;

	public:
		static IGraphicsContext* Create(IWindow* pWindow, GraphicsContextFlags flags);
		static bool OnEvent(const Event& event);

	protected:
		static IGraphicsContext* s_pInstance;
	};
}