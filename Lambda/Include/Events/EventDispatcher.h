#pragma once
#include "Events/Event.h"
#include <vector>

namespace Lambda
{
	typedef bool(*EventCallback)(const Event*);

    //----------
    //EventLayer
    //----------

	class LAMBDA_API EventLayer
	{
	public:
		LAMBDA_NO_COPY(EventLayer);

		inline EventLayer(const char* pName)
        : m_pName(pName) {}
		~EventLayer() = default;

		virtual void	OnPush()	= 0;
		virtual void	OnPop()	    = 0;
		virtual bool	OnEvent(const Event* pEvent) = 0;
		virtual uint32	GetRecivableCategories() const = 0;

        inline const char* GetName() const { return m_pName; }

	private:
		const char* m_pName;
	};

    //---------------
    //EventDispatcher
    //---------------

	class LAMBDA_API EventDispatcher
	{
	public:
		LAMBDA_STATIC_CLASS(EventDispatcher);

		static bool DispatchEvent(const Event* pEvent);
		static void PushEventLayer(EventLayer* pLayer);
		static void PopEventLayer();

	private:
		static std::vector<EventLayer*>	s_LayerStack;
	};
}
