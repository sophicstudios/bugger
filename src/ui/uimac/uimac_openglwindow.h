#ifndef INCLUDED_UIMAC_OPENGLWINDOW_H
#define INCLUDED_UIMAC_OPENGLWINDOW_H

#include <agtui_glwindow.h>
#include <agtg_renderingcontext.h>
#include <agtm_rect.h>
#include <agtm_point2d.h>
#include <agtui_mouseevent.h>
#include <aftt_datetime.h>

namespace uimac {

class OpenGLWindow : public agtui::GLWindow
{
public:
    struct Impl;

    OpenGLWindow(std::string const& title, agtm::Rect<float> const& frame);
    
    virtual ~OpenGLWindow();

    virtual void show();
    
    virtual void hide();

    virtual agtui::GLWindow::DisplayTimerPtr displayTimer();
    
    virtual agtm::Rect<float> bounds() const;

    virtual agtui::GLWindow::RenderingContextPtr renderingContext();
    
    virtual void registerResizeEventHandler(agtui::GLWindow::ResizeEventHandler const& handler);

    virtual void registerDrawEventHandler(agtui::GLWindow::DrawEventHandler const& handler);
    
    virtual void registerKeyEventHandler(KeyEventHandler const& handler);

    virtual void registerMouseEventHandler(agtui::GLWindow::MouseEventHandler const& handler);
    
    virtual void registerTouchEventHandler(agtui::GLWindow::TouchEventHandler const& handler);

private:
    OpenGLWindow(OpenGLWindow const&);
    OpenGLWindow& operator=(OpenGLWindow const&);
    
    Impl* m_impl;
};

} // namespace

#endif // INCLUDED
