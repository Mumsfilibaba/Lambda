#include "LambdaPch.h"

#ifdef LAMBDA_PLAT_MACOS
    #include "Platform/macOS/MacConsoleOutput.h"

    #include <stdarg.h>
    #include <Foundation/Foundation.h>

namespace Lambda
{
    MacConsoleOutput::MacConsoleOutput()
        : IConsoleOutput(),
        m_pTextView(nullptr),
        m_pScrollView(nullptr),
        m_pTextViewTextColor(nullptr),
        m_pConsoleWindow(nullptr)
    {
        int32 consoleWidth  = 600;
        int32 consoleHeight = 400;
        int32 consolePosX = 0;
        int32 consolePosY = 0;
        
        CocoaConsoleWindow* pConsole = [[CocoaConsoleWindow alloc] initWithContentRect: NSMakeRect(consolePosX, consolePosY, consoleWidth, consoleHeight) styleMask: (NSWindowStyleMaskTitled | NSWindowStyleMaskClosable | NSWindowStyleMaskMiniaturizable | NSWindowStyleMaskResizable) backing: NSBackingStoreBuffered defer: NO];
        m_pConsoleWindow = pConsole;
        [m_pConsoleWindow setDelegate:m_pConsoleWindow];
        
        NSScrollView* pScrollView = [[NSScrollView alloc] initWithFrame:[[m_pConsoleWindow contentView] frame]];
        m_pScrollView = pScrollView;
        
        NSSize contentSize = [m_pScrollView contentSize];
        [m_pScrollView setBorderType:NSNoBorder];
        [m_pScrollView setHasVerticalScroller:YES];
        [m_pScrollView setHasHorizontalScroller:NO];
        [m_pScrollView setAutoresizingMask:NSViewWidthSizable | NSViewHeightSizable];
        
        NSTextView* pTextView = [[NSTextView alloc] initWithFrame:NSMakeRect( 0, 0, contentSize.width, contentSize.height )];
        m_pTextView = pTextView;
        
        [m_pTextView setMinSize:NSMakeSize( 0.0, contentSize.height ) ];
        [m_pTextView setMaxSize:NSMakeSize( FLT_MAX, FLT_MAX )];
        [m_pTextView setVerticallyResizable:YES];
        [m_pTextView setHorizontallyResizable:NO];
        [m_pTextView setAutoresizingMask:NSViewWidthSizable];
        [m_pTextView setBackgroundColor: [NSColor colorWithSRGBRed:0.1 green:0.1 blue:0.1 alpha:1.0]];
        
        [[m_pTextView textContainer] setContainerSize:NSMakeSize( contentSize.width, FLT_MAX )];
        [[m_pTextView textContainer] setWidthTracksTextView:YES];
        
        [m_pScrollView setDocumentView:m_pTextView];
        [m_pConsoleWindow setContentView:m_pScrollView];

        [m_pConsoleWindow setOpaque:YES];
        
        //Start with a white textcolor
        SetTextColor(EConsoleColor::CONSOLE_COLOR_WHITE);
    }

    MacConsoleOutput::~MacConsoleOutput()
    {
        [m_pTextViewTextColor release];
        [m_pScrollView release];
        [m_pTextView release];
        [m_pConsoleWindow release];
    }
    
    void MacConsoleOutput::Show()
    {
        if (m_pConsoleWindow)
        {
            [m_pConsoleWindow makeKeyAndOrderFront:nil];
        }
    }
    
    void MacConsoleOutput::Hide()
    {
        if (m_pConsoleWindow)
        {            
            [m_pConsoleWindow orderOut:m_pConsoleWindow];
        }
    }

    void MacConsoleOutput::SetTitle(const char* pTitle)
    {
        NSString* pNSTitle = [NSString stringWithUTF8String:pTitle];
        [m_pConsoleWindow setTitle:pNSTitle];
    }

    void MacConsoleOutput::SetTextColor(EConsoleColor color)
    {
        NSMutableArray* pColors        = [[NSMutableArray alloc] init];
        NSMutableArray* pAttributeKeys = [[NSMutableArray alloc] init];
        
        if (color == EConsoleColor::CONSOLE_COLOR_RED)
            [pColors addObject:[NSColor colorWithSRGBRed:1.0 green:0.0 blue:0.0 alpha:1.0]];
        else if (color == EConsoleColor::CONSOLE_COLOR_GREEN)
            [pColors addObject:[NSColor colorWithSRGBRed:0.0 green:1.0 blue:0.0 alpha:1.0]];
        else if (color == EConsoleColor::CONSOLE_COLOR_BLUE)
            [pColors addObject:[NSColor colorWithSRGBRed:0.0 green:0.0 blue:1.0 alpha:1.0]];
        else if (color == EConsoleColor::CONSOLE_COLOR_YELLOW)
            [pColors addObject:[NSColor colorWithSRGBRed:1.0 green:1.0 blue:0.0 alpha:1.0]];
        else
            [pColors addObject:[NSColor colorWithSRGBRed:1.0 green:1.0 blue:1.0 alpha:1.0]];
            
        [pColors addObject:[NSColor colorWithSRGBRed:0.1 green:0.1 blue:0.1 alpha:1.0]];
        
        [pAttributeKeys addObject:NSForegroundColorAttributeName];
        [pAttributeKeys addObject:NSBackgroundColorAttributeName];

        if (m_pTextViewTextColor)
        {
            [m_pTextViewTextColor release];
        }
        
        NSDictionary* pTextViewTextColor = [[NSDictionary alloc] initWithObjects:pColors forKeys:pAttributeKeys];
        m_pTextViewTextColor = pTextViewTextColor;
        
        [pColors release];
        [pAttributeKeys release];
    }

    void MacConsoleOutput::Write(const char* pFormat, ...)
    {
        va_list args;
        va_start(args, pFormat);
        MacConsoleOutput::WriteV(pFormat, args);
        va_end(args);
    }

    void MacConsoleOutput::WriteV(const char* pFormat, va_list args)
    {
        NSString* pNSFormat = [NSString stringWithUTF8String:pFormat];
        NSString* pNSString = [[NSString alloc] initWithFormat:pNSFormat arguments:args];
                
        NSAttributedString* pAttributedString = [[NSAttributedString alloc] initWithString:pNSString attributes:m_pTextViewTextColor];
        [[m_pTextView textStorage] appendAttributedString:pAttributedString];
        
        [[m_pTextView textStorage] setFont:[NSFont fontWithName:@"Courier" size:16]];
        [m_pTextView scrollRangeToVisible:NSMakeRange([[m_pTextView string] length], 0)];
        
        [pAttributedString release];
        [pNSString release];
    }
    
    void MacConsoleOutput::Clear()
    {
        [m_pTextView setString:@""];
    }
}
#endif
