#ifndef INCLUDED_ATTA_UIGEN_GLWINDOW_H
#define INCLUDED_ATTA_UIGEN_GLWINDOW_H

#include <aftt_datetime.h>
#include <agtg_renderingcontext.h>
#include <agtm_rect.h>
#include <agtui_mouseevent.h>
#include <agtui_touchevent.h>
#include <boost/function.hpp>

namespace uigen {

class GLWindow
{
public:
    typedef boost::function<void (agtm::Rect<float> const&)> ResizeEventHandler;
    typedef boost::function<void ()> KeyEventHandler;
    typedef boost::function<void (agtui::MouseEvent const&)> MouseEventHandler;
    typedef boost::function<void (agtui::TouchEvent const&)> TouchEventHandler;
    typedef boost::function<void (aftt::Datetime const&)> DisplayRefreshHandler;
    
    GLWindow();
    
    virtual ~GLWindow() = 0;

    virtual void show() = 0;
    
    virtual void hide() = 0;
    
    virtual void startDisplayTimer() = 0;
    
    virtual void stopDisplayTimer() = 0;
    
    virtual agtm::Rect<float> bounds() = 0;
    
    virtual agtg::RenderingContext& context() = 0;
    
    virtual void registerResizeEventHandler(ResizeEventHandler const& handler) = 0;
    
    virtual void registerKeyEventHandler(KeyEventHandler const& handler) = 0;
    
    virtual void registerMouseEventHandler(MouseEventHandler const& handler) = 0;
    
    virtual void registerTouchEventHandler(TouchEventHandler const& handler) = 0;

    virtual void registerDisplayRefreshHandler(DisplayRefreshHandler const& handler) = 0;
};

} // namespace

#endif // INCLUDED
