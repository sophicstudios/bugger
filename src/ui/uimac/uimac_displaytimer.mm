#include <uimac_displaytimer.h>
#include <uimac_renderingcontext.h>
#include <aftt_datetime.h>
#include <aftt_systemtime.h>
#include <aftu_exception.h>
#include <iostream>

#import <QuartzCore/QuartzCore.h>

namespace uimac {

struct DisplayTimerCallback
{
    CVReturn onDisplayRefresh(CVDisplayLinkRef displayLink,
                              const CVTimeStamp* inNow,
                              const CVTimeStamp* inOutputTime,
                              CVOptionFlags flagsIn,
                              CVOptionFlags* flagsOut)
    {
        if (displayRefreshHandler) {
            aftt::Datetime now = aftt::SystemTime::nowAsDatetimeUTC();
            displayRefreshHandler(now);
        }

        return kCVReturnSuccess;
    }

    uigen::DisplayTimer::DisplayRefreshHandler displayRefreshHandler;
};

struct DisplayTimer::Impl
{
    DisplayTimerCallback* callback;
    CVDisplayLinkRef displayLink;
};

CVReturn displayLinkCallback(CVDisplayLinkRef displayLink,
                             const CVTimeStamp *inNow,
                             const CVTimeStamp *inOutputTime,
                             CVOptionFlags flagsIn,
                             CVOptionFlags *flagsOut,
                             void *displayLinkContext)
{
    @autoreleasepool {
        DisplayTimerCallback* callback = reinterpret_cast<DisplayTimerCallback*>(displayLinkContext);
        
        return callback->onDisplayRefresh(displayLink,
                                          inNow,
                                          inOutputTime,
                                          flagsIn,
                                          flagsOut);
    }
}

DisplayTimer::DisplayTimer(RenderingContext& renderingContext, NSOpenGLPixelFormat* pixelFormat)
: m_impl(new DisplayTimer::Impl())
{
    // setup the display link to get a timer linked
    // to the refresh rate of the active display
    CVDisplayLinkCreateWithActiveCGDisplays(&m_impl->displayLink);
    if (!m_impl->displayLink) {
        throw aftu::Exception() << "DisplayTimer: CVDisplayLinkCreateWithActiveCGDisplays failed!";
    }
    
    double refreshPeriod = CVDisplayLinkGetActualOutputVideoRefreshPeriod(m_impl->displayLink);
    std::cout << "DisplayTimer: refreshPeriod: " << refreshPeriod << std::endl;

    m_impl->callback = new DisplayTimerCallback();
    CVReturn cvResult = CVDisplayLinkSetOutputCallback(m_impl->displayLink, displayLinkCallback, m_impl->callback);

    if (cvResult != kCVReturnSuccess) {
        throw aftu::Exception() << "DisplayTimer: CVDisplayLinkSetOutputCallback failed ["
            << " result: " << cvResult
            << " ]";
    }
    
    CGLContextObj cglContext = (CGLContextObj)[renderingContext.nativeContext() CGLContextObj];
    CGLPixelFormatObj cglPixelFormat = (CGLPixelFormatObj)[pixelFormat CGLPixelFormatObj];
    
    CVDisplayLinkSetCurrentCGDisplayFromOpenGLContext(m_impl->displayLink, cglContext, cglPixelFormat);
    
    //m_impl->timeFreq = CVGetHostClockFrequency();
}

DisplayTimer::~DisplayTimer()
{
    if (m_impl) {
        CVDisplayLinkRelease(m_impl->displayLink);
        delete m_impl->callback;
        delete m_impl;
    }
}

void DisplayTimer::registerDisplayRefreshHandler(uigen::DisplayTimer::DisplayRefreshHandler const& handler)
{
    m_impl->callback->displayRefreshHandler = handler;
}

void DisplayTimer::start()
{
    std::cout << "DisplayTimer::start" << std::endl;

    CVDisplayLinkStart(m_impl->displayLink);
}

void DisplayTimer::stop()
{
    std::cout << "DisplayTimer::stop" << std::endl;

    CVDisplayLinkStop(m_impl->displayLink);
}

} // namespace
