#include "LambdaPch.h"
#include "Core/Application.h"

namespace Lambda
{
    //------------
    //CApplication
    //------------

    template<>
    CApplication* CSingleton<CApplication>::s_pInstance = nullptr;

    CApplication::CApplication()
        : CSingleton(),
        m_EventCallback(nullptr)
    {
    }


    CApplication::~CApplication()
    {
    }


    void CApplication::Release()
    {
        delete this;
    }


    void CApplication::DispatchEvent(const SSystemEvent& event)
	{
        m_EventCallback(event);
	}
}
