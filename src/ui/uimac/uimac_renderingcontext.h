#ifndef INCLUDED_ATTA_UIMAC_OPENGLRENDERINGCONTEXT_H
#define INCLUDED_ATTA_UIMAC_OPENGRENDERINGCONTEXT_H

#include <AppKit/NSOpenGL.h>
#include <OpenGL/OpenGL.h>
#include <OpenGL/gl.h>
#include <agtg_renderingcontext.h>

namespace uimac {

class RenderingContext : public agtg::RenderingContext
{
public:
    RenderingContext(NSOpenGLPixelFormat* pixelFormat);
    
    virtual ~RenderingContext();
    
    virtual void makeCurrent();
    
    virtual void preRender();
    
    virtual void postRender();
    
    void lockContext();
    
    void unlockContext();
    
    void setView(NSView* view);
    
    NSOpenGLContext* nativeContext() const;

private:
    NSOpenGLContext* m_context;
};

} // namespace

#endif // INCLUDED
