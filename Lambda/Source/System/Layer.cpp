#include "LambdaPch.h"
#include "System/Layer.h"

namespace Lambda
{
    //-----
    //Layer
    //-----

    Layer::Layer(const char* pName)
        : m_pName(pName)
    {
    }


    void Layer::OnLoad()
    {
    }


    void Layer::OnUpdate(Timestep dt)
    {
    }


    void Layer::OnRender(Timestep dt, ICommandList* pCurrentList)
    {
    }


    void Layer::OnRenderUI(Timestep dt)
    {
    }


    void Layer::OnRelease()
    {
    }


    const char* Layer::GetName() const
    {
        return m_pName;
    }
}
