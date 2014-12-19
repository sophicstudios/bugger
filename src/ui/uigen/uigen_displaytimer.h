#ifndef INCLUDED_UIGEN_DISPLAYTIMER_H
#define INCLUDED_UIGEN_DISPLAYTIMER_H

#include <functional>
#include <aftt_datetime.h>

namespace uigen {

class DisplayTimer
{
public:
    typedef std::function<void (aftt::Datetime const&)> DisplayRefreshHandler;
    
    DisplayTimer();
    
    virtual ~DisplayTimer() = 0;

    virtual void registerDisplayRefreshHandler(DisplayRefreshHandler const& handler) = 0;

    virtual void start() = 0;
    
    virtual void stop() = 0;
};

} // namespace

#endif // INCLUDED
