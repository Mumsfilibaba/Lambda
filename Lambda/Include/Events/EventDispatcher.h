#pragma once
#include "Events/Event.h"
#include "Time/Timestep.h"
#include <vector>
#include <unordered_map>

namespace Lambda
{
	typedef bool(*EventCallbackFunc)(const Event&);

	//--------------
	//IEventCallback
	//--------------

	class LAMBDA_API IEventCallback
	{
	public:
        IEventCallback() = default;
        virtual ~IEventCallback() = default;
		virtual bool Callback(const Event& event) = 0;
	};

	//-----------------
	//EventCallbackFunc
	//-----------------

	template<typename EventT>
	class EventCallback : public IEventCallback
	{
		typedef bool(*EventCallbackFunc)(const EventT&);

	public:
		_forceinline  EventCallback(EventCallbackFunc func)
			: m_Func(func) {}
		virtual ~EventCallback() = default;
		
		_forceinline  virtual bool Callback(const Event& event) override final
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
		_forceinline ObjectEventCallback(ObjectType* pObjectRef, ObjectEventCallbackFunc func)
			: m_pObjectRef(pObjectRef), m_Func(func) {}
		virtual ~ObjectEventCallback() = default;
		
		_forceinline virtual bool Callback(const Event& event) override final
		{
			return (m_pObjectRef->*m_Func)(static_cast<const EventT&>(event));
		}
	private:
		ObjectType*	m_pObjectRef;
		ObjectEventCallbackFunc m_Func;
	};

    //---------------
    //EventDispatcher
    //---------------

	class LAMBDA_API EventDispatcher
	{
		typedef std::vector<IEventCallback*> CallbackList;

	public:
        EventDispatcher() = default;
        ~EventDispatcher() = default;
        
        void Init();
        void Release();

        bool DispatchEvent(const Event& pEvent);
        void PushCallback(EventType key, IEventCallback* pCallback);
	private:
		std::unordered_map<EventType, CallbackList>	m_CallbackTable;
	};


    //--------------
    //EventForwarder
    //--------------

    class LAMBDA_API EventForwarder
    {
    public:
        EventForwarder() = default;
        ~EventForwarder() = default;
        
        //Forward event to another function
        template <typename ObjectType, typename EventT>
        inline bool ForwardEvent(ObjectType* pObjectRef, bool(ObjectType::* objectCallbackFunc)(const EventT&), const Event& event)
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
    };
}
