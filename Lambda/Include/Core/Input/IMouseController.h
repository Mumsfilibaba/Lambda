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

	//-----------------------------------------------------------------
	//CDummyMouseController - Used when no real controller is available
	//-----------------------------------------------------------------

	class CDummyMouseController final : public IMouseController
	{
	public:
		CDummyMouseController() = default;
		~CDummyMouseController() = default;

		virtual bool IsButtonup(EMouseButton) const override final
		{
			return false;
		}

		virtual bool IsButtonDown(EMouseButton) const override final
		{
			return false;
		}

		virtual void SetPosition(const Point&) override final
		{
		}

		virtual Point GetPosition() const override final
		{
			return Point(0, 0);
		}

		virtual CMouseState GetState() const override final
		{
			return CMouseState();
		}
	};
}
