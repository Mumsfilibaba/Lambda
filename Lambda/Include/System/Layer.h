#pragma once
#include "Time/Timestep.h"

namespace Lambda
{
    class Event;
    
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
        virtual void OnRender(Timestep dt);
        virtual void OnRenderUI(Timestep dt);
        virtual void OnRelease();
        virtual bool OnEvent(const Event& event) = 0;
        virtual uint32 GetRecivableCategories() const = 0;
        
        const char* GetName() const;
    private:
        const char* m_pName;
    };
}
