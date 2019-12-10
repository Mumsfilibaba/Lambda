#pragma once
#include "LambdaCore.h"
#include <vector>

namespace Lambda
{
    class CLayer;

    //-----------
    //CLayerStack
    //-----------

    class LAMBDA_API CLayerStack
    {
    public:
        CLayerStack() = default;
        ~CLayerStack() = default;

        LAMBDA_NO_COPY(CLayerStack);
        
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
