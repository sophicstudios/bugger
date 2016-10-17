#ifndef INCLUDED_UIMAC_DISPLAYTIMER_H
#define INCLUDED_UIMAC_DISPLAYTIMER_H

#include <agtui_displaytimer.h>

namespace uimac {

class RenderingContext;

class DisplayTimer : public agtui::DisplayTimer
{
public:
    DisplayTimer(NSOpenGLContext* openglContext, NSOpenGLPixelFormat* pixelFormat);
    
    virtual ~DisplayTimer();

    virtual void registerCallback(agtui::DisplayTimer::Callback const& callback);

    virtual void start();
    
    virtual void stop();

private:
    struct Impl;
    Impl* m_impl;
};

} // namespace

#endif // INCLUDED
