#import <uimac_displaytimer.h>
#import <uimac_renderingcontext.h>
#import <aftt_datetime.h>
#import <aftt_systemtime.h>
#import <aftu_exception.h>
#import <QuartzCore/QuartzCore.h>
#import <iostream>

namespace uimac {

struct DisplayLinkContext
{
    agta::DisplayTimer::Callback callback;
};

struct DisplayTimer::Impl
{
    CVDisplayLinkRef displayLink;
    DisplayLinkContext context;
};

CVReturn displayLinkCallback(CVDisplayLinkRef displayLink,
                             const CVTimeStamp *inNow,
                             const CVTimeStamp *inOutputTime,
                             CVOptionFlags flagsIn,
                             CVOptionFlags *flagsOut,
                             void *displayLinkContext)
{
    @autoreleasepool {
        DisplayLinkContext* context = reinterpret_cast<DisplayLinkContext*>(displayLinkContext);
        if (context->callback) {
            context->callback();
        }
    }

    return kCVReturnSuccess;
}

DisplayTimer::DisplayTimer(RenderingContext& renderingContext, NSOpenGLPixelFormat* pixelFormat)
: m_impl(new DisplayTimer::Impl())
{
    // setup the display link to get a timer with a refresh rate that works
    // with the currently active displays
    CVDisplayLinkCreateWithActiveCGDisplays(&m_impl->displayLink);

    if (!m_impl->displayLink) {
        throw aftu::Exception() << "DisplayTimer: CVDisplayLinkCreateWithActiveCGDisplays failed!";
    }

    // Check the refresh period
    double refreshPeriod = CVDisplayLinkGetActualOutputVideoRefreshPeriod(m_impl->displayLink);

    std::cout << "DisplayTimer: refreshPeriod: " << refreshPeriod << std::endl;

    // set the callback for the display link
    CVReturn cvResult = CVDisplayLinkSetOutputCallback(m_impl->displayLink, displayLinkCallback, &(m_impl->context));

    if (cvResult != kCVReturnSuccess) {
        throw aftu::Exception() << "DisplayTimer: CVDisplayLinkSetOutputCallback failed ["
            << " result: " << cvResult
            << " ]";
    }

    // Get information about the display linked to the current rendering context
    CGLContextObj cglContext = (CGLContextObj)[renderingContext.nativeContext() CGLContextObj];
    CGLPixelFormatObj cglPixelFormat = (CGLPixelFormatObj)[pixelFormat CGLPixelFormatObj];

    // Set the currently active display for the display link
    CVDisplayLinkSetCurrentCGDisplayFromOpenGLContext(m_impl->displayLink, cglContext, cglPixelFormat);
    
    //m_impl->timeFreq = CVGetHostClockFrequency();
}

DisplayTimer::~DisplayTimer()
{
    if (m_impl) {
        CVDisplayLinkRelease(m_impl->displayLink);
        delete m_impl;
    }
}

void DisplayTimer::registerCallback(agta::DisplayTimer::Callback const& callback)
{
    m_impl->context.callback = callback;
}

void DisplayTimer::start()
{
    CVDisplayLinkStart(m_impl->displayLink);
}

void DisplayTimer::stop()
{
    CVDisplayLinkStop(m_impl->displayLink);
}

} // namespace
