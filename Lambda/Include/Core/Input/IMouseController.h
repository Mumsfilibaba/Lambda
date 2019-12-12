#pragma once
#include "CMouseState.h"

namespace Lambda
{
    //----------------
    //IMouseController
    //----------------

	class IMouseController
    {
    public:
        LAMBDA_INTERFACE(IMouseController);
        
		virtual bool IsButtonup(EMouseButton button) const = 0;
		virtual bool IsButtonDown(EMouseButton button) const = 0;
        virtual void SetPosition(const Point& position) = 0;
        virtual Point GetPosition() const = 0;
        virtual CMouseState GetState() const = 0;
    };
}
