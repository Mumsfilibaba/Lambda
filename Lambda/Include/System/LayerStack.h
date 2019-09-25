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
        std::vector<Layer*>::iterator Begin();
        std::vector<Layer*>::iterator End();
    private:
        std::vector<Layer*> m_Layers;
    };
}
