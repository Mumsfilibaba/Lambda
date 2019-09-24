#pragma once
#include "Events/Event.h"
#include <vector>
#include <unordered_map>

namespace Lambda
{
	typedef bool(*EventCallbackFunc)(const Event&);

    //----------
    //EventLayer
    //----------

	class LAMBDA_API EventLayer
	{
	public:
		LAMBDA_NO_COPY(EventLayer);

		inline EventLayer(const char* pName)
        : m_pName(pName) {}
		virtual ~EventLayer() = default;

		virtual void	OnPush()	= 0;
		virtual void	OnPop()	    = 0;
		virtual bool	OnEvent(const Event& event) = 0;
		virtual uint32	GetRecivableCategories() const = 0;

        inline const char* GetName() const { return m_pName; }

	private:
		const char* m_pName;
	};

	//--------------
	//IEventCallback
	//--------------

	class LAMBDA_API IEventCallback
	{
	public:
		virtual bool Callback(const Event& event) = 0;
	};

	//-------------
	//EventCallbackFunc
	//-------------

	template<typename EventT>
	class EventCallback : public IEventCallback
	{
		typedef bool(*EventCallbackFunc)(const EventT&);

	public:
		inline EventCallback(EventCallbackFunc func)
			: m_Func(func) {}
		virtual ~EventCallback() = default;

		inline virtual bool Callback(const Event& event) override final
		{
			return m_Func(static_cast<const EventT&>(event));
		}

	private:
		EventCallbackFunc m_Func;
	};

	//-------------------
	//ObjectEventCallback
	//-------------------

	template<typename ObjectType, typename EventT>
	class ObjectEventCallback : public IEventCallback
	{
		typedef bool(ObjectType::*ObjectEventCallbackFunc)(const EventT&);

	public:
		inline ObjectEventCallback(ObjectType* pObjectRef, ObjectEventCallbackFunc func)
			: m_pObjectRef(pObjectRef), m_Func(func) {}
		~ObjectEventCallback() = default;

		inline virtual bool Callback(const Event& event) override final
		{
			return (m_pObjectRef->*m_Func)(static_cast<const EventT&>(event));
		}

	private:
		ObjectType*				m_pObjectRef;
		ObjectEventCallbackFunc m_Func;
	};

    //---------------
    //EventDispatcher
    //---------------

	class LAMBDA_API EventDispatcher
	{
		typedef std::vector<IEventCallback*> CallbackList;

	public:
		LAMBDA_STATIC_CLASS(EventDispatcher);

		static bool DispatchEvent(const Event& pEvent);
		static void PushEventLayer(EventLayer* pLayer);
		static void PopEventLayer();
		static void Initialize();
		static void Release();

		//Push a global callback function
		template <typename EventT>
		static void PushCallback(bool(*callbackFunc)(const EventT&))
		{
			PushCallback(EventT::GetStaticType(), DBG_NEW EventCallback<EventT>(callbackFunc));
		}

		//Push object callback function
		template <typename ObjectType, typename EventT>
		static void PushCallback(ObjectType* pObjectRef, bool(ObjectType::* objectCallbackFunc)(const EventT&))
		{
			PushCallback(EventT::GetStaticType(), DBG_NEW ObjectEventCallback<ObjectType, EventT>(pObjectRef, objectCallbackFunc));
		}

		//Forward event to another function
		template <typename ObjectType, typename EventT>
		static bool ForwardEvent(ObjectType* pObjectRef, bool(ObjectType::* objectCallbackFunc)(const EventT&), const Event& event)
		{
			if (event.GetType() == EventT::GetStaticType())
			{
				if ((pObjectRef->*objectCallbackFunc)(static_cast<const EventT&>(event)))
				{
					event.SetIsHandled(true);
					return true;
				}
			}

			return false;
		}

	private:
		static void PushCallback(EventType key, IEventCallback* pCallback);

	private:
		static std::vector<EventLayer*>						s_LayerStack;
		static std::unordered_map<EventType, CallbackList>	s_CallbackTable;
	};
}
