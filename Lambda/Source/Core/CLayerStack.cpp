#include "LambdaPch.h"
#include "Core/CLayerStack.h"
#include "Core/CLayer.h"

namespace Lambda
{
	//----------–
	//CLayerStack
	//-----------

    void CLayerStack::ReleaseLayers()
    {
		for (auto pLayer : m_Layers)
			ReleaseLayer(pLayer);
    }


    void CLayerStack::PushLayer(CLayer* pLayer)
    {
        LAMBDA_ASSERT(pLayer != nullptr);
        
        m_Layers.emplace_back(pLayer);
		LOG_ENGINE_INFO("Pushed Layer %d '%s'.\n", m_Layers.size(), pLayer->GetName());
    }


    void CLayerStack::PopLayer()
    {
        Layer* pLayer = m_Layers.back();
        m_Layers.pop_back();
        
		LOG_ENGINE_INFO("Poped Layer '%s'.\n", pLayer->GetName());
		
		ReleaseLayer(pLayer);
    }
	
	
	void CLayerStack::ReleaseLayer(Layer* pLayer)
	{
		pLayer->OnRelease();
	}
}
