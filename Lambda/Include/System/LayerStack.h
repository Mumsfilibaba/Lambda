#pragma once
#include "Defines.h"
#include "Types.h"
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
        
        void Init();
        void Release();
        void PushLayer(Layer* pLayer);
        void PopLayer();

		//Enables foreachloops
		_forceinline std::vector<Layer*>::iterator LayerStack::Begin()
		{
			return m_Layers.begin();
		}


		_forceinline std::vector<Layer*>::iterator LayerStack::End()
		{
			return m_Layers.end();
		}
    private:
        std::vector<Layer*> m_Layers;
    };
}
