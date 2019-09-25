#include "LambdaPch.h"
#include "System/LayerStack.h"
#include "System/Layer.h"

namespace Lambda
{
    void LayerStack::Init()
    {
        //Nothing for now
    }


    void LayerStack::Release()
    {
        for (auto pLayer : m_Layers)
            SafeDelete(pLayer);
    }


    void LayerStack::PushLayer(Layer* pLayer)
    {
        LAMBDA_ASSERT(pLayer != nullptr);
        
        m_Layers.emplace_back(pLayer);
        LOG_DEBUG_INFO("Lambda Engine: Pushed eventlayer %d '%s'.\n", m_Layers.size(), pLayer->GetName());
    }


    void LayerStack::PopLayer()
    {
        Layer* pLayer = m_Layers.back();
        m_Layers.pop_back();
        
        LOG_DEBUG_INFO("Lambda Engine: Poped eventlayer '%s'.\n", pLayer->GetName());
        SafeDelete(pLayer);
    }


    std::vector<Layer*>::iterator LayerStack::Begin()
    {
        return m_Layers.begin();
    }


    std::vector<Layer*>::iterator LayerStack::End()
    {
        return m_Layers.end();
    }
}
