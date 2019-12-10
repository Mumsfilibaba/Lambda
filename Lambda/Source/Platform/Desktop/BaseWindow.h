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
        CBaseWindow() = default;
        virtual ~CBaseWindow() = default;
        
        virtual bool HasFocus() const override = 0;
        virtual uint32 GetHeight() const override = 0;
        virtual uint32 GetWidth() const override = 0;
        
        virtual void SetEventCallback(IEventCallback* pEventCallback) override
        {
            SafeDelete(m_pEventCallback);
            m_pEventCallback = pEventCallback;
        }
    protected:
        bool DispatchEvent(const CEvent& event)
        {
            if (m_pEventCallback)
                return m_pEventCallback->Call(event);
            
            return false;
        }
    protected:
        IEventCallback* m_pEventCallback;
    };
}
