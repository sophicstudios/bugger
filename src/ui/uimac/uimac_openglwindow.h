#ifndef INCLUDED_WRANGLER_UIMAC_OPENGLWINDOW_H
#define INCLUDED_WRANGLER_UIMAC_OPENGLWINDOW_H

#include <agtg_renderingcontext.h>
#include <agtm_rect.h>
#include <agtm_point2d.h>
#include <agtui_mouseevent.h>
#include <aftt_datetime.h>
#include <boost/function.hpp>

namespace wrangler {
namespace uimac {

class OpenGLWindow
{
public:
    struct Impl;

    OpenGLWindow(std::string const& title, aegis::agtm::Rect<float> const& frame);
    
    ~OpenGLWindow();

    void show();
    
    void hide();
    
    void startDisplayTimer();
    
    void stopDisplayTimer();
    
    aegis::agtg::RenderingContext& context();
    
    void registerResizeEventHandler(boost::function<void (aegis::agtm::Rect<float> const&)> const& handler);
    
    void registerMouseEventHandler(boost::function<void (aegis::agtui::MouseEvent const&)> const& handler);

    void registerDisplayRefreshHandler(boost::function<void (aegis::aftt::Datetime const&)> const& handler);

private:
    OpenGLWindow(OpenGLWindow const&);
    OpenGLWindow& operator=(OpenGLWindow const&);
    
    Impl* m_impl;
};

} // namespace
} // namespace

#endif // INCLUDED
