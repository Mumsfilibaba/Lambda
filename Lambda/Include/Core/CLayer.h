#pragma once
#include "Time/Timestep.h"

namespace Lambda
{
	class Renderer3D;
	class IDeviceContext;
    
    //------
    //CLayer
    //------

    class LAMBDA_API CLayer
    {
    public:
        CLayer(const char* pName);
        virtual ~CLayer() = default;

        LAMBDA_NO_COPY(CLayer);

        virtual void OnLoad();
        virtual void OnUpdate(Timestep dt);
        virtual void OnRender(Renderer3D& renderer, Timestep dt);
        virtual void OnRenderUI(Timestep dt);
        virtual void OnRelease();        
        const char* GetName() const;
    private:
        const char* m_pName;
    };
}
