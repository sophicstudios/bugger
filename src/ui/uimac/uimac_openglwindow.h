#ifndef INCLUDED_ATTA_UIMAC_OPENGLWINDOW_H
#define INCLUDED_ATTA_UIMAC_OPENGLWINDOW_H

#include <uigen_glwindow.h>
#include <agtg_renderingcontext.h>
#include <agtm_rect.h>
#include <agtm_point2d.h>
#include <agtui_mouseevent.h>
#include <aftt_datetime.h>
#include <boost/function.hpp>

namespace uimac {

class OpenGLWindow : public uigen::GLWindow
{
public:
    struct Impl;

    OpenGLWindow(std::string const& title, agtm::Rect<float> const& frame);
    
    virtual ~OpenGLWindow();

    virtual void show();
    
    virtual void hide();
    
    virtual void startDisplayTimer();
    
    virtual void stopDisplayTimer();
    
    virtual agtm::Rect<float> bounds();

    virtual agtg::RenderingContext& context();
    
    virtual void registerResizeEventHandler(uigen::GLWindow::ResizeEventHandler const& handler);
    
    virtual void registerKeyEventHandler(KeyEventHandler const& handler);

    virtual void registerMouseEventHandler(uigen::GLWindow::MouseEventHandler const& handler);
    
    virtual void registerTouchEventHandler(uigen::GLWindow::TouchEventHandler const& handler);

    virtual void registerDisplayRefreshHandler(uigen::GLWindow::DisplayRefreshHandler const& handler);

private:
    OpenGLWindow(OpenGLWindow const&);
    OpenGLWindow& operator=(OpenGLWindow const&);
    
    Impl* m_impl;
};

} // namespace

#endif // INCLUDED
