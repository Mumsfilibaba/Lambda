#include "LambdaPch.h"

#include "Core/Engine/LayerStack.h"

namespace Lambda
{
	//-----------
	//CLayerStack
	//-----------

	/*////////////////////////////////////////////////////////////////////////////////////////////////*/
	CLayerStack::CLayerStack()
		: ISystemEventListener()
	{
	}

	/*////////////////////////////////////////////////////////////////////////////////////////////////*/
	CLayerStack::~CLayerStack()
	{
		ReleaseLayers();
	}

	/*////////////////////////////////////////////////////////////////////////////////////////////////*/
	bool CLayerStack::OnSystemEvent(const SSystemEvent& event)
	{
		for (CLayer* pLayer : m_Layers)
		{
			if (pLayer->OnSystemEvent(event))
				return true;
		}

		return false;
	}

	/*////////////////////////////////////////////////////////////////////////////////////////////////*/
	void CLayerStack::PushLayer(CLayer* pLayer)
	{
		if (pLayer)
		{
			pLayer->OnAttach();
			m_Layers.emplace_back(pLayer);
		}
	}

	/*////////////////////////////////////////////////////////////////////////////////////////////////*/
	void CLayerStack::PopLayer()
	{
		CLayer* pLayer = m_Layers.back();
		pLayer->OnDetach();	
		SafeDelete(pLayer);

		m_Layers.pop_back();
	}

	/*////////////////////////////////////////////////////////////////////////////////////////////////*/
	void CLayerStack::ReleaseLayers()
	{
		size_t count = m_Layers.size();
		for (size_t i = 0; i < count; i++)
			PopLayer();
	}

	/*////////////////////////////////////////////////////////////////////////////////////////////////*/
	void CLayerStack::OnUpdate(const CTime& time)
	{
		for (CLayer* pLayer : m_Layers)
			pLayer->OnUpdate(time);
	}
	
	/*////////////////////////////////////////////////////////////////////////////////////////////////*/
	CLayerStack* CLayerStack::Create()
	{
		return DBG_NEW CLayerStack();
	}
}