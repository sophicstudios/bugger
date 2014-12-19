#ifndef INCLUDED_ATTA_UIIOS_RENDERINGCONTEXT_H
#define INCLUDED_ATTA_UIIOS_RENDERINGCONTEXT_H

#import <GLKit/GLKit.h>
#import <QuartzCore/CAEAGLLayer.h>
#import <OpenGLES/ES1/gl.h>
#import <OpenGLES/ES1/glext.h>
#import <agtg_renderingcontext.h>
#import <agtm_size2d.h>

namespace uiios {

class RenderingContext : public agtg::RenderingContext
{
public:
    RenderingContext(CAEAGLLayer* layer);
    
    virtual ~RenderingContext();
    
    virtual void preRender();
    
    virtual void postRender();

    void makeCurrent();
    
    void destroyRenderBuffers();
    
    void createRenderBuffers();
    
    agtm::Size2d<int> getRenderBufferSize();
    
private:
    EAGLContext* m_context;
    CAEAGLLayer* m_layer;
    GLuint m_frameBuffer;
    GLuint m_colorRenderBuffer;
    GLuint m_depthRenderBuffer;
};

} // namespace

#endif // INCLUDED
