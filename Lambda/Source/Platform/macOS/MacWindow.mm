#import "LambdaPch.h"

#if defined(LAMBDA_PLAT_MACOS)
    #import "Core/Engine/Console.h"
    
    #import "Platform/macOS/MacWindow.h"
    #import "Platform/macOS/MacSystem.h"

    #import <Foundation/Foundation.h>

namespace Lambda
{
    //----------
    //CMacWindow
    //----------
    
    /*////////////////////////////////////////////////////////////////////////////////////////////////*/
    CMacWindow::CMacWindow(CMacSystem* pSystem, const char* pTitle, uint32 nWidth, uint32 nHeight)
        : IWindow(),
        m_pSystem(pSystem),
        m_pView(nullptr),
        m_pCocoaWindow(nullptr),
        m_nWidth(0),
        m_nHeight(0),
        m_bHasFocus(false),
        m_bIsMiniaturized(false)
    {
        CConsole::PrintLine("Created window");
        
        //Create a native cocoa window
        NSRect contentRect = NSMakeRect(0, 0, nWidth, nHeight);
        NSUInteger styleMask = NSWindowStyleMaskMiniaturizable | NSWindowStyleMaskTitled | NSWindowStyleMaskClosable | NSWindowStyleMaskResizable;
        CCocoaWindow* pCocoaWindow = [[CCocoaWindow alloc] initWithContentRect:this :contentRect styleMask:styleMask backing:NSBackingStoreBuffered defer:NO];
        m_pCocoaWindow = pCocoaWindow;
        
        //Finish window
        NSString* pNSTitle = [NSString stringWithUTF8String:pTitle];
        [m_pCocoaWindow setTitle:pNSTitle];
        
        const NSWindowCollectionBehavior behavior = NSWindowCollectionBehaviorFullScreenPrimary | NSWindowCollectionBehaviorManaged;
        [m_pCocoaWindow setCollectionBehavior:behavior];
        
        [m_pCocoaWindow setAcceptsMouseMovedEvents:YES];
        [m_pCocoaWindow setRestorable:NO];

        //Create view
        CCocoaView* pView = [[CCocoaView alloc] initWithWindow:this];
        m_pView = pView;
        
        [m_pCocoaWindow setContentView:m_pView];
        [m_pCocoaWindow makeFirstResponder:m_pView];
        
        //Show window
        [m_pCocoaWindow makeKeyAndOrderFront:nil];
        [m_pCocoaWindow center];
    }
    
    /*////////////////////////////////////////////////////////////////////////////////////////////////*/
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
    
    /*////////////////////////////////////////////////////////////////////////////////////////////////*/
    void CMacWindow::SetFullscreen(bool bIsFullscreen)
    {
        if (CMacWindow::IsFullscreen() != bIsFullscreen)
        {
            [m_pCocoaWindow toggleFullScreen:nil];
        }
    }
    
    /*////////////////////////////////////////////////////////////////////////////////////////////////*/
    void CMacWindow::OnClose()
    {
        m_pSystem->OnWindowClose(this);
    }
    
    /*////////////////////////////////////////////////////////////////////////////////////////////////*/
    void CMacWindow::OnResize(uint32 nWidth, uint32 nHeight)
    {
        if (nWidth != m_nWidth && nHeight != m_nHeight)
        {
            //Set members
            m_nHeight = nHeight;
            m_nWidth  = nWidth;
            
            //Send event
            SSystemEvent event = {};
            event.EventType = ESystemEvent::SYSTEM_EVENT_WINDOW_RESIZED;
            event.WindowResizedEvent.ResizeType = EResizeType::RESIZE_TYPE_STANDARD;
            event.WindowResizedEvent.Width      = nWidth;
            event.WindowResizedEvent.Height     = nHeight;
            m_pSystem->OnSystemEvent(event);
            
            CConsole::PrintLine("width=%d, height=%d", nWidth, nHeight);
        }
    }
    
    /*////////////////////////////////////////////////////////////////////////////////////////////////*/
    void CMacWindow::OnMiniaturized(uint32 nWidth, uint32 nHeight, bool bMiniturized)
    {
        if (m_bIsMiniaturized != bMiniturized)
        {
            //Set members
            m_bIsMiniaturized = bMiniturized;
            m_nWidth  = nWidth;
            m_nHeight = nHeight;
            
            //Send event
            SSystemEvent event = {};
            event.EventType = ESystemEvent::SYSTEM_EVENT_WINDOW_RESIZED;
            event.WindowResizedEvent.ResizeType = bMiniturized ? EResizeType::RESIZE_TYPE_MINIMIZED : EResizeType::RESIZE_TYPE_MAXIMIZED;
            event.WindowResizedEvent.Width      = m_nWidth;
            event.WindowResizedEvent.Height     = m_nHeight;
            m_pSystem->OnSystemEvent(event);
            
            CConsole::PrintLine("bMiniturized=%s width=%d height=%d", bMiniturized ? "true" : "false", nWidth, nHeight);
        }
    }
    
    /*////////////////////////////////////////////////////////////////////////////////////////////////*/
    void CMacWindow::OnToggleFullscreen(bool bFullscreen)
    {
        if (m_bIsFullscreen != bFullscreen)
        {
            m_bIsFullscreen = bFullscreen;
            CConsole::PrintLine("fullscreen=%s", bFullscreen ? "true" : "false");
        }
    }

    /*////////////////////////////////////////////////////////////////////////////////////////////////*/
    void CMacWindow::OnMove(uint32 x, uint32 y)
    {
        if (m_Position.x != x && m_Position.y != y)
        {
            //Set members
            m_Position.x = x;
            m_Position.y = y;
            
            //Send event
            SSystemEvent event = {};
            event.EventType = ESystemEvent::SYSTEM_EVENT_WINDOW_MOVED;
            event.WindowMovedEvent.x = x;
            event.WindowMovedEvent.y = y;
            m_pSystem->OnSystemEvent(event);
        }
    }
    
    /*////////////////////////////////////////////////////////////////////////////////////////////////*/
    void CMacWindow::OnFocusChanged(bool bHasFocus)
    {
        if (m_bHasFocus != bHasFocus)
        {
            //Set member
            m_bHasFocus = bHasFocus;
            
            //Send event
            SSystemEvent event = {};
            event.EventType = ESystemEvent::SYSTEM_EVENT_WINDOW_FOCUS_CHANGED;
            event.WindowFocusChangedEvent.bHasFocus = bHasFocus;
            m_pSystem->OnSystemEvent(event);
        }
    }
    
    /*////////////////////////////////////////////////////////////////////////////////////////////////*/
    void CMacWindow::OnKeydown(EKey keycode, uint32 modifiers)
    {
        //Send event
        SSystemEvent event = {};
        event.EventType = ESystemEvent::SYSTEM_EVENT_KEY_PRESSED;
        event.KeyEvent.Key         = keycode;
        event.KeyEvent.Modifiers   = modifiers;
        event.KeyEvent.RepeatCount = 0;
        m_pSystem->OnSystemEvent(event);
    }
    
    /*////////////////////////////////////////////////////////////////////////////////////////////////*/
    void CMacWindow::OnKeyup(EKey keycode, uint32 modifiers)
    {
        //Send event
        SSystemEvent event = {};
        event.EventType = ESystemEvent::SYSTEM_EVENT_KEY_RELEASED;
        event.KeyEvent.Key         = keycode;
        event.KeyEvent.Modifiers   = modifiers;
        event.KeyEvent.RepeatCount = 0;
        m_pSystem->OnSystemEvent(event);
    }
    
    /*////////////////////////////////////////////////////////////////////////////////////////////////*/
    void CMacWindow::OnKeyTyped(uint32 character)
    {
        //Send event
        SSystemEvent event = {};
        event.EventType = ESystemEvent::SYSTEM_EVENT_KEY_TEXT;
        event.KeyTextEvent.Character = character;
        m_pSystem->OnSystemEvent(event);
        
        CConsole::PrintLine("Key typed %c", char(character));
    }
    
    /*////////////////////////////////////////////////////////////////////////////////////////////////*/
    void CMacWindow::OnMouseButtonDown(EMouseButton button, uint32 modifiers)
    {
        //Send event
        SSystemEvent event = {};
        event.EventType = ESystemEvent::SYSTEM_EVENT_MOUSE_BUTTON_PRESSED;
        event.MouseButtonEvent.Button    = button;
        event.MouseButtonEvent.Modifiers = modifiers;
        m_pSystem->OnSystemEvent(event);
    }

    /*////////////////////////////////////////////////////////////////////////////////////////////////*/
    void CMacWindow::OnMouseButtonUp(EMouseButton button, uint32 modifiers)
    {
        //Send event
        SSystemEvent event = {};
        event.EventType = ESystemEvent::SYSTEM_EVENT_MOUSE_BUTTON_RELEASED;
        event.MouseButtonEvent.Button    = button;
        event.MouseButtonEvent.Modifiers = modifiers;
        m_pSystem->OnSystemEvent(event);
    }

    /*////////////////////////////////////////////////////////////////////////////////////////////////*/
    void CMacWindow::OnMouseMove(uint32 x, uint32 y)
    {
        //Send event
        SSystemEvent event = {};
        event.EventType = ESystemEvent::SYSTEM_EVENT_MOUSE_MOVED;
        event.MouseMovedEvent.x = x;
        event.MouseMovedEvent.y = y;
        m_pSystem->OnSystemEvent(event);
    }

    /*////////////////////////////////////////////////////////////////////////////////////////////////*/
    void CMacWindow::OnMouseScroll(float horizontalValue, float verticalValue)
    {
        //Send event
        SSystemEvent event = {};
        event.EventType = ESystemEvent::SYSTEM_EVENT_MOUSE_SCROLLED;
        event.MouseScrolledEvent.Horizontal = horizontalValue;
        event.MouseScrolledEvent.Vertical   = verticalValue;
        m_pSystem->OnSystemEvent(event);
    }
}
#endif
