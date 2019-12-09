#pragma once
#include "LambdaCore.h"

namespace Lambda
{
    class CEvent;

	typedef bool(*EventCallbackFunc)(const Event&);

	//--------------
    //CEventCallback
	//--------------

	template<typename T>
	class CEventCallback : public IEventCallback
	{
		typedef bool(*EventCallbackFunc)(const T&);

	public:
		_forceinline  CEventCallback(EventCallbackFunc func)
			: m_Func(func) {}
		virtual ~CEventCallback() = default;
		
		_forceinline virtual bool Call(const CEvent& event) override final
		{
			return m_Func(static_cast<const T&>(event));
		}
	private:
		EventCallbackFunc m_Func;
	};

	//--------------------
	//CObjectEventCallback
	//--------------------

	template<typename O, typename T>
	class CObjectEventCallback : public IEventCallback
	{
		typedef bool(O::*ObjectEventCallbackFunc)(const T&);

	public:
		_forceinline CObjectEventCallback(O* pObjectRef, ObjectEventCallbackFunc func)
			: m_pObjectRef(pObjectRef), m_Func(func) {}
		virtual ~CObjectEventCallback() = default;
		
		_forceinline virtual bool Call(const CEvent& event) override final
		{
			return (m_pObjectRef->*m_Func)(static_cast<const T&>(event));
		}
	private:
		O*	m_pObjectRef;
		ObjectEventCallbackFunc m_Func;
	};
}
