#include "LambdaPch.h"
#include "Core/CLayer.h"

namespace Lambda
{
    //------
    //CLayer
    //------

    CLayer::CLayer(const char* pName)
        : m_pName(pName)
    {
    }


    void CLayer::OnLoad()
    {
    }


    void CLayer::OnUpdate(const CTime&)
    {
    }


    void CLayer::OnRender(Renderer3D&, const CTime&)
    {
    }


    void CLayer::OnRenderUI(const CTime&)
    {
    }


    void CLayer::OnRelease()
    {
		delete this;
    }


    const char* CLayer::GetName() const
    {
        return m_pName;
    }
}
