#pragma once
#include "Time/Timestep.h"

namespace Lambda
{
    class Event;
	class Renderer3D;
	class IDeviceContext;
    
    //-----
    //Layer
    //-----

    class LAMBDA_API Layer
    {
    public:
        LAMBDA_NO_COPY(Layer);

        Layer(const char* pName);
        virtual ~Layer() = default;

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
