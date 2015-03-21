#ifndef INCLUDED_UIMAC_DISPLAYTIMER_H
#define INCLUDED_UIMAC_DISPLAYTIMER_H

#include <agta_displaytimer.h>

namespace uimac {

class RenderingContext;

class DisplayTimer : public agta::DisplayTimer
{
public:
    DisplayTimer(RenderingContext& renderingContext, NSOpenGLPixelFormat* pixelFormat);
    
    virtual ~DisplayTimer();

    virtual void registerDisplayRefreshHandler(uigen::DisplayTimer::DisplayRefreshHandler const& handler);

    virtual void start();
    
    virtual void stop();

private:
    struct Impl;
    Impl* m_impl;
};

} // namespace

#endif // INCLUDED
