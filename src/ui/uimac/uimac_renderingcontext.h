#ifndef INCLUDED_WRANGLER_UIMAC_OPENGLRENDERINGCONTEXT_H
#define INCLUDED_WRANGLER_UIMAC_OPENGRENDERINGCONTEXT_H

#include <AppKit/NSOpenGL.h>
#include <OpenGL/OpenGL.h>
#include <OpenGL/gl.h>
#include <agtg_renderingcontext.h>

namespace wrangler {
namespace uimac {

class RenderingContext : public aegis::agtg::RenderingContext
{
public:
    RenderingContext(NSOpenGLPixelFormat* pixelFormat);
    
    virtual ~RenderingContext();
    
    virtual void makeCurrent();
    
    virtual void preRender();
    
    virtual void postRender();
    
    void setView(NSView* view);
    
    NSOpenGLContext* nativeContext() const;

private:
    NSOpenGLContext* m_context;
};

} // namespace
} // namespace

#endif // INCLUDED
