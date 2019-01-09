#ifndef INCLUDED_BUGGER_UIMAC_OPENGLRENDERINGCONTEXT_H
#define INCLUDED_BUGGER_UIMAC_OPENGLRENDERINGCONTEXT_H

#include <agtg_gl.h>
#include <agtg_shaderprogram.h>
#include <AppKit/NSOpenGL.h>
#include <agtg_renderingcontext.h>

namespace uimac {

class RenderingContext : public agtg::RenderingContext
{
public:
    RenderingContext(NSOpenGLContext* context);
    
    virtual ~RenderingContext();
    
    virtual void makeCurrent();

    virtual void preRender();
    
    virtual void postRender();
    
    void lockContext();
    
    void unlockContext();
        
    NSOpenGLContext* nativeContext() const;

private:
    NSOpenGLContext* m_context;
};

} // namespace

#endif // INCLUDED
