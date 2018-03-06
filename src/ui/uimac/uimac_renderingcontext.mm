#import <uimac_renderingcontext.h>
#import <aftl_logger.h>
#import <AppKit/AppKit.h>

namespace uimac {

RenderingContext::RenderingContext(NSOpenGLPixelFormat* pixelFormat)
{
    m_context = [[NSOpenGLContext alloc] initWithFormat:pixelFormat shareContext:nil];
    if (!m_context) {
        throw aftu::Exception("Could not allocate context");
    }
    
    GLint sync = 1;
    [m_context setValues:&sync forParameter:NSOpenGLCPSwapInterval];
}

RenderingContext::~RenderingContext()
{
    if (m_context) {
        [m_context release];
    }
}

void RenderingContext::makeCurrent()
{
    [m_context makeCurrentContext];
}


agtg::RenderingContext::ShaderProgramPtr RenderingContext::createShader()
{
    return agtg::RenderingContext::ShaderProgramPtr(new agtg::ShaderProgram());
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

void RenderingContext::setView(NSView* view)
{
    [m_context setView:view];
}
    
NSOpenGLContext* RenderingContext::nativeContext() const
{
    return m_context;
}

} // namespace
