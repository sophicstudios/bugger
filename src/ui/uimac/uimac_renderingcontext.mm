#import <uimac_renderingcontext.h>
#import <aftl_logger.h>
#import <AppKit/AppKit.h>

namespace uimac {

RenderingContext::RenderingContext(NSOpenGLContext* context)
: m_context(context)
{
    GLint sync = 1;
    [m_context setValues:&sync forParameter:NSOpenGLContextParameterSwapInterval];
}

RenderingContext::~RenderingContext()
{
}

void RenderingContext::makeCurrent()
{
    [m_context makeCurrentContext];
}

void RenderingContext::preRender()
{
    CGLContextObj cglContext = (CGLContextObj)[m_context CGLContextObj];
    CGLLockContext(cglContext);

    [m_context makeCurrentContext];
    [m_context update];
}

void RenderingContext::postRender()
{
    [m_context flushBuffer];

    CGLContextObj cglContext = (CGLContextObj)[m_context CGLContextObj];
    CGLUnlockContext(cglContext);
}
    
NSOpenGLContext* RenderingContext::nativeContext() const
{
    return m_context;
}

} // namespace
