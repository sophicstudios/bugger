#ifndef INCLUDED_UIMAC_OPENGLWINDOW_H
#define INCLUDED_UIMAC_OPENGLWINDOW_H

#include <agta_glwindow.h>
#include <agtg_renderingcontext.h>
#include <agtm_rect.h>
#include <agtm_point2d.h>
#include <agtui_mouseevent.h>
#include <aftt_datetime.h>

namespace uimac {

class OpenGLWindow : public agta::GLWindow
{
public:
    struct Impl;

    OpenGLWindow(std::string const& title, agtm::Rect<float> const& frame);
    
    virtual ~OpenGLWindow();

    virtual void show();
    
    virtual void hide();

    virtual agta::DisplayTimer& displayTimer();
    
    virtual agtm::Rect<float> bounds();

    virtual agtg::RenderingContext& context();
    
    virtual void registerResizeEventHandler(agta::GLWindow::ResizeEventHandler const& handler);

    virtual void registerDrawEventHandler(agta::GLWindow::DrawEventHandler const& handler);
    
    virtual void registerKeyEventHandler(KeyEventHandler const& handler);

    virtual void registerMouseEventHandler(agta::GLWindow::MouseEventHandler const& handler);
    
    virtual void registerTouchEventHandler(agta::GLWindow::TouchEventHandler const& handler);

private:
    OpenGLWindow(OpenGLWindow const&);
    OpenGLWindow& operator=(OpenGLWindow const&);
    
    Impl* m_impl;
};

} // namespace

#endif // INCLUDED
