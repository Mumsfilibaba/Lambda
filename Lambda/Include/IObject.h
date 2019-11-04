#pragma once
#include "Defines.h"
#include "Types.h"

namespace Lambda
{
	typedef uint32 RefCountValue;

    //-------
    //IObject
    //-------
    
	//Interface for referencecounted objects. Classes that inherits from IObject should not
	//have a virtual constructor since they should be deleted with the Release-function
	class LAMBDA_API IObject
	{
	public:
		LAMBDA_INTERFACE(IObject);

		virtual RefCountValue Release() = 0;
		virtual RefCountValue AddRef()  = 0;
	};

	//----------------
	//RefCountedObject
	//----------------

	//Base class for all referencecounted objects. Keeps track of all strong references.
	//Base should be the base interface. For example 'class VKNDevice : public RefCountedObject<IDevice>'
	template<typename Base>
	class RefCountedObject : public Base
	{
	public:
		LAMBDA_NO_COPY(RefCountedObject);

		RefCountedObject() : Base(), m_StrongReferences(0) {}
		virtual ~RefCountedObject() = default;


		virtual RefCountValue Release() override
		{
			RefCountValue counter = --m_StrongReferences;
			if (counter < 1)
				delete this;

			return counter;
		}


		virtual RefCountValue AddRef() override
		{
			return ++m_StrongReferences;
		}
	private:
		RefCountValue m_StrongReferences;
	};

	//-------
	//AutoRef
	//-------

	//Wrapper for an IObject that releases it when destroyed (out of scope)
	//Similar to Microsoft's Microsoft::WRL::ComPtr class
	template <typename TObject>
	class AutoRef
	{
	public:
		_forceinline AutoRef(TObject* pObject = nullptr)
			: m_pObject(pObject)
		{
		}


		_forceinline AutoRef(const AutoRef& other)
			: m_pObject(nullptr)
		{
			*this = other;
		}


		_forceinline AutoRef(AutoRef&& other)
			: m_pObject(nullptr)
		{
			*this = other;
		}


		_forceinline ~AutoRef()
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


		_forceinline RefCountValue Release()
		{
			RefCountValue counter = 0;
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


		_forceinline AutoRef& operator=(TObject* pObject)
		{
			AutoRef other(pObject);
			return *this = other;
		}


		_forceinline AutoRef& operator=(const AutoRef& other)
		{
			if (m_pObject)
				m_pObject->Release();

			m_pObject = other.m_pObject;
			m_pObject->AddRef();

			return *this;
		}


		_forceinline AutoRef& operator=(AutoRef&& other)
		{
			if (m_pObject)
				m_pObject->Release();
				 
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
