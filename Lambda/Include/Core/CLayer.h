#pragma once
#include "Time/CTime.h"

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
        virtual void OnUpdate(const CTime& time);
        virtual void OnRender(Renderer3D& renderer, const CTime& time);
        virtual void OnRenderUI(const CTime& time);
        virtual void OnRelease();        
        const char* GetName() const;
    private:
        const char* m_pName;
    };
}
