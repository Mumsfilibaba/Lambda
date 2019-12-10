#include "LambdaPch.h"
#include "Core/CLayer.h"

namespace Lambda
{
    //-----
    //CLayer
    //-----

    CLayer::CLayer(const char* pName)
        : m_pName(pName)
    {
    }


    void CLayer::OnLoad()
    {
    }


    void CLayer::OnUpdate(const CTime& dt)
    {
    }


    void CLayer::OnRender(Renderer3D& renderer, const CTime& dt)
    {
    }


    void CLayer::OnRenderUI(const CTime& dt)
    {
    }


    void CLayer::OnRelease()
    {
    }


    const char* CLayer::GetName() const
    {
        return m_pName;
    }
}
