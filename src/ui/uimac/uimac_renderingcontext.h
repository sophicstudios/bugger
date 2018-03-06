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
    RenderingContext(NSOpenGLPixelFormat* pixelFormat);
    
    virtual ~RenderingContext();
    
    virtual void makeCurrent();

    virtual agtg::RenderingContext::ShaderProgramPtr createShader();

    virtual void preRender();
    
    virtual void postRender();
    
    void lockContext();
    
    void unlockContext();
    
    void setView(NSView* view);
    
    NSOpenGLContext* nativeContext() const;

private:
    NSOpenGLContext* m_context;
    GLuint m_currentProgram;
};

} // namespace

#endif // INCLUDED
