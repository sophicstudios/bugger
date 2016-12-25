#import <uiios_openglwindow.h>
#import <uiios_renderingcontext.h>
#import <uiios_displaytimer.h>
#import <aftt_systemtime.h>
#import <UIKit/UIKit.h>
#import <QuartzCore/CAEAGLLayer.h>
#import <QuartzCore/CADisplayLink.h>
#import <OpenGLES/EAGL.h>
#import <iostream>

//////////
// uiios_OpenGLView

namespace {

agtm::Rect<float> fromCGRect(CGRect rect)
{
    return agtm::Rect<float>(
        agtm::Point2d<float>(rect.origin.x, rect.origin.y),
        agtm::Size2d<float>(rect.size.width, rect.size.height));
}

} // namespace

namespace uiios { class OpenGLView; }

@interface uiios_OpenGLView : UIView {
    uiios::OpenGLView* m_view;
}

- (id) initWithFrame:(CGRect)frame View:(uiios::OpenGLView*)view;

@end

//////////
// uiios_OpenGLViewController
@interface uiios_OpenGLViewController: UIViewController {
    uiios::OpenGLView* m_view;
}

- (id) initWithView:(uiios::OpenGLView*)view;

@end

//////////
// uiios_OpenGLWindow
@interface uiios_OpenGLWindow : UIWindow {
    uiios::OpenGLWindow::Impl* m_windowImpl;
}

- (id) initWithFrame:(CGRect)frame windowImpl:(uiios::OpenGLWindow::Impl*)windowImpl;

- (void) windowBecameVisible:(NSNotification*)notification;

- (void) windowBecameHidden:(NSNotification*)notification;

@end

namespace uiios {

class OpenGLView : public agtui::GLView
{
public:
    OpenGLView(CGRect bounds)
    : m_bounds(fromCGRect(bounds))
    {
        // create the view
        m_view = [[uiios_OpenGLView alloc] initWithFrame:bounds View:this];

        m_viewController = [[uiios_OpenGLViewController alloc] initWithView:this];
        m_viewController.view = m_view;

        // create the OpenGL Context
        m_renderingContext = std::shared_ptr<uiios::RenderingContext>(new uiios::RenderingContext((CAEAGLLayer*)[m_view layer]));

        m_displayTimer = std::shared_ptr<uiios::DisplayTimer>(new uiios::DisplayTimer());
    }

    virtual ~OpenGLView()
    {
        [m_viewController release];
        [m_view release];
    }

    uiios_OpenGLView* nativeView() const
    {
        return m_view;
    }

    uiios_OpenGLViewController* nativeViewController() const
    {
        return m_viewController;
    }

    virtual agtm::Rect<float> bounds() const
    {
        return m_bounds;
    }

    virtual agtui::GLView::DisplayTimerPtr displayTimer() const
    {
        return m_displayTimer;
    }
    
    virtual agtui::GLView::RenderingContextPtr renderingContext() const
    {
        return m_renderingContext;
    }

    void onViewResize(agtm::Rect<float> const& rect)
    {
        std::cout << "OpenGLView::onViewResize" << std::endl;
        m_renderingContext->makeCurrent();
        m_renderingContext->destroyRenderBuffers();
        m_renderingContext->createRenderBuffers();
        this->onResize(rect);
    }
    
    void onViewDraw(agtm::Rect<float> const& dirtyRect)
    {
        std::cout << "OpenGLView::onViewDraw" << std::endl;
        this->onDraw(dirtyRect);
    }
    
    void onViewTouchEvent(agtui::MouseEvent const& event)
    {
        std::cout << "OpenGLView::onViewMouseEvent" << std::endl;
    }
    
    void onViewKeyEvent()
    {
        std::cout << "OpenGLView::onViewKeyEvent" << std::endl;
    }
    
private:
    uiios_OpenGLView* m_view;
    uiios_OpenGLViewController* m_viewController;
    agtm::Rect<float> m_bounds;
    std::shared_ptr<uiios::RenderingContext> m_renderingContext;
    std::shared_ptr<uiios::DisplayTimer> m_displayTimer;
};

//////////
// OpenGLWindow::Impl
struct OpenGLWindow::Impl
{
    uiios_OpenGLWindow* window;
    std::shared_ptr<uiios::OpenGLView> view;
};

//////////
// OpenGLWindow IMPLEMENTATION

OpenGLWindow::OpenGLWindow(std::string const& title)
: m_impl(new OpenGLWindow::Impl())
{
    // get the screen bounds
    CGRect screenBounds = [[UIScreen mainScreen] bounds];

    // create the window object
    m_impl->window = [[uiios_OpenGLWindow alloc] initWithFrame:screenBounds windowImpl:m_impl];

    m_impl->view = std::shared_ptr<uiios::OpenGLView>(new uiios::OpenGLView(screenBounds));
    [m_impl->window setRootViewController:m_impl->view->nativeViewController()];
}

OpenGLWindow::~OpenGLWindow()
{
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

std::shared_ptr<agtui::GLView> OpenGLWindow::glView()
{
    return m_impl->view;
}

} // namespace

@implementation uiios_OpenGLView

+ (Class) layerClass
{
    std::cout << "uiios_OpenGLView::layerClass" << std::endl;
    return [CAEAGLLayer class];
}

- (id) initWithFrame:(CGRect)frame View:(uiios::OpenGLView*)view
{
    std::cout << "uiios_OpenGLView::initWithFrame" << std::endl;
    
    self = [super initWithFrame:frame];
    if (self) {
        CAEAGLLayer* layer = (CAEAGLLayer*)self.layer;
        layer.opaque = YES;
        layer.drawableProperties = [NSDictionary dictionaryWithObjectsAndKeys:
            [NSNumber numberWithBool:NO], kEAGLDrawablePropertyRetainedBacking,
            kEAGLColorFormatRGBA8, kEAGLDrawablePropertyColorFormat, nil];
                
        m_view = view;
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

- (id) initWithView:(uiios::OpenGLView *)view
{
    std::cout << "uiios_OpenGLViewController::initWithView" << std::endl;
    
    self = [super init];
    if (self) {
        m_view = view;
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

    m_view->onViewResize(bounds);
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

@end
