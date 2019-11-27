#include "LambdaPch.h"
#include "Core/LayerStack.h"
#include "Core/Layer.h"

namespace Lambda
{
	//----------
	//LayerStack
	//----------

    void LayerStack::ReleaseLayers()
    {
		for (auto pLayer : m_Layers)
			ReleaseLayer(pLayer);
    }


    void LayerStack::PushLayer(Layer* pLayer)
    {
        LAMBDA_ASSERT(pLayer != nullptr);
        
        m_Layers.emplace_back(pLayer);
		LOG_ENGINE_INFO("Pushed Layer %d '%s'.\n", m_Layers.size(), pLayer->GetName());
    }


    void LayerStack::PopLayer()
    {
        Layer* pLayer = m_Layers.back();
        m_Layers.pop_back();
        
		LOG_ENGINE_INFO("Poped Layer '%s'.\n", pLayer->GetName());
		
		ReleaseLayer(pLayer);
    }
	
	
	void LayerStack::ReleaseLayer(Layer* pLayer)
	{
		pLayer->OnRelease();
	}
}