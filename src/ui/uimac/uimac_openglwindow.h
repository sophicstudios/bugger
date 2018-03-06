#ifndef INCLUDED_BUGGER_UIMAC_OPENGLWINDOW_H
#define INCLUDED_BUGGER_UIMAC_OPENGLWINDOW_H

#include <agtui_glview.h>
#include <agtm_rect.h>
#include <memory>
#include <string>

namespace uimac {

class OpenGLWindow
{
public:
    struct Impl;

    OpenGLWindow(std::string const& title, agtm::Rect<float> const& frame);
    
    virtual ~OpenGLWindow();

    virtual void show();
    
    virtual void hide();

    virtual std::shared_ptr<agtui::GLView> glView();

private:
    OpenGLWindow(OpenGLWindow const&);

    OpenGLWindow& operator=(OpenGLWindow const&);

    void resizeEventHandler(agtm::Rect<float> const& bounds);
    
    Impl* m_impl;
};

} // namespace

#endif // INCLUDED
