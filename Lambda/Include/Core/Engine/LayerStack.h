#pragma once
#include "LambdaCore.h"

#include "Layer.h"
#include "Core/Event/ISystemEventListener.h"

#include <vector>

namespace Lambda
{
	//LayerStack
	class LayerStack final : public ISystemEventListener
	{
	public:
		LayerStack();
		~LayerStack() = default;

		/*ISysemEventListener Interface*/
		virtual bool OnSystemEvent(const SystemEvent& event) override final;

		/*LayerStack Interface*/
		void PushLayer(Layer* player);
		void PopLayer();
		void ReleaseLayers();
		
		void OnUpdate(const Time& time);
	private:
		std::vector<Layer*> m_Layers;
	};
}