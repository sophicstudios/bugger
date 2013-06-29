#import <Cocoa/Cocoa.h>
#import <Foundation/NSGeometry.h>
#import <QuartzCore/CVDisplayLink.h>
#import <QuartzCore/CVHostTime.h>
#import <uimac_openglwindow.h>
#import <uimac_renderingcontext.h>
#import <aftt_systemtime.h>
#import <iostream>

using namespace aegis;

namespace wrangler {
namespace uimac {

// OpenGLWindow::Impl
class OpenGLWindow::Impl
{
public:
    Impl(std::string const& title, agtm::Rect<float> const& frame);
    
    ~Impl();
    
    void show();
    
    void hide();
    
    void startDisplayTimer();
    
    void stopDisplayTimer();
    
    agtg::RenderingContext& context();
    
    void registerResizeEventHandler(boost::function<void (agtm::Rect<float> const&)> const& handler);
    
    void registerMouseEventHandler(boost::function<void (agtui::MouseEvent const&)> const& handler);

    void registerDisplayRefreshHandler(boost::function<void (aegis::aftt::Datetime const&)> const& handler);

// Called by View
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
    NSWindow* m_window;
    NSOpenGLPixelFormat* m_pixelFormat;
    uimac::RenderingContext* m_renderingContext;
    CVDisplayLinkRef m_displayLink;
    double m_timeFreq;
    uint64_t m_baseTime;
    boost::function<void (agtm::Rect<float> const&)> m_sizeHandler;
    boost::function<void (agtui::MouseEvent const&)> m_mouseHandler;
    boost::function<void (aftt::Datetime const&)> m_displayRefreshHandler;
};

CVReturn displayLinkCallback(
    CVDisplayLinkRef displayLink,
    const CVTimeStamp *inNow,
    const CVTimeStamp *inOutputTime,
    CVOptionFlags flagsIn,
    CVOptionFlags *flagsOut,
    void *displayLinkContext);

} // namespace
} // namespace

// uimac_OpenGLView
@interface uimac_OpenGLView : NSView {
    wrangler::uimac::OpenGLWindow::Impl* m_windowImpl;
    NSOpenGLContext* m_context;
    NSRect m_bounds;
}

- (id) initWithFrame: (NSRect)frame
    context: (NSOpenGLContext*)context
    windowImpl: (wrangler::uimac::OpenGLWindow::Impl*)windowImpl;

- (void)resizeView;

@end

@implementation uimac_OpenGLView

- (id)initWithFrame:(NSRect)frame
    context:(NSOpenGLContext *)context
    windowImpl: (wrangler::uimac::OpenGLWindow::Impl*)windowImpl
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

namespace wrangler {
namespace uimac {

CVReturn displayLinkCallback(
    CVDisplayLinkRef displayLink,
    const CVTimeStamp *inNow,
    const CVTimeStamp *inOutputTime,
    CVOptionFlags flagsIn,
    CVOptionFlags *flagsOut,
    void *displayLinkContext)
{
    @autoreleasepool {
        return reinterpret_cast<OpenGLWindow::Impl*>(displayLinkContext)->onDisplayRefresh(
            displayLink, inNow, inOutputTime, flagsIn, flagsOut);
    }
}

//////////
// OpenGLWindow::Impl IMPLEMENTATION

OpenGLWindow::Impl::Impl(std::string const& title, agtm::Rect<float> const& frame)
{
    // create window and OpenGL view
    //NSRect screenRect = [[NSScreen mainScreen] visibleFrame]; // get dimensions of screen
    
    NSRect windowFrame = NSMakeRect(0.0f, 0.0f, static_cast<float>(frame.width()), static_cast<float>(frame.height()));

    m_window = [[NSWindow alloc] initWithContentRect:windowFrame
        styleMask:NSTitledWindowMask
            | NSClosableWindowMask
            | NSMiniaturizableWindowMask
            | NSResizableWindowMask
            | NSTexturedBackgroundWindowMask
        backing:NSBackingStoreBuffered
        defer:NO];
    
    [m_window setTitle:[NSString stringWithUTF8String: title.c_str()]];
    [m_window setReleasedWhenClosed:YES];
    
    // setup pixel format and rendering context
    NSOpenGLPixelFormatAttribute attrs[] = {
        NSOpenGLPFADoubleBuffer,
        NSOpenGLPFADepthSize, 32,
        0            
    };
    
    m_pixelFormat = [[NSOpenGLPixelFormat alloc] initWithAttributes:attrs];
    if (!m_pixelFormat) {
        std::cerr << "Could not allocate pixel format" << std::endl;
    }
    
    m_renderingContext = new uimac::RenderingContext(m_pixelFormat);

    NSRect contentRect = [m_window contentRectForFrameRect:windowFrame];
    
    uimac_OpenGLView* view = [[[uimac_OpenGLView alloc] initWithFrame:contentRect
        context:m_renderingContext->nativeContext()
        windowImpl: this] autorelease];
    
    [m_window setOpaque:YES];
    [m_window setContentView:view];
    [m_window makeFirstResponder:view];
    
    // setup the display link to get a timer linked
    // to the refresh rate of the active display
    CVDisplayLinkCreateWithActiveCGDisplays(&m_displayLink);
    
    double refreshPeriod = CVDisplayLinkGetActualOutputVideoRefreshPeriod(m_displayLink);
    
    std::cout << "refreshPeriod: " << refreshPeriod << std::endl;
    
    CVReturn cvResult = CVDisplayLinkSetOutputCallback(
        m_displayLink, displayLinkCallback, this);

    if (cvResult != kCVReturnSuccess) {
        std::cout << "CVDisplayLinkSetOutputCallback failed ["
            << " result: " << cvResult
            << " ]" << std::endl;
    }
    
    CGLContextObj cglContext = (CGLContextObj)[m_renderingContext->nativeContext() CGLContextObj];
    CGLPixelFormatObj cglPixelFormat = (CGLPixelFormatObj)[m_pixelFormat CGLPixelFormatObj];
    
    CVDisplayLinkSetCurrentCGDisplayFromOpenGLContext(m_displayLink, cglContext, cglPixelFormat);
    
    m_timeFreq = CVGetHostClockFrequency();
}

OpenGLWindow::Impl::~Impl()
{
    [m_pixelFormat release];
    [m_window release];    
}

void OpenGLWindow::Impl::show()
{
    [m_window makeKeyAndOrderFront:NSApp];
}

void OpenGLWindow::Impl::hide()
{
}

void OpenGLWindow::Impl::startDisplayTimer()
{
    m_baseTime = CVGetCurrentHostTime();
    
    CVDisplayLinkStart(m_displayLink);
}

void OpenGLWindow::Impl::stopDisplayTimer()
{
    CVDisplayLinkStop(m_displayLink);
}

aegis::agtg::RenderingContext& OpenGLWindow::Impl::context()
{
    return *m_renderingContext;
}
    
void OpenGLWindow::Impl::registerResizeEventHandler(boost::function<void (aegis::agtm::Rect<float> const&)> const& handler)
{
    m_sizeHandler = handler;
}

void OpenGLWindow::Impl::registerMouseEventHandler(boost::function<void (aegis::agtui::MouseEvent const&)> const& handler)
{
    m_mouseHandler = handler;
}

void OpenGLWindow::Impl::registerDisplayRefreshHandler(boost::function<void (aegis::aftt::Datetime const&)> const& handler)
{
    m_displayRefreshHandler = handler;
}

void OpenGLWindow::Impl::onViewResize(agtm::Rect<float> const& rect)
{
    std::cout << "Impl::onViewResize" << std::endl;
    m_sizeHandler(rect);
}

void OpenGLWindow::Impl::onViewDraw(agtm::Rect<float> const& dirtyRect)
{
    std::cout << "Impl::onViewDraw" << std::endl;
}

void OpenGLWindow::Impl::onViewMouseEvent(agtui::MouseEvent const& event)
{
    std::cout << "Impl::onViewMouseEvent" << std::endl;
}

void OpenGLWindow::Impl::onViewKeyEvent()
{
    std::cout << "Impl::onViewKeyEvent" << std::endl;
}

CVReturn OpenGLWindow::Impl::onDisplayRefresh(
    CVDisplayLinkRef displayLink,
    const CVTimeStamp *inNow,
    const CVTimeStamp *inOutputTime,
    CVOptionFlags flagsIn,
    CVOptionFlags *flagsOut)
{
    aftt::Datetime now = aftt::SystemTime::nowAsDatetimeUTC();

/*
    std::cout << "Impl::onDisplayRefresh ["
        << " outputTime (video): " << (inOutputTime->videoTime - m_baseTime) / m_timeFreq
        << " outputTime (host): "<< (inOutputTime->hostTime - m_baseTime) / m_timeFreq
        << " now: " << now
        << " ]" << std::endl;
    
*/
    
    if (m_displayRefreshHandler) {
        m_displayRefreshHandler(now);
    }
    
    return kCVReturnSuccess;
}

//////////
// OpenGLWindow IMPLEMENTATION

OpenGLWindow::OpenGLWindow(std::string const& title, agtm::Rect<float> const& frame)
: m_impl(new OpenGLWindow::Impl(title, frame))
{}

OpenGLWindow::~OpenGLWindow()
{
    delete m_impl;
}

void OpenGLWindow::show()
{
    m_impl->show();
}

void OpenGLWindow::hide()
{
    m_impl->hide();
}

void OpenGLWindow::startDisplayTimer()
{
    m_impl->startDisplayTimer();
}

void OpenGLWindow::stopDisplayTimer()
{
    m_impl->stopDisplayTimer();
}

agtg::RenderingContext& OpenGLWindow::context()
{
    return m_impl->context();
}

void OpenGLWindow::registerResizeEventHandler(boost::function<void (agtm::Rect<float> const&)> const& handler)
{
    m_impl->registerResizeEventHandler(handler);
}

void OpenGLWindow::registerMouseEventHandler(boost::function<void (agtui::MouseEvent const&)> const& handler)
{
    m_impl->registerMouseEventHandler(handler);
}

void OpenGLWindow::registerDisplayRefreshHandler(boost::function<void (aegis::aftt::Datetime const&)> const& handler)
{
    m_impl->registerDisplayRefreshHandler(handler);
}

} // namespace
} // namespace
