#import <UIKit/UIKit.h>
#import <QuartzCore/CAEAGLLayer.h>
#import <QuartzCore/CADisplayLink.h>
#import <OpenGLES/EAGL.h>
#import <uiios_openglwindow.h>
#import <uiios_renderingcontext.h>
#import <aftt_systemtime.h>
#import <iostream>

//////////
// uiios_OpenGLView

@interface uiios_OpenGLView : UIView {
    uiios::OpenGLWindow::Impl* m_windowImpl;
}

- (id) initWithFrame:(CGRect)frame windowImpl:(uiios::OpenGLWindow::Impl*)windowImpl;

@end

//////////
// uiios_OpenGLViewController
@interface uiios_OpenGLViewController: UIViewController {
    uiios::OpenGLWindow::Impl* m_windowImpl;
}

- (id) initWithWindowImpl:(uiios::OpenGLWindow::Impl*)windowImpl;

@end

//////////
// uiios_OpenGLWindow
@interface uiios_OpenGLWindow : UIWindow {
    uiios::OpenGLWindow::Impl* m_windowImpl;
}

//@property (nonatomic, strong) CADisplayLink* displayLink;

- (id) initWithFrame:(CGRect)frame windowImpl:(uiios::OpenGLWindow::Impl*)windowImpl;
- (void) windowBecameVisible:(NSNotification*)notification;
- (void) windowBecameHidden:(NSNotification*)notification;
- (void) onDisplayLinkUpdate:(CADisplayLink*)displayLink;

@end

namespace uiios {

//////////
// OpenGLWindow::Impl
struct OpenGLWindow::Impl
{
    void onDisplayLinkUpdate(CADisplayLink* displayLink);
    void onViewResize(agtm::Rect<float> const& rect);
    void onViewTouchEvent(agtui::TouchEvent const& event);

    uiios_OpenGLWindow* window;
    uiios_OpenGLView* view;
    uiios_OpenGLViewController* viewController;
    agtm::Rect<float> bounds;
    RenderingContext* renderingContext;
    CADisplayLink* displayLink;
    double timeFreq;
    uint64_t baseTime;
    uigen::GLWindow::ResizeEventHandler sizeHandler;
    uigen::GLWindow::KeyEventHandler keyHandler;
    uigen::GLWindow::TouchEventHandler touchHandler;
    uigen::GLWindow::DisplayRefreshHandler displayRefreshHandler;    
};

//////////
// OpenGLWindow IMPLEMENTATION

OpenGLWindow::OpenGLWindow(std::string const& title)
: m_impl(new OpenGLWindow::Impl())
{
    // get the screen bounds
    CGRect screenBounds = [[UIScreen mainScreen] bounds];
    
    agtm::Rect<float> bounds(
        agtm::Point2d<float>(screenBounds.origin.x, screenBounds.origin.y),
        agtm::Size2d<float>(screenBounds.size.width, screenBounds.size.height));

    // create the window object
    m_impl->window = [[uiios_OpenGLWindow alloc] initWithFrame:screenBounds windowImpl:m_impl];

    // create the view
    m_impl->view = [[uiios_OpenGLView alloc] initWithFrame:screenBounds windowImpl:m_impl];

    m_impl->viewController = [[uiios_OpenGLViewController alloc] initWithWindowImpl:m_impl];
    m_impl->viewController.view = m_impl->view;

    [m_impl->window setRootViewController:m_impl->viewController];

    // create the OpenGL Context
    m_impl->renderingContext = new uiios::RenderingContext((CAEAGLLayer*)[m_impl->view layer]);

    m_impl->renderingContext->makeCurrent();
    
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
        
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    
    glClearColor(0.0f, 1.0f, 0.0f, 1.0f);
    
    m_impl->displayLink = [[CADisplayLink displayLinkWithTarget:m_impl->window selector:@selector(onDisplayLinkUpdate:)] retain];
}

OpenGLWindow::~OpenGLWindow()
{
    delete m_impl->renderingContext;
    
    std::cout << "~OpenGLWindow displayLink retainCount: " << [m_impl->displayLink retainCount] << std::endl;
    [m_impl->displayLink release];
    [m_impl->viewController release];
    [m_impl->view release];
    [m_impl->window release];

    delete m_impl;
}

void OpenGLWindow::show()
{
    std::cout << "OpenGLWindow::show()" << std::endl;
    [m_impl->window makeKeyAndVisible];
}

void OpenGLWindow::hide()
{
    std::cout << "OpenGLWindow::hide()" << std::endl;
}

void OpenGLWindow::startDisplayTimer()
{
    std::cout << "OpenGLWindow::startDisplayTimer()" << std::endl;
    std::cout << "displayLink retainCount: " << [m_impl->displayLink retainCount] << std::endl;
    [m_impl->displayLink addToRunLoop:[NSRunLoop currentRunLoop] forMode:NSRunLoopCommonModes];
    std::cout << "displayLink retainCount: " << [m_impl->displayLink retainCount] << std::endl;
}

void OpenGLWindow::pauseDisplayTimer()
{
    [m_impl->displayLink setPaused:TRUE];
}

void OpenGLWindow::resumeDisplayTimer()
{
    [m_impl->displayLink setPaused:FALSE];
}

void OpenGLWindow::stopDisplayTimer()
{
    std::cout << "OpenGLWindow::stopDisplayTimer()" << std::endl;
    std::cout << "displayLink retainCount: " << [m_impl->displayLink retainCount] << std::endl;
    [m_impl->displayLink invalidate];
    std::cout << "displayLink retainCount: " << [m_impl->displayLink retainCount] << std::endl;
}

agtm::Rect<float> OpenGLWindow::bounds()
{
    return m_impl->bounds;
}

agtg::RenderingContext& OpenGLWindow::context()
{
    return *m_impl->renderingContext;
}

void OpenGLWindow::registerResizeEventHandler(uigen::GLWindow::ResizeEventHandler const& handler)
{
    m_impl->sizeHandler = handler;
}

void OpenGLWindow::registerKeyEventHandler(KeyEventHandler const& handler)
{
    m_impl->keyHandler = handler;
}

void OpenGLWindow::registerMouseEventHandler(uigen::GLWindow::MouseEventHandler const& handler)
{
    // no mouse events currently
}

void OpenGLWindow::registerTouchEventHandler(uigen::GLWindow::TouchEventHandler const& handler)
{
    m_impl->touchHandler = handler;
}

void OpenGLWindow::registerDisplayRefreshHandler(uigen::GLWindow::DisplayRefreshHandler const& handler)
{
    m_impl->displayRefreshHandler = handler;
}

void OpenGLWindow::Impl::onDisplayLinkUpdate(CADisplayLink* displayLink)
{
    std::cout << "onDisplayLinkUpdate" << std::endl;
    
    if (displayRefreshHandler) {
        //CFTimeInterval timestamp = [displayLink timestamp];
        aftt::Datetime now = aftt::SystemTime::nowAsDatetimeUTC();
        renderingContext->preRender();
        displayRefreshHandler(now);
        renderingContext->postRender();
    }
}

void OpenGLWindow::Impl::onViewResize(agtm::Rect<float> const& rect)
{
    std::cout << "onViewResize rect:" << rect << std::endl;
    
    bounds = rect;
    
    renderingContext->destroyRenderBuffers();
    renderingContext->createRenderBuffers();
    
    agtm::Size2d<int> bufferSize = renderingContext->getRenderBufferSize();

    GLfloat width = static_cast<GLfloat>(bufferSize.width());
    GLfloat height = static_cast<GLfloat>(bufferSize.height());

    glViewport(0, 0, static_cast<GLsizei>(width), static_cast<GLsizei>(height));

    std::cout << "width: " << width << std::endl;
    std::cout << "height: " << height << std::endl;

    GLfloat right = width / 2;
    GLfloat left = -right;
    GLfloat top = height / 2;
    GLfloat bottom = -top;
    
    std::cout << "left: " << left << std::endl;
    std::cout << "right: " << right << std::endl;
    std::cout << "top: " << top << std::endl;
    std::cout << "bottom: " << bottom << std::endl;
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    
    glOrthof(left, right, bottom, top, -1.0f, 1.0f);

    if (displayRefreshHandler) {
        //CFTimeInterval timestamp = [displayLink timestamp];
        aftt::Datetime now = aftt::SystemTime::nowAsDatetimeUTC();
        renderingContext->preRender();
        displayRefreshHandler(now);
        renderingContext->postRender();
    }
}

void OpenGLWindow::Impl::onViewTouchEvent(agtui::TouchEvent const& event)
{
}

} // namespace

@implementation uiios_OpenGLView

+ (Class) layerClass
{
    std::cout << "uiios_OpenGLView::layerClass" << std::endl;
    return [CAEAGLLayer class];
}

- (id) initWithFrame:(CGRect)frame
    windowImpl:(uiios::OpenGLWindow::Impl*)windowImpl
{
    std::cout << "uiios_OpenGLView::initWithFrame" << std::endl;
    
    self = [super initWithFrame:frame];
    if (self) {
        CAEAGLLayer* layer = (CAEAGLLayer*)self.layer;
        layer.opaque = YES;
        layer.drawableProperties = [NSDictionary dictionaryWithObjectsAndKeys:
            [NSNumber numberWithBool:NO], kEAGLDrawablePropertyRetainedBacking,
            kEAGLColorFormatRGBA8, kEAGLDrawablePropertyColorFormat, nil];
                
        m_windowImpl = windowImpl;
    }
    
    return self;
}

- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event
{
    std::cout << "uiios_OpenGLView::touchesBegan" << std::endl;
}

- (void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event
{
    std::cout << "uiios_OpenGLView::touchesMoved" << std::endl;
}

- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event
{
    std::cout << "uiios_OpenGLView::touchesEnded" << std::endl;
}

- (void)touchesCancelled:(NSSet *)touches withEvent:(UIEvent *)event
{
    std::cout << "uiios_OpenGLView::touchesCancelled" << std::endl;
}

@end

@implementation uiios_OpenGLViewController

- (id) initWithWindowImpl:(uiios::OpenGLWindow::Impl*)windowImpl
{
    std::cout << "uiios_OpenGLViewController::initWithWindowImpl" << std::endl;
    
    self = [super init];
    if (self) {
        m_windowImpl = windowImpl;
    }
    
    return self;
}

- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation
{
    std::cout << "uiios_OpenGLViewController::shouldAutorotateToInterfaceOrientation ["
        << " interfaceOrientation: " << interfaceOrientation
        << " ]" << std::endl;
    
    if ([[UIDevice currentDevice] userInterfaceIdiom] == UIUserInterfaceIdiomPhone) {
        return (interfaceOrientation != UIInterfaceOrientationPortraitUpsideDown);
    } else {
        return YES;
    }
}

- (void)viewWillLayoutSubviews
{
    CGRect viewBounds = [[self view] bounds];
    
    agtm::Rect<float> bounds(
        agtm::Point2d<float>(viewBounds.origin.x, viewBounds.origin.y),
        agtm::Size2d<float>(viewBounds.size.width, viewBounds.size.height));

    m_windowImpl->onViewResize(bounds);
}

@end

@implementation uiios_OpenGLWindow

- (id) initWithFrame:(CGRect)frame windowImpl:(uiios::OpenGLWindow::Impl*)windowImpl
{
    std::cout << "uiios_OpenGLWindow::initWithFrame" << std::endl;
    
    self = [super initWithFrame:frame];
    if (self) {
        m_windowImpl = windowImpl;
        
        [[NSNotificationCenter defaultCenter] addObserver:self
            selector:@selector(windowBecameVisible:)
            name:UIWindowDidBecomeVisibleNotification
            object:nil];
            
        [[NSNotificationCenter defaultCenter] addObserver:self
            selector:@selector(windowBecameHidden:)
            name:UIWindowDidBecomeHiddenNotification
            object:nil];
    }
    
    return self;
}

- (void) windowBecameVisible:(NSNotification*)notification
{
    std::cout << "uiios_OpenGLWindow::windowBecameVisible" << std::endl;
}

- (void) windowBecameHidden:(NSNotification*)notification
{
    std::cout << "uiios_OpenGLWindow::windowBecameHidden" << std::endl;
}

- (void) onDisplayLinkUpdate:(CADisplayLink *)displayLink
{
    m_windowImpl->onDisplayLinkUpdate(displayLink);
}

@end
