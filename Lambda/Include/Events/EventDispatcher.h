#pragma once
#include "Events/Event.h"
#include <vector>

namespace Lambda
{
	typedef bool(*EventCallback)(const Event& event);

    //----------
    //EventLayer
    //----------

	class LAMBDA_API EventLayer
	{
	public:
		LAMBDA_NO_COPY(EventLayer);

		EventLayer(const char* pName);
		~EventLayer() = default;

		virtual void	OnPush()	= 0;
		virtual void	OnPop()	= 0;
		virtual bool	OnEvent(const Event& event) = 0;
		virtual uint32	GetRecivableCategories() const = 0;

		const char* GetName() const;

	private:
		const char* m_pName;
	};


	inline EventLayer::EventLayer(const char* pName)
		: m_pName(pName)
	{
	}
	

	inline const char* EventLayer::GetName() const
	{
		return m_pName;
	}


    //---------------
    //EventDispatcher
    //---------------

	class LAMBDA_API EventDispatcher
	{
	public:
		LAMBDA_STATIC_CLASS(EventDispatcher);

		static bool DispatchEvent(const Event& event);
		static void PushEventLayer(EventLayer* pLayer);
		static void PopEventLayer();

	private:
		static std::vector<Event>		s_EventQueue;
		static std::vector<EventLayer*>	s_LayerStack;
	};
}
