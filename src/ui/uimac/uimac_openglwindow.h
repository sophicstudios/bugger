#ifndef INCLUDED_UIMAC_OPENGLWINDOW_H
#define INCLUDED_UIMAC_OPENGLWINDOW_H

#include <agtui_window.h>
#include <agtg_renderingcontext.h>
#include <agtm_rect.h>
#include <agtm_point2d.h>
#include <agtui_mouseevent.h>
#include <aftt_datetime.h>

namespace uimac {

class OpenGLWindow : public agtui::Window
{
public:
    struct Impl;

    OpenGLWindow(std::string const& title, agtm::Rect<float> const& frame);
    
    virtual ~OpenGLWindow();

    virtual void show();
    
    virtual void hide();

    virtual agtm::Rect<float> bounds() const;

    virtual agtui::Window::DisplayTimerPtr displayTimer() const;

    virtual agtui::Window::RenderingContextPtr renderingContext() const;
    
private:
    OpenGLWindow(OpenGLWindow const&);

    OpenGLWindow& operator=(OpenGLWindow const&);

    void resizeEventHandler(agtm::Rect<float> const& bounds);
    
    Impl* m_impl;
};

} // namespace

#endif // INCLUDED
