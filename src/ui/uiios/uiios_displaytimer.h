#ifndef INCLUDED_UIIOS_DISPLAYTIMER_H
#define INCLUDED_UIIOS_DISPLAYTIMER_H

#include <agtui_displaytimer.h>

namespace uiios {

class RenderingContext;

class DisplayTimer : public agtui::DisplayTimer
{
public:
    struct Impl;

    DisplayTimer();
    
    virtual ~DisplayTimer();

    virtual void registerCallback(agtui::DisplayTimer::Callback const& callback);

    virtual void start();
    
    virtual void stop();

private:
    std::shared_ptr<Impl> m_impl;
};

} // namespace

#endif // INCLUDED
