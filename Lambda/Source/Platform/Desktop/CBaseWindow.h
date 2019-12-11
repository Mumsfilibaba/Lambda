#include "IWindow.h"
#include "Core/Event/IEventCallback.h"

namespace Lambda
{
    //-----------
    //CBaseWindow
    //-----------

    class CBaseWindow : public IWindow
    {
    public:
        CBaseWindow();
        virtual ~CBaseWindow();
        
        virtual bool HasFocus() const override = 0;
        virtual uint32 GetHeight() const override = 0;
        virtual uint32 GetWidth() const override = 0;
		virtual void SetEventCallback(IEventCallback* pEventCallback) override;
    protected:
		virtual bool DispatchEvent(const CEvent& event);
    protected:
        IEventCallback* m_pEventCallback;
    };
}
