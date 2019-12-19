#pragma once
#include "LambdaCore.h"

namespace Lambda
{
    struct SEvent;

	//--------------
	//IEventCallback
	//--------------

	class IEventCallback
	{
	public:
		LAMBDA_INTERFACE(IEventCallback);

		virtual bool Call(const SEvent& event) = 0;
		virtual IEventCallback* Copy() = 0;
	};

	//--------------------
	//CStaticEventCallback
	//--------------------

	class CStaticEventCallback final : public IEventCallback
	{
	public:
		typedef bool(*EventCallbackFunc)(const SEvent&);

		_forceinline   CStaticEventCallback(EventCallbackFunc func)
			: IEventCallback(),
			m_Func(func)
		{
		}
		~CStaticEventCallback() = default;

		_forceinline  virtual bool Call(const SEvent& event) override final
		{
			return m_Func(event);
		}

		_forceinline  virtual IEventCallback* Copy() override final
		{
			return DBG_NEW CStaticEventCallback(m_Func);
		}
	private:
		EventCallbackFunc m_Func;
	};

	//--------------------
	//CObjectEventCallback
	//--------------------

	template<typename O>
	class CObjectEventCallback final : public IEventCallback
	{
	public:
		typedef bool(O::* ObjectEventCallbackFunc)(const SEvent&);

		_forceinline  CObjectEventCallback(O* pObjectRef, ObjectEventCallbackFunc func)
			: IEventCallback(),
			m_pObjectRef(pObjectRef),
			m_Func(func)
		{
		}
		~CObjectEventCallback() = default;

		_forceinline  virtual bool Call(const SEvent& event) override final
		{
			return (m_pObjectRef->*m_Func)(event);
		}

		_forceinline  virtual IEventCallback* Copy() override final
		{
			return DBG_NEW CObjectEventCallback(m_pObjectRef, m_Func);
		}
	private:
		O* m_pObjectRef;
		ObjectEventCallbackFunc m_Func;
	};

	//--------------
	//CEventCallback
	//--------------

	class CEventCallback
	{
	public:
		_forceinline CEventCallback(CStaticEventCallback::EventCallbackFunc func)
			: m_pCallback(nullptr)
		{
			m_pCallback = DBG_NEW CStaticEventCallback(func);
		}


		template<typename T>
		using ObjectEventCallbackFunc = typename CObjectEventCallback<T>::ObjectEventCallbackFunc;

		template<typename T>
		_forceinline CEventCallback(T* pObjectRef, ObjectEventCallbackFunc<T> func)
			: m_pCallback(nullptr)
		{
			m_pCallback = DBG_NEW CObjectEventCallback<T>(pObjectRef, func);
		}


		_forceinline CEventCallback(CEventCallback&& other) noexcept
			: m_pCallback(other.m_pCallback)
		{
			other.m_pCallback = nullptr;
		}


		_forceinline CEventCallback(const CEventCallback& other) noexcept
			: m_pCallback(nullptr)
		{
			if (other.m_pCallback)
			{
				m_pCallback = other.m_pCallback->Copy();
			}
		}


		_forceinline ~CEventCallback()
		{
			if (m_pCallback)
			{
				delete m_pCallback;
				m_pCallback = nullptr;
			}
		}


		_forceinline CEventCallback& operator=(const CEventCallback& other) noexcept
		{
			if (this != &other)
			{
				if (m_pCallback)
				{
					delete m_pCallback;
				}

				if (other.m_pCallback)
				{
					m_pCallback = other.m_pCallback->Copy();
				}
			}

			return *this;
		}


		_forceinline CEventCallback& operator=(CEventCallback&& other) noexcept
		{
			if (this != &other)
			{
				m_pCallback = other.m_pCallback;
				other.m_pCallback = nullptr;
			}

			return *this;
		}


		_forceinline bool operator()(const SEvent& event)
		{
			return m_pCallback->Call(event);
		}
	private:
		IEventCallback* m_pCallback;
	};
}