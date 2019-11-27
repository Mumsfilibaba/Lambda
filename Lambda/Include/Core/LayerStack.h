#pragma once
#include "LambdaCore.h"
#include "Utilities/Singleton.h"
#include <vector>

namespace Lambda
{
    class Layer;

    //----------
    //LayerStack
    //----------

    class LAMBDA_API LayerStack
    {
    public:
        LAMBDA_NO_COPY(LayerStack);
        
        LayerStack() = default;
        ~LayerStack() = default;
        
        void ReleaseLayers();
        void PushLayer(Layer* pLayer);
        void PopLayer();

		//Enables foreachloops
		_forceinline std::vector<Layer*>::iterator begin()
		{
			return m_Layers.begin();
		}


		_forceinline std::vector<Layer*>::iterator end()
		{
			return m_Layers.end();
		}
	private:
		void ReleaseLayer(Layer* pLayer);
    private:
        std::vector<Layer*> m_Layers;
    };
}
