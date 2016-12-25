#ifndef INCLUDED_UIIOS_OPENGLWINDOW_H
#define INCLUDED_UIIOS_OPENGLWINDOW_H

#include <agtui_glview.h>
#include <memory>
#include <string>

namespace uiios {

class OpenGLWindow
{
public:
    struct Impl;

    OpenGLWindow(std::string const& title);
    
    virtual ~OpenGLWindow();

    virtual void show();
    
    virtual void hide();
    
    virtual std::shared_ptr<agtui::GLView> glView();

private:
    OpenGLWindow(OpenGLWindow const&);
    OpenGLWindow& operator=(OpenGLWindow const&);
    
    Impl* m_impl;
};

} // namespace

#endif // INCLUDED
