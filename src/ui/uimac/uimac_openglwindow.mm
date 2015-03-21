#import <Cocoa/Cocoa.h>
#import <Foundation/NSGeometry.h>
#import <uimac_openglwindow.h>
#import <uimac_renderingcontext.h>
#import <uimac_displaytimer.h>
#import <aftt_systemtime.h>
#import <iostream>

// uimac_OpenGLView
@interface uimac_OpenGLView : NSView {
    uimac::OpenGLWindow::Impl* m_windowImpl;
    NSOpenGLContext* m_context;
    NSRect m_bounds;
}

- (id) initWithFrame: (NSRect)frame
    context: (NSOpenGLContext*)context
    windowImpl: (uimac::OpenGLWindow::Impl*)windowImpl;

- (void)resizeView;

@end

namespace uimac {

// OpenGLWindow::Impl
struct OpenGLWindow::Impl
{
    void onViewResize(agtm::Rect<float> const& rect);
    
    void onViewDraw(agtm::Rect<float> const& dirtyRect);
    
    void onViewMouseEvent(agtui::MouseEvent const& event);
    
    void onViewKeyEvent();
    
    CVReturn onDisplayRefresh(CVDisplayLinkRef displayLink,
        const CVTimeStamp *inNow,
        const CVTimeStamp *inOutputTime,
        CVOptionFlags flagsIn,
        CVOptionFlags *flagsOut);

    NSWindow* window;
    agtm::Rect<float> bounds;
    NSOpenGLPixelFormat* pixelFormat;
    uimac::RenderingContext* renderingContext;
    uimac::DisplayTimer* displayTimer;
    agta::GLWindow::ResizeEventHandler sizeHandler;
    agta::GLWindow::KeyEventHandler keyHandler;
    agta::GLWindow::MouseEventHandler mouseHandler;
    agta::DisplayTimer::DisplayRefreshHandler displayRefreshHandler;
};


//////////
// OpenGLWindow IMPLEMENTATION

OpenGLWindow::OpenGLWindow(std::string const& title, agtm::Rect<float> const& frame)
: m_impl(new OpenGLWindow::Impl())
{
    // create window and OpenGL view
    //NSRect screenRect = [[NSScreen mainScreen] visibleFrame]; // get dimensions of screen
    
    NSRect windowFrame = NSMakeRect(0.0f, 0.0f, static_cast<float>(frame.width()), static_cast<float>(frame.height()));

    m_impl->window = [[NSWindow alloc] initWithContentRect:windowFrame
        styleMask:NSTitledWindowMask
            | NSClosableWindowMask
            | NSMiniaturizableWindowMask
            | NSResizableWindowMask
            | NSTexturedBackgroundWindowMask
        backing:NSBackingStoreBuffered
        defer:NO];
    
    [m_impl->window setTitle:[NSString stringWithUTF8String: title.c_str()]];
    [m_impl->window setReleasedWhenClosed:YES];
    
    // setup pixel format and rendering context
    NSOpenGLPixelFormatAttribute attrs[] = {
        NSOpenGLPFADoubleBuffer,
        NSOpenGLPFADepthSize, 32,
        0            
    };
    
    m_impl->pixelFormat = [[NSOpenGLPixelFormat alloc] initWithAttributes:attrs];
    if (!m_impl->pixelFormat) {
        std::cerr << "Could not allocate pixel format" << std::endl;
    }
    
    m_impl->renderingContext = new uimac::RenderingContext(m_impl->pixelFormat);

    NSRect contentRect = [m_impl->window contentRectForFrameRect:windowFrame];
    
    m_impl->bounds = agtm::Rect<float>(
        agtm::Point2d<float>(contentRect.origin.x, contentRect.origin.y),
        agtm::Size2d<float>(contentRect.size.width, contentRect.size.height));
    
    uimac_OpenGLView* view = [[[uimac_OpenGLView alloc] initWithFrame:contentRect
        context:m_impl->renderingContext->nativeContext()
        windowImpl: m_impl] autorelease];
    
    [m_impl->window setOpaque:YES];
    [m_impl->window setContentView:view];
    [m_impl->window makeFirstResponder:view];
    
    // setup the display link to get a timer linked
    // to the refresh rate of the active display
    m_impl->displayTimer = new uimac::DisplayTimer(*m_impl->renderingContext, m_impl->pixelFormat);

    m_impl->renderingContext->makeCurrent();
}

OpenGLWindow::~OpenGLWindow()
{
    [m_impl->pixelFormat release];
    [m_impl->window release];

    delete m_impl->displayTimer;

    delete m_impl;
}

void OpenGLWindow::show()
{
    [m_impl->window makeKeyAndOrderFront:NSApp];

    m_impl->onViewResize(m_impl->bounds);
}

void OpenGLWindow::hide()
{
}

agta::DisplayTimer& OpenGLWindow::displayTimer()
{
    return *m_impl->displayTimer;
}

agtm::Rect<float> OpenGLWindow::bounds()
{
    return m_impl->bounds;
}

agtg::RenderingContext& OpenGLWindow::context()
{
    return *m_impl->renderingContext;
}

void OpenGLWindow::registerResizeEventHandler(agta::GLWindow::ResizeEventHandler const& handler)
{
    m_impl->sizeHandler = handler;
}

void OpenGLWindow::registerKeyEventHandler(KeyEventHandler const& handler)
{
    m_impl->keyHandler = handler;
}

void OpenGLWindow::registerMouseEventHandler(agta::GLWindow::MouseEventHandler const& handler)
{
    m_impl->mouseHandler = handler;
}

void OpenGLWindow::registerTouchEventHandler(agta::GLWindow::TouchEventHandler const& handler)
{
    // no touch events currently
}

void OpenGLWindow::Impl::onViewResize(agtm::Rect<float> const& rect)
{
    std::cout << "Impl::onViewResize" << std::endl;
    if (sizeHandler) {
        sizeHandler(rect);
    }
}

void OpenGLWindow::Impl::onViewDraw(agtm::Rect<float> const& dirtyRect)
{
    std::cout << "Impl::onViewDraw" << std::endl;
}

void OpenGLWindow::Impl::onViewMouseEvent(agtui::MouseEvent const& event)
{
    std::cout << "Impl::onViewMouseEvent" << std::endl;
    if (mouseHandler) {
        mouseHandler(event);
    }
}

void OpenGLWindow::Impl::onViewKeyEvent()
{
    std::cout << "Impl::onViewKeyEvent" << std::endl;
    if (keyHandler) {
        keyHandler();
    }
}

} // namespace

@implementation uimac_OpenGLView

- (id)initWithFrame:(NSRect)frame
    context:(NSOpenGLContext *)context
    windowImpl: (uimac::OpenGLWindow::Impl*)windowImpl
{
    self = [super initWithFrame:frame];

    if (self) {
        m_windowImpl = windowImpl;
        m_context = [context retain];
        m_bounds = [self bounds];
    }
        
    return self;
}

- (void)dealloc
{
    [m_context release];
    [super dealloc];
}

- (void)awakeFromNib
{
    // nothing to do here
}

- (void)lockFocus
{
    std::cout << "lockFocus" << std::endl;
    [super lockFocus];
    
    if ([m_context view] != self) {
        std::cout << "uimac_OpenGLView: setting context view to self" << std::endl;
        [m_context setView:self];
    }
}

- (BOOL)isOpaque
{
    return YES;
}

- (void)resizeView
{
    NSRect resizeBounds = [self bounds];
    
    std::cout << "uimac_OpenGLView: resizeView ["
        << " resizeBounds: ["
        << resizeBounds.origin.x << " " << resizeBounds.origin.y << " "
        << resizeBounds.size.width << " " << resizeBounds.size.height
        << " ] bounds: ["
        << m_bounds.origin.x << " " << m_bounds.origin.y << " "
        << m_bounds.size.width << " " << m_bounds.size.height
        << " ]" << " ]" << std::endl;

    m_bounds = resizeBounds;
    
    agtm::Rect<float> rect(
        agtm::Point2d<float>(m_bounds.origin.x, m_bounds.origin.y),
        agtm::Size2d<float>(m_bounds.size.width, m_bounds.size.height));

    m_windowImpl->onViewResize(rect);
}

- (void)drawRect:(NSRect)dirtyRect
{
    @autoreleasepool {
        if ([self inLiveResize]) {
            std::cout << "uimac_OpenGLView: drawRect (inLiveResize) ["
                << " dirtyRect: ["
                << dirtyRect.origin.x << " " << dirtyRect.origin.y << " "
                << dirtyRect.size.width << " " << dirtyRect.size.height
                << " ]" << " ]" << std::endl;

            [self resizeView];
        }
        else {
            std::cout << "drawRect ["
                << " dirtyRect: ["
                << dirtyRect.origin.x << " " << dirtyRect.origin.y << " "
                << dirtyRect.size.width << " " << dirtyRect.size.height
                << " ]" << " ]" << std::endl;
        }

        agtm::Rect<float> rect(
            agtm::Point2d<float>(m_bounds.origin.x, m_bounds.origin.y),
            agtm::Size2d<float>(m_bounds.size.width, m_bounds.size.height));

        m_windowImpl->onViewDraw(rect);
    }
}

- (BOOL)acceptsFirstResponder
{
    return YES;
}

- (void)mouseDown:(NSEvent *)event
{
    NSPoint location = [event locationInWindow];
    agtui::MouseEvent::Type type = agtui::MouseEvent::Type_MOUSEDOWN;
    agtui::MouseEvent::Button button = agtui::MouseEvent::Button_LEFT;
    agtm::Point2d<float> point(location.x, location.y);
    
    agtui::MouseEvent mouseEvent(type, button, point);
    
    m_windowImpl->onViewMouseEvent(mouseEvent);
}

- (void)rightMouseDown:(NSEvent*)event
{
    NSPoint location = [event locationInWindow];
    agtui::MouseEvent::Type type = agtui::MouseEvent::Type_MOUSEDOWN;
    agtui::MouseEvent::Button button = agtui::MouseEvent::Button_RIGHT;
    agtm::Point2d<float> point(location.x, location.y);
    
    agtui::MouseEvent mouseEvent(type, button, point);
    
    m_windowImpl->onViewMouseEvent(mouseEvent);
}

- (void)mouseUp:(NSEvent *)event
{
    NSPoint location = [event locationInWindow];
    agtui::MouseEvent::Type type = agtui::MouseEvent::Type_MOUSEUP;
    agtui::MouseEvent::Button button = agtui::MouseEvent::Button_LEFT;
    agtm::Point2d<float> point(location.x, location.y);
    
    agtui::MouseEvent mouseEvent(type, button, point);
    
    m_windowImpl->onViewMouseEvent(mouseEvent);
}

- (void)rightMouseUp:(NSEvent*)event
{
    NSPoint location = [event locationInWindow];
    agtui::MouseEvent::Type type = agtui::MouseEvent::Type_MOUSEUP;
    agtui::MouseEvent::Button button = agtui::MouseEvent::Button_RIGHT;
    agtm::Point2d<float> point(location.x, location.y);
    
    agtui::MouseEvent mouseEvent(type, button, point);
    
    m_windowImpl->onViewMouseEvent(mouseEvent);
}

- (void)mouseMoved:(NSEvent *)event
{
    NSPoint location = [event locationInWindow];
    agtui::MouseEvent::Type type = agtui::MouseEvent::Type_MOUSEMOVE;
    agtui::MouseEvent::Button button = agtui::MouseEvent::Button_NONE;
    agtm::Point2d<float> point(location.x, location.y);
    
    agtui::MouseEvent mouseEvent(type, button, point);
    
    m_windowImpl->onViewMouseEvent(mouseEvent);
}

- (void)mouseDragged:(NSEvent *)event
{
    NSPoint location = [event locationInWindow];
    agtui::MouseEvent::Type type = agtui::MouseEvent::Type_MOUSEDRAG;
    agtui::MouseEvent::Button button = agtui::MouseEvent::Button_LEFT;
    agtm::Point2d<float> point(location.x, location.y);
    
    agtui::MouseEvent mouseEvent(type, button, point);
    
    m_windowImpl->onViewMouseEvent(mouseEvent);
}

- (void)rightMouseDragged:(NSEvent*)event
{
    NSPoint location = [event locationInWindow];
    agtui::MouseEvent::Type type = agtui::MouseEvent::Type_MOUSEDRAG;
    agtui::MouseEvent::Button button = agtui::MouseEvent::Button_RIGHT;
    agtm::Point2d<float> point(location.x, location.y);
    
    agtui::MouseEvent mouseEvent(type, button, point);
    
    m_windowImpl->onViewMouseEvent(mouseEvent);
}

- (void)mouseEntered:(NSEvent *)event
{
    NSPoint location = [event locationInWindow];
    agtui::MouseEvent::Type type = agtui::MouseEvent::Type_MOUSEENTER;
    agtui::MouseEvent::Button button = agtui::MouseEvent::Button_NONE;
    agtm::Point2d<float> point(location.x, location.y);
    
    agtui::MouseEvent mouseEvent(type, button, point);
    
    m_windowImpl->onViewMouseEvent(mouseEvent);
}

- (void)mouseExited:(NSEvent *)event
{
    NSPoint location = [event locationInWindow];
    agtui::MouseEvent::Type type = agtui::MouseEvent::Type_MOUSELEAVE;
    agtui::MouseEvent::Button button = agtui::MouseEvent::Button_NONE;
    agtm::Point2d<float> point(location.x, location.y);
    
    agtui::MouseEvent mouseEvent(type, button, point);
    
    m_windowImpl->onViewMouseEvent(mouseEvent);
}

- (void)keyDown:(NSEvent *)event
{
    NSLog(@"uimac_OpenGLView: keyDown: %@", event);

    m_windowImpl->onViewKeyEvent();
}

- (void)keyUp:(NSEvent *)event
{
    NSLog(@"uimac_OpenGLView: keyUp: %@", event);

    m_windowImpl->onViewKeyEvent();
}

@end

