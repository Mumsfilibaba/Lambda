#include "LambdaPch.h"

#if defined(LAMBDA_PLAT_MACOS)
    #include "Core/Event/Event.h"
    #include "Core/Event/IEventListener.h"

    #include "Core/Debug/IConsoleOutput.h"

    #include "Platform/macOS/MacWindow.h"

    #include <Cocoa/Cocoa.h>

namespace Lambda
{
    CMacWindow::CMacWindow(const SWindowDesc& desc)
        : IWindow(),
        m_pView(nullptr),
        m_pCocoaWindow(nullptr),
        m_Width(0),
        m_Height(0),
        m_bIsMiniaturized(false)
    {
        Init(desc);
    }
    
    CMacWindow::~CMacWindow()
    {
        if (m_pCocoaWindow)
        {
            [m_pCocoaWindow dealloc];
            m_pCocoaWindow = nullptr;
        }
        
        if (m_pView)
        {
            [m_pView release];
            m_pView = nullptr;
        }
    }
    
    void CMacWindow::Init(const SWindowDesc& desc)
    {
        //Create a native cocoa window
        NSRect contentRect = NSMakeRect(0, 0, desc.Width, desc.Height);
        NSUInteger styleMask = NSWindowStyleMaskMiniaturizable | NSWindowStyleMaskTitled | NSWindowStyleMaskClosable | NSWindowStyleMaskResizable;
        CCocoaWindow* pCocoaWindow = [[CCocoaWindow alloc] initWithContentRect:this :contentRect styleMask:styleMask backing:NSBackingStoreBuffered defer:NO];
        m_pCocoaWindow = pCocoaWindow;
        
        NSString* pNSTitle = [NSString stringWithUTF8String:desc.pTitle];
        [m_pCocoaWindow setTitle:pNSTitle];
        
        const NSWindowCollectionBehavior behavior = NSWindowCollectionBehaviorFullScreenPrimary | NSWindowCollectionBehaviorManaged;
        [m_pCocoaWindow setCollectionBehavior:behavior];
        
        [m_pCocoaWindow setAcceptsMouseMovedEvents:YES];
        [m_pCocoaWindow setRestorable:NO];

        CCocoaView* pView = [[CCocoaView alloc] initWithWindow:this];
        m_pView = pView;
        
        [m_pCocoaWindow setContentView:m_pView];
        [m_pCocoaWindow makeFirstResponder:m_pView];
    }
    
    bool CMacWindow::DispatchEvent(const SEvent& event)
    {
        for (IEventListener* pEventListener : m_EventListeners)
        {
            if (pEventListener->OnEvent(event))
                return true;
        }
        
        return false;
    }
    
    void CMacWindow::Show()
    {
        [m_pCocoaWindow makeKeyAndOrderFront:nil];
        [m_pCocoaWindow center];
    }
    
    void CMacWindow::AddEventListener(IEventListener* pEventListener)
    {
        m_EventListeners.emplace_back(pEventListener);
    }
    
    void CMacWindow::RemoveEventListener(IEventListener* pEventListener)
    {
        for (auto it = m_EventListeners.begin(); it != m_EventListeners.end(); it++)
        {
            if (*it == pEventListener)
            {
                m_EventListeners.erase(it);
                break;
            }
        }
    }
    
    void CMacWindow::OnClose()
    {
        SEvent event = {};
        event.EventType = EEventType::EVENT_TYPE_WINDOW_CLOSED;
        DispatchEvent(event);
    }
    
    void CMacWindow::OnResize(uint32 nWidth, uint32 nHeight)
    {
        if (nWidth != m_Width && nHeight != m_Height)
        {
            //Set members
            m_Height = nHeight;
            m_Width  = nWidth;
            
            //Send event
            SEvent event = {};
            event.EventType = EEventType::EVENT_TYPE_WINDOW_RESIZE;
            event.WindowResizeEvent.ResizeType = EResizeType::RESIZE_TYPE_STANDARD;
            event.WindowResizeEvent.Width      = nWidth;
            event.WindowResizeEvent.Height     = nHeight;
            DispatchEvent(event);
        }
    }
    
    void CMacWindow::OnMiniaturized(uint32 nWidth, uint32 nHeight, bool bMiniturized)
    {
        if (m_bIsMiniaturized != bMiniturized)
        {
            m_bIsMiniaturized = bMiniturized;
            m_Width  = nWidth;
            m_Height = nHeight;
            
            SEvent event = {};
            event.EventType = EEventType::EVENT_TYPE_WINDOW_RESIZE;
            event.WindowResizeEvent.ResizeType = bMiniturized ? EResizeType::RESIZE_TYPE_MINIMIZE : EResizeType::RESIZE_TYPE_MAXIMIZE;
            event.WindowResizeEvent.Width      = m_Width;
            event.WindowResizeEvent.Height     = m_Height;
            DispatchEvent(event);
        }
    }
    
    void CMacWindow::OnToggleFullscreen(bool)
    {
        //TODO: Fix this when we add fullscreen ability to windows
    }

    void CMacWindow::OnMove(uint32 x, uint32 y)
    {
        SEvent event = {};
        event.EventType = EEventType::EVENT_TYPE_WINDOW_MOVE;
        event.WindowMoveEvent.x = x;
        event.WindowMoveEvent.y = y;
        DispatchEvent(event);
    }
    
    void CMacWindow::OnFocusChanged(bool bHasFocus)
    {
        SEvent event = {};
        event.EventType = EEventType::EVENT_TYPE_WINDOW_FOCUS;
        event.WindowFocusEvent.bHasFocus = bHasFocus;
        DispatchEvent(event);
    }
    
    void CMacWindow::OnKeydown(EKey keycode, uint32 modifiers)
    {
        SEvent event = {};
        event.EventType = EEventType::EVENT_TYPE_KEY_PRESSED;
        event.KeyEvent.Key         = keycode;
        event.KeyEvent.Modifiers   = modifiers;
        event.KeyEvent.RepeatCount = 0;
        DispatchEvent(event);
    }
    
    void CMacWindow::OnKeyup(EKey keycode, uint32 modifiers)
    {
        SEvent event = {};
        event.EventType = EEventType::EVENT_TYPE_KEY_RELEASED;
        event.KeyEvent.Key         = keycode;
        event.KeyEvent.Modifiers   = modifiers;
        event.KeyEvent.RepeatCount = 0;
        DispatchEvent(event);
    }
    
    void CMacWindow::OnKeyTyped(uint32 character)
    {
        SEvent event = {};
        event.EventType = EEventType::EVENT_TYPE_KEY_TYPED;
        event.KeyTypedEvent.Character = character;
        DispatchEvent(event);
    }
    
    void CMacWindow::OnMouseButtonDown(EMouseButton button, uint32 modifiers)
    {
        SEvent event = {};
        event.EventType = EEventType::EVENT_TYPE_MOUSE_BUTTON_PRESSED;
        event.MouseButtonEvent.Button    = button;
        event.MouseButtonEvent.Modifiers = modifiers;
        DispatchEvent(event);
    }

    void CMacWindow::OnMouseButtonUp(EMouseButton button, uint32 modifiers)
    {
        SEvent event = {};
        event.EventType = EEventType::EVENT_TYPE_MOUSE_BUTTON_RELEASED;
        event.MouseButtonEvent.Button    = button;
        event.MouseButtonEvent.Modifiers = modifiers;
        DispatchEvent(event);
    }

    void CMacWindow::OnMouseMove(uint32 x, uint32 y)
    {
        SEvent event = {};
        event.EventType = EEventType::EVENT_TYPE_MOUSE_MOVED;
        event.MouseMoveEvent.x = x;
        event.MouseMoveEvent.y = y;
        DispatchEvent(event);
    }

    void CMacWindow::OnMouseScroll(float horizontalValue, float verticalValue)
    {
        SEvent event = {};
        event.EventType = EEventType::EVENT_TYPE_MOUSE_SCROLLED;
        event.MouseScrolledEvent.Horizontal = horizontalValue;
        event.MouseScrolledEvent.Vertical   = verticalValue;
        DispatchEvent(event);
    }
}
#endif
