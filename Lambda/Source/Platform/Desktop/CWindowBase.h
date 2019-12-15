#include "IWindow.h"
#include "Core/Event/IEventCallback.h"

namespace Lambda
{
    //-----------
    //CWindowBase
    //-----------

    class CWindowBase : public IWindow
    {
    public:
        CWindowBase();
        virtual ~CWindowBase();
        
        virtual bool HasFocus() const override = 0;
        virtual uint32 GetHeight() const override = 0;
        virtual uint32 GetWidth() const override = 0;
		virtual void SetEventCallback(IEventCallback* pEventCallback) override;
    protected:
		virtual bool DispatchEvent(const CEvent& event);
    private:
        IEventCallback* m_pEventCallback;
    };
}
