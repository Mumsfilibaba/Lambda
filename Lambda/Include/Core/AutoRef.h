#pragma once
#include "IObject.h"

namespace Lambda
{
    //-------
    //AutoRef
    //-------

    //Wrapper for an IObject that releases it when destroyed (out of scope)
    //Similar to Microsoft's Microsoft::WRL::ComPtr class
    template <typename T>
    class AutoRef
    {
    public:
        _forceinline AutoRef(T* pObject = nullptr)
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


        _forceinline T* Get() const
        {
            return m_pObject;
        }
        
        
        template<typename U>
        _forceinline U* GetAs() const
        {
            return static_cast<U*>(m_pObject);
        }


        _forceinline T** GetAdressOf()
        {
            return &m_pObject;
        }


        _forceinline T* const * GetAdressOf() const
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


        _forceinline T* operator->() const
        {
            return m_pObject;
        }


        _forceinline T& operator*()
        {
            LAMBDA_ASSERT(m_pObject != nullptr);
            return *m_pObject;
        }


        _forceinline const T& operator*() const
        {
            LAMBDA_ASSERT(m_pObject != nullptr);
            return *m_pObject;
        }


        _forceinline T** operator&()
        {
            return &m_pObject;
        }


        _forceinline T* const * operator&() const
        {
            return &m_pObject;
        }


        _forceinline AutoRef& operator=(T* pObject)
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
        T* m_pObject;
    };
}
