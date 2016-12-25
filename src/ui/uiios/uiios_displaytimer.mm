#import <uiios_displaytimer.h>
#import <aftt_datetime.h>
#import <aftt_systemtime.h>
#import <aftu_exception.h>
#import <QuartzCore/QuartzCore.h>
#import <iostream>

@interface uiios_DisplayLinkContext : NSObject {
    uiios::DisplayTimer::Impl* m_displayTimerImpl;
}

- (id) initWithDisplayTimerImpl:(uiios::DisplayTimer::Impl*)displayTimerImpl;

- (void) onDisplayLinkUpdate:(CADisplayLink*)displayLink;

@end

namespace uiios {

struct DisplayTimer::Impl
{
    void onDisplayLinkUpdate(CADisplayLink* displayLink);

    CADisplayLink* displayLink;
    uiios_DisplayLinkContext* displayLinkContext;
    agtui::DisplayTimer::Callback callback;
};

DisplayTimer::DisplayTimer()
: m_impl(new DisplayTimer::Impl())
{
    m_impl->displayLinkContext = [[uiios_DisplayLinkContext alloc] initWithDisplayTimerImpl:m_impl.get()];
    m_impl->displayLink = [[CADisplayLink displayLinkWithTarget:m_impl->displayLinkContext selector:@selector(onDisplayLinkUpdate:)] retain];
}

DisplayTimer::~DisplayTimer()
{
    [m_impl->displayLink invalidate];
    [m_impl->displayLink release];
    [m_impl->displayLinkContext release];
}

void DisplayTimer::registerCallback(agtui::DisplayTimer::Callback const& callback)
{
    m_impl->callback = callback;
}

void DisplayTimer::start()
{
    std::cout << "OpenGLWindow::startDisplayTimer()" << std::endl;
    std::cout << "displayLink retainCount: " << [m_impl->displayLink retainCount] << std::endl;
    [m_impl->displayLink addToRunLoop:[NSRunLoop currentRunLoop] forMode:NSRunLoopCommonModes];
    std::cout << "displayLink retainCount: " << [m_impl->displayLink retainCount] << std::endl;
}

void DisplayTimer::stop()
{
    std::cout << "OpenGLWindow::stopDisplayTimer()" << std::endl;
    std::cout << "displayLink retainCount: " << [m_impl->displayLink retainCount] << std::endl;
    [m_impl->displayLink removeFromRunLoop:[NSRunLoop currentRunLoop] forMode:NSRunLoopCommonModes];
    std::cout << "displayLink retainCount: " << [m_impl->displayLink retainCount] << std::endl;
}

void DisplayTimer::Impl::onDisplayLinkUpdate(CADisplayLink* displayLink)
{
    std::cout << "DisplayTimer::Impl::onDisplayLinkUpdate" << std::endl;
    
    if (callback) {
        //CFTimeInterval timestamp = [displayLink timestamp];
        aftt::Datetime now = aftt::SystemTime::nowAsDatetimeUTC();
        callback();
    }
}

} // namespace

@implementation uiios_DisplayLinkContext

- (id) initWithDisplayTimerImpl:(uiios::DisplayTimer::Impl*)displayTimerImpl
{
    self = [super init];

    if (self)
    {
        m_displayTimerImpl = displayTimerImpl;
    }

    return self;
}

- (void) onDisplayLinkUpdate:(CADisplayLink*)displayLink
{
    m_displayTimerImpl->onDisplayLinkUpdate(displayLink);
}

@end
