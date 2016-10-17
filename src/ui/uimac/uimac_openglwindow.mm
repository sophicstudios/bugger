#import <uimac_openglwindow.h>
#import <uimac_renderingcontext.h>
#import <uimac_displaytimer.h>
#import <agtg_gl.h>
#import <aftt_systemtime.h>
#import <aftu_exception.h>
#import <Cocoa/Cocoa.h>
#import <Foundation/NSGeometry.h>
#import <iostream>
#import <string>
#import <vector>

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

class OpenGLView : public agtui::GLView
{
public:
    OpenGLView(NSRect bounds)
    {
        m_bounds = agtm::Rect<float>(
            agtm::Point2d<float>(bounds.origin.x, bounds.origin.y),
            agtm::Size2d<float>(bounds.size.width, bounds.size.height));
        
        // setup pixel format and rendering context
        std::vector<NSOpenGLPixelFormatAttribute> attrs;
        attrs.push_back(NSOpenGLPFADoubleBuffer);
        attrs.push_back(NSOpenGLPFADepthSize);
        attrs.push_back(32);
        attrs.push_back(NSOpenGLPFAOpenGLProfile);

        bool oldVersionCheck = false;

        #if MAC_OS_X_VERSION_MAX_ALLOWED >= 101000 // OSX Yosemite has hardware support for 4.1
            attrs.push_back(NSOpenGLProfileVersion4_1Core);
        #elif MAC_OS_X_VERSION_MAX_ALLOWED >= 1090 // OSX Mavericks should use 3.2 (4.1 falls back to software)
            attrs.push_back(NSOpenGLProfileVersion3_2Core);
        #else
            attrs.push_back(NSOpenGLProfileVersionLegacy);
            oldVersionCheck = true;
        #endif

        attrs.push_back(0);

        m_pixelFormat = [[NSOpenGLPixelFormat alloc] initWithAttributes:&(attrs[0])];
        if (!m_pixelFormat) {
            std::cerr << "Could not allocate pixel format! MAC_OS_X_VERSION_MAX_ALLOWED: " << MAC_OS_X_VERSION_MAX_ALLOWED << std::endl;
            throw std::exception();
        }

        if (oldVersionCheck) {
            const GLubyte* glVersion = glGetString(GL_VERSION);
            std::cout << "OpenGL version (glGetString): " << glVersion << std::endl;
        } else {
            GLint glMajorVersion;
            GLint glMinorVersion;

            glGetIntegerv(GL_MAJOR_VERSION, &glMajorVersion);
            glGetIntegerv(GL_MINOR_VERSION, &glMinorVersion);

            std::cout << "OpenGL version: " << glMajorVersion << "." << glMinorVersion << std::endl;
        }

        m_renderingContext = std::shared_ptr<uimac::RenderingContext>(new uimac::RenderingContext(m_pixelFormat));

        // setup the display link to get a timer linked
        // to the refresh rate of the active display
        m_displayTimer = std::shared_ptr<uimac::DisplayTimer>(new uimac::DisplayTimer(m_renderingContext->nativeContext(), m_pixelFormat));

        m_view = [[uimac_OpenGLView alloc] initWithFrame:bounds context:m_renderingContext->nativeContext() windowImpl: m_impl];
    }

    virtual ~OpenGLView()
    {
        [m_pixelFormat release];
        [m_view release];
    }

    uimac_OpenGLView* nativeView() const
    {
        return m_view;
    }

    virtual agtm::Rect<float> bounds() const
    {
        return m_bounds;
    }

    virtual DisplayTimerPtr displayTimer() const;
    
    virtual RenderingContextPtr renderingContext() const;

    void onViewResize(agtm::Rect<float> const& rect);
    
    void onViewDraw(agtm::Rect<float> const& dirtyRect);
    
    void onViewMouseEvent(agtui::MouseEvent const& event);
    
    void onViewKeyEvent();
    
    CVReturn onDisplayRefresh(CVDisplayLinkRef displayLink,
        const CVTimeStamp *inNow,
        const CVTimeStamp *inOutputTime,
        CVOptionFlags flagsIn,
        CVOptionFlags *flagsOut);

private:
    uimac_OpenGLView* m_view;
    agtm::Rect<float> m_bounds;
    NSOpenGLPixelFormat* m_pixelFormat;
    std::shared_ptr<uimac::RenderingContext> m_renderingContext;
    std::shared_ptr<uimac::DisplayTimer> m_displayTimer;
};

// OpenGLWindow::Impl
struct OpenGLWindow::Impl
{
    NSWindow* window;
    agtm::Rect<float> bounds;
    std::shared_ptr<uimac::OpenGLView> glView;
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

    NSRect contentRect = [m_impl->window contentRectForFrameRect:windowFrame];
    
    m_impl->glView = new uimac::OpenGLView(contentRect);

    [m_impl->window setOpaque:YES];
    [m_impl->window setContentView:m_impl->glView->nativeView()];
    [m_impl->window makeFirstResponder:m_impl->glView->nativeView()];
    
    m_impl->glView->renderingContext()->makeCurrent();
}

OpenGLWindow::~OpenGLWindow()
{
    [m_impl->window release];

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
agtm::Rect<float> OpenGLWindow::bounds() const
{
    return m_impl->bounds;
}

void OpenGLWindow::resizeEventHandler(agtm::Rect<float> const& bounds)
{
    onResize(bounds);
}

void OpenGLWindow::Impl::onViewResize(agtm::Rect<float> const& rect)
{
    std::cout << "Impl::onViewResize" << std::endl;
    if (resizeEventHandler) {
        resizeEventHandler(rect);
    }
}

void OpenGLWindow::Impl::onViewDraw(agtm::Rect<float> const& dirtyRect)
{
    std::cout << "Impl::onViewDraw" << std::endl;
    if (drawEventHandler) {
        drawEventHandler();
    }
}

void OpenGLWindow::Impl::onViewMouseEvent(agtui::MouseEvent const& event)
{
    std::cout << "Impl::onViewMouseEvent" << std::endl;
    if (mouseEventHandler) {
        mouseEventHandler(event);
    }
}

void OpenGLWindow::Impl::onViewKeyEvent()
{
    std::cout << "Impl::onViewKeyEvent" << std::endl;
    if (keyEventHandler) {
        keyEventHandler();
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

