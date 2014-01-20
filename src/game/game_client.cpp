#include <game_client.h>
#include <aftt_systemtime.h>
#include <aftt_datetimeinterval.h>
#include <agtg_colorrgba.h>

#include <afts_platform.h>
#if defined(AFTS_OS_IPHONE)

#define RENDERER_OPENGLES
#include <OpenGLES/ES1/gl.h>

#elif defined(AFTS_OS_MACOS)

#define RENDERER_OPENGL
#include <OpenGL/gl.h>

#endif // AFTS_OS

#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <iostream>
#include <functional>
#include <algorithm>
#include <cmath>
#include <cstdlib>

namespace game {

namespace {

static const size_t NUM_PARTICLES = 20;
static const float  PARTICLE_ACCEL_RATE = 800.0f; // feet/second
static const float  MAX_SPEED = 1000.0f; // feet/second
static float MAX_SPREAD = 360.0f; // angles
//static float PI = 3.14159265f;

agtm::Vector2<float> g_sourcePosition;

struct Particle
{
    aftt::DatetimeInterval age;
    aftt::DatetimeInterval maxAge;
    float angle; // angles
    agtm::Vector2<float> velocity;
    agtm::Vector2<float> position;
    agtg::ColorRGBA<float> color;
};

void initParticle(Particle& particle)
{
    // initialize age to zero
    particle.age = aftt::DatetimeInterval();

    float ageJitter = static_cast<float>(rand()) / RAND_MAX;
    
    particle.maxAge = aftt::DatetimeInterval(
        aftt::Seconds(0),
        aftt::Nanoseconds(static_cast<int>(ageJitter * 900000000)));

    float jitter = static_cast<float>(rand()) / RAND_MAX;
    particle.angle = 90.0f - (MAX_SPREAD / 2.0f) + (jitter * MAX_SPREAD);

    // initialize velocity to zero
    particle.velocity = agtm::Vector2<float>(0.0f, 0.0f);
    
    // initialize location to source location
    particle.position = g_sourcePosition;

    // initialize color to red
    particle.color = agtg::ColorRGBA<float>(
        static_cast<float>(rand()) / RAND_MAX,
        static_cast<float>(rand()) / RAND_MAX,
        static_cast<float>(rand()) / RAND_MAX,
        1.0f);
}

} // namespace

struct Client::Impl
{
    Impl()
    : window(NULL),
      paused(false),
      texture(0),
      frame(0),
      direction(0),
      sourcePosition(agtm::Vector2<float>(0.0f, 0.0f)),
      particleTimer(aftt::DatetimeInterval(aftt::Seconds(0), aftt::Nanoseconds(0)))
    {}
    
    void onDrawFrame(aftt::Datetime const& datetime);
    void drawScene(aftt::Datetime const& datetime);

    uigen::GLWindow* window;
    aftt::Datetime prevTime;
    
    std::vector<Particle> particles;
    agtm::Vector2<float> sourcePosition;
    aftt::DatetimeInterval particleTimer;
    bool paused;
    
    Client::ImagePtr image;
    GLuint texture;
    int frame;
    int direction;
};

Client::Client(uigen::GLWindow& window, ImagePtr const& image)
: m_impl(new Impl())
{
    m_impl->window = &window;
    m_impl->image = image;

    glGenTextures(1, &m_impl->texture);
    glBindTexture(GL_TEXTURE_2D, m_impl->texture);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_impl->image->width(), m_impl->image->height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, &(m_impl->image->data()[0]));
}

Client::~Client()
{
    delete m_impl;
}

void Client::run()
{
    std::cout << "Client::run" << std::endl;
    if (!m_impl->paused) {
        m_impl->window->show();
        
        boost::function<void (agtui::MouseEvent const&)> mouseEventHandler
            = boost::bind(&Client::onMouseEvent, this, _1);
        
        m_impl->window->registerMouseEventHandler(mouseEventHandler);

        boost::function<void (aftt::Datetime const&)> displayRefreshHandler
            = boost::bind(&Client::onDrawFrame, this, _1);

        m_impl->window->registerDisplayRefreshHandler(displayRefreshHandler);
        
        m_impl->prevTime = aftt::SystemTime::nowAsDatetimeUTC();
    }
    
    m_impl->window->startDisplayTimer();
}

void Client::pause()
{
    std::cout << "Client::pause" << std::endl;
    
    // stop the game timer, all game state threads and save the current state in case
    // the game never comes back (on iOS devices, for example)

    m_impl->window->stopDisplayTimer();
}

void Client::resume()
{
    std::cout << "Client::resume" << std::endl;
    
    // resume current state, reload resources if they were unloaded, restart game timers
    // and resume action from where we left off
    
    m_impl->window->startDisplayTimer();
}

void Client::stop()
{
    std::cout << "Client::stop" << std::endl;
    m_impl->window->stopDisplayTimer();
}

void Client::onDrawFrame(aftt::Datetime const& datetime)
{
    glMatrixMode(GL_MODELVIEW);

    aftt::DatetimeInterval elapsed = datetime - m_impl->prevTime;
    //double elapsedSeconds = elapsed.totalSeconds();
    m_impl->particleTimer += elapsed;
    int nanoseconds = 1000000000 / 10;
    aftt::DatetimeInterval frameTime = aftt::DatetimeInterval(aftt::Seconds(0), aftt::Nanoseconds(nanoseconds));
    if (m_impl->particleTimer >= frameTime) {
        m_impl->particleTimer -= frameTime;
        
        m_impl->frame = ((m_impl->frame + 1) % 3) + (m_impl->direction * 3);
    }

    m_impl->prevTime = datetime;

    // clear the screen
    glClearColor(0.9f, 0.9f, 0.9f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // initialize the modelview matrix
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // for each particle in the particle list
    typedef std::vector<Particle>::iterator ParticleIter;

    GLfloat vertices[] = {
        -16.0f, -16.0f, 0.0f,
         16.0f, -16.0f, 0.0f,
        -16.0f,  16.0f, 0.0f,
         16.0f,  16.0f, 0.0f
    };

    GLfloat frameWidth = 1.0f / 8.0f;
    GLfloat frameX = m_impl->frame * frameWidth;
    
    // texture coords are flipped vertically
    GLfloat texCoords[] = {
        frameX, 1.0f,
        frameX + frameWidth, 1.0f,
        frameX, 0.0f,
        frameX + frameWidth, 0.0f
    };
    
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    glEnable(GL_CULL_FACE);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    glBindTexture(GL_TEXTURE_2D, m_impl->texture);
    glEnable(GL_TEXTURE_2D);
    glTexCoordPointer(2, GL_FLOAT, 0, texCoords);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    glVertexPointer(3, GL_FLOAT, 0, vertices);
    glEnableClientState(GL_VERTEX_ARRAY);

    glPushMatrix();
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glPopMatrix();
    
    /*
    for (ParticleIter it = m_impl->particles.begin(); it != m_impl->particles.end(); ++it) {
        // save the current modelview matrix
        glPushMatrix();

        // calculate acceleration and increase velocity
        float vel = it->velocity.length();
        if (vel < MAX_SPEED) {
            float dv = std::min(MAX_SPEED - vel, static_cast<float>(PARTICLE_ACCEL_RATE * elapsedSeconds));
            float angle = it->angle * (PI / 180.0);
            float dx = std::cos(angle) * dv;
            float dy = std::sin(angle) * dv;
            it->velocity += agtm::Vector2<float>(dx, dy);
        }
        
        // update position based on current velocity
        it->position += (it->velocity * elapsedSeconds); 
        
        // move the particle along its velocity vector
        glTranslatef(it->position.x(), it->position.y(), 0.0f);
        
        // update the age
        it->age += elapsed;
        
        // draw the particle
        //agtg::ColorRGBA<float>& color = it->color;
        //glColor4f(color.red(), color.green(), color.blue(), color.alpha());
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        
        // restore the modelview matrix
        glPopMatrix();
        
        if (it->age > it->maxAge) {
            initParticle(*it);
        }
    }
    */
    
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisable(GL_TEXTURE_2D);
    
    // fill the particle list based on the particle rate
    if (m_impl->particles.size() < NUM_PARTICLES) {
        double triggerTime = 1.0 / NUM_PARTICLES;
        double particleTimerSeconds = m_impl->particleTimer.totalSeconds();
        if (particleTimerSeconds >= triggerTime) {
            size_t numParticles = particleTimerSeconds / triggerTime;
            for (size_t i = 0; i < numParticles; ++i) {
                Particle particle;
                initParticle(particle);
                m_impl->particles.push_back(particle);
            }
            
            m_impl->particleTimer = aftt::DatetimeInterval(aftt::Seconds(0), aftt::Nanoseconds(0));
        }
    }
}

void Client::onMouseEvent(agtui::MouseEvent const& event)
{
    std::cout << "mouse event: " << event << std::endl;
    if (event.type() == agtui::MouseEvent::Type_MOUSEDOWN) {
        m_impl->direction = m_impl->direction == 0 ? 1 : 0;
        std::cout << "direction: " << m_impl->direction << std::endl;
    }
}

} // namespace
