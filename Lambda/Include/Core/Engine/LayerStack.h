#pragma once
#include "LambdaCore.h"
#include "Layer.h"

#include "Core/Event/ISystemEventListener.h"

#include <vector>

namespace Lambda
{
	//-----------
	//CLayerStack
	//-----------

	class CLayerStack final : public ISystemEventListener
	{
	private:
		CLayerStack();
	public:
		~CLayerStack();

		void PushLayer(CLayer* player);
		void PopLayer();
		void ReleaseLayers();
		
		void OnUpdate(const CTime& time);

		static CLayerStack* Create();

		/*ISysemEventListener Interface*/
		virtual bool OnSystemEvent(const SSystemEvent& event) override final;
	private:
		std::vector<CLayer*> m_Layers;
	};
}