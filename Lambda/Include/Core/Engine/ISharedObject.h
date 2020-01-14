#pragma once
#include "LambdaCore.h"

namespace Lambda
{
    //-------------
    //ISharedObject
    //-------------
    
	//Interface for referencecounted objects. Classes that inherits from ISharedObject should not
	//have a virtual constructor since they should be deleted with the release-function
	class LAMBDA_API ISharedObject
	{
	public:
		LAMBDA_INTERFACE(ISharedObject);

		virtual uint32 Release() = 0;
		virtual uint32 AddRef()  = 0;
		virtual uint32 GetReferences() const = 0;
	};

	//-------------
	//CSharedObject
	//-------------

	//Base class for all referencecounted objects. Keeps track of all strong references.
	//Base should be the base interface. For example 'class VKNDevice : public CSharedObject<IDevice>'
	template<typename TBase>
	class CSharedObject : public TBase
	{
	public:
		CSharedObject()
			: TBase(),
			m_StrongReferences(0) 
		{
		}

		virtual ~CSharedObject() = default;

		LAMBDA_NO_COPY(CSharedObject);

		virtual uint32 Release() override
		{
			uint32 counter = --m_StrongReferences;
			if (counter < 1)
			{
				delete this;
			}

			return counter;
		}

		virtual uint32 AddRef() override
		{
			return ++m_StrongReferences;
		}

		virtual uint32 GetReferences() const override
		{
			return m_StrongReferences;
		}
	private:
		uint32 m_StrongReferences;
	};

	//----------
	//CSharedRef
	//----------

	//Wrapper for an ISharedObject that releases it when destroyed (out of scope)
	//Similar to Microsoft's Microsoft::WRL::ComPtr class
	template <typename TObject>
	class CSharedRef
	{
	public:
		_forceinline CSharedRef(TObject* pObject = nullptr)
			: m_pObject(pObject)
		{
		}

		_forceinline CSharedRef(const CSharedRef& other)
			: m_pObject(nullptr)
		{
			*this = other;
		}

		_forceinline CSharedRef(CSharedRef&& other)
			: m_pObject(nullptr)
		{
			*this = other;
		}

		_forceinline ~CAutoRef()
		{
            SafeRelease(m_pObject);
		}

		_forceinline TObject* Get() const
		{
			return m_pObject;
		}
        
        template<typename T>
		_forceinline T* GetAs() const
        {
            return reinterpret_cast<T*>(m_pObject);
        }

		_forceinline TObject** GetAdressOf()
		{
			return &m_pObject;
		}

		_forceinline TObject* const * GetAdressOf() const
		{
			return &m_pObject;
		}

		_forceinline uint32 Release()
		{
			uint32 counter = 0;
			if (m_pObject)
			{
				counter = m_pObject->Release();
				m_pObject = nullptr;
			}
			return counter;
		}

		_forceinline TObject* operator->() const
		{
			return m_pObject;
		}

		_forceinline TObject& operator*()
		{
			LAMBDA_ASSERT(m_pObject != nullptr);
			return *m_pObject;
		}

		_forceinline const TObject& operator*() const
		{
			LAMBDA_ASSERT(m_pObject != nullptr);
			return *m_pObject;
		}

		_forceinline TObject** operator&()
		{
			return &m_pObject;
		}

		_forceinline TObject* const * operator&() const
		{
			return &m_pObject;
		}

		_forceinline CSharedRef& operator=(TObject* pObject)
		{
			AutoRef other(pObject);
			return *this = other;
		}

		_forceinline CSharedRef& operator=(const CSharedRef& other)
		{
			if (m_pObject)
			{
				m_pObject->Release();
			}

			m_pObject = other.m_pObject;
			m_pObject->AddRef();

			return *this;
		}

		_forceinline CSharedRef& operator=(CSharedRef&& other)
		{
			if (m_pObject)
			{
				m_pObject->Release();
			}
				 
			m_pObject = other.m_pObject;
			other.m_pObject = nullptr;

			return *this;
		}

		_forceinline operator bool()
		{
			return m_pObject != nullptr;
		}
	private:
		TObject* m_pObject;
	};
}
