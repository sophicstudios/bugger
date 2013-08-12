#include <uiios_renderingcontext.h>
#include <iostream>
#include <exception>

namespace uiios {

RenderingContext::RenderingContext(CAEAGLLayer* layer)
: m_layer(layer),
  m_frameBuffer(0),
  m_colorRenderBuffer(0),
  m_depthRenderBuffer(0)
{
    m_context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES1];
    if (!m_context) {
        std::cerr << "Could not allocate EAGLContext (ES1)" << std::endl;
        throw std::exception();
    }
}

RenderingContext::~RenderingContext()
{
    [m_context release];
}

void RenderingContext::preRender()
{
    makeCurrent();
    glBindFramebuffer(GL_FRAMEBUFFER, m_frameBuffer);
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
}

void RenderingContext::postRender()
{
    const GLenum discards[] = { GL_DEPTH_ATTACHMENT };
    glBindFramebuffer(GL_FRAMEBUFFER, m_frameBuffer);
    glDiscardFramebufferEXT(GL_FRAMEBUFFER, 1, discards);
    
    glBindRenderbuffer(GL_RENDERBUFFER, m_colorRenderBuffer);
    [m_context presentRenderbuffer:GL_RENDERBUFFER];
}

void RenderingContext::makeCurrent()
{
    [EAGLContext setCurrentContext:m_context];
}

void RenderingContext::destroyRenderBuffers()
{
    makeCurrent();
    
    if (m_depthRenderBuffer != 0) {
        glDeleteRenderbuffers(1, &m_depthRenderBuffer);
        m_depthRenderBuffer = 0;
    }
    
    if (m_colorRenderBuffer != 0) {
        glDeleteRenderbuffers(1, &m_colorRenderBuffer);
        m_colorRenderBuffer = 0;
    }
    
    if (m_frameBuffer != 0) {
        glDeleteFramebuffers(1,  &m_frameBuffer);
        m_frameBuffer = 0;
    }
}

void RenderingContext::createRenderBuffers()
{
    makeCurrent();
    
    glGenFramebuffers(1, &m_frameBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, m_frameBuffer);
    
    glGenRenderbuffers(1, &m_colorRenderBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, m_colorRenderBuffer);
    [m_context renderbufferStorage:GL_RENDERBUFFER fromDrawable:m_layer];
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, m_colorRenderBuffer);
    
    GLint width, height;
    glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH, &width);
    glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_HEIGHT, &height);
    
    glGenRenderbuffers(1, &m_depthRenderBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, m_depthRenderBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_depthRenderBuffer);
}

agtm::Size2d<int> RenderingContext::getRenderBufferSize()
{
    GLint width, height;
    glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH, &width);
    glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_HEIGHT, &height);
    
    return agtm::Size2d<GLint>(width, height);
}

} // namespace
