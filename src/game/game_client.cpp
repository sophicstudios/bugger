#include <game_client.h>
#include <aftt_systemtime.h>
#include <OpenGL/gl.h>
#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <iostream>
#include <functional>

namespace wrangler {
namespace game {

using namespace aegis;

static const NUM_PARTICLES = 100;
static const PARTICLE_CREATION_RATE = 10; // per second
static const PARTICLE_ACCEL_RATE = 100; // feet/second
static const MAX_SPEED = 20; // feet/second
static const SECONDS_ALIVE = 1;

struct Particle
{
    aftt::DatetimeInterval age;
    agtm::Vector2<float> velocity;
    agtm::Vector2<float> position;
    agtg::ColorRGBA<float> color;
};

class Client::Impl
{
public:
    Impl();
    
    ~Impl();
    
    void run();
    
    void onResize(aegis::agtm::Rect<float> const& bounds);
    
    void onDrawFrame(aegis::aftt::Datetime const& datetime);

private:
    void initializeOpenGL();
    void initializeSource();
    void drawScene(aegis::aftt::Datetime const& datetime);

    uimac::OpenGLWindow* m_window;
    aegis::aftt::Datetime m_prevTime;
    
    std::vector<Particle> m_particles;
    agtm::Vector2<float> m_sourceLocation;
    agtm::Vector2<float> m_particleDirection;
};

Client::Impl::Impl()
{
    agtm::Rect<float> frame(agtm::Point2d<float>(0.0f, 0.0f), agtm::Size2d<float>(640.0f, 480.0f));
    m_window = new uimac::OpenGLWindow("Wrangler", frame);
    
    m_window->context().makeCurrent();

    initializeOpenGL();
    initializeSource();
    onResize(frame);
}

Client::Impl::~Impl()
{
    delete m_window;
}

void Client::Impl::initializeOpenGL()
{
    std::cout << "Client::initializeOpenGL" << std::endl;
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
        
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    glTranslatef(0.0f, 0.0f, -2.0f);
    
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}

void Client::Impl::initializeSource()
{
    std::cout << "Client::initializeMesh" << std::endl;
}

void Client::Impl::run()
{
    boost::function<void (agtm::Rect<float> const&)> resizeHandler
        = boost::bind(&Impl::onResize, this, _1);

    m_window->registerResizeEventHandler(resizeHandler);

    boost::function<void (aftt::Datetime const&)> displayRefreshHandler
        = boost::bind(&Impl::onDrawFrame, this, _1);

    m_window->registerDisplayRefreshHandler(displayRefreshHandler);

    m_window->show();
    
    m_prevTime = aftt::SystemTime::nowAsDatetimeUTC();
    
    m_window->startDisplayTimer();
}

void Client::Impl::onResize(agtm::Rect<float> const& bounds)
{
    std::cout << "Client::onResize bounds: " << bounds << std::endl;
    
    m_window->context().preRender();
    
    GLdouble width = static_cast<GLdouble>(bounds.width());
    GLdouble height = static_cast<GLdouble>(bounds.height());
    GLdouble aspect = width / height;
    GLdouble right = aspect * (width * 2);
    GLdouble left = -right;
    GLdouble top = height / 2;
    GLdouble bottom = -top;
    GLdouble near = -1.0;
    GLdouble far = 5.0;
    
    std::cout << "width: " << width << std::endl;
    std::cout << "height: " << height << std::endl;
    std::cout << "left: " << left << std::endl;
    std::cout << "right: " << right << std::endl;
    std::cout << "top: " << top << std::endl;
    std::cout << "bottom: " << bottom << std::endl;
    
    glViewport(0, 0, static_cast<GLsizei>(width), static_cast<GLsizei>(height));

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    
    //glFrustum(left, right, bottom, top, near, far);
    glOrtho(left, right, bottom, top, near, far);
    
    glMatrixMode(GL_MODELVIEW);
    
    drawScene(m_prevTime);

    m_window->context().postRender();
}

void Client::Impl::onDrawFrame(aftt::Datetime const& datetime)
{
    m_window->context().preRender();
    drawScene(datetime);
    m_window->context().postRender();
}

void Client::Impl::drawScene(aftt::Datetime const& datetime)
{
    aftt::DatetimeInterval elapsed = datetime - m_prevTime;

    std::cout << "Client::drawScene ["
        << " datetime: " << datetime
        << " prev: " << m_prevTime
        << " elapsed: " << elapsed
        << " ]" << std::endl;

    m_prevTime = datetime;    
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glBegin(GL_TRIANGLES);
        glColor3f(1.0f, 0.0f, 0.0f);
        glVertex3f(-1.0f, -1.0f, 0.0f);
        glColor3f(0.0f, 1.0f, 0.0f);
        glVertex3f(1.0f, -1.0f, 0.0f);
        glColor3f(0.0f, 0.0f, 1.0f);
        glVertex3f(0.0f, 1.0f, 0.0f);
    glEnd();
}

Client::Client()
: m_impl(new Impl())
{
}

Client::~Client()
{
    delete m_impl;
}

void Client::run()
{
    m_impl->run();
}

void Client::onResize(agtm::Rect<float> const& bounds)
{
    m_impl->onResize(bounds);
}

void Client::onDrawFrame(aftt::Datetime const& datetime)
{
    m_impl->onDrawFrame(datetime);
}

} // namespace
} // namespace
