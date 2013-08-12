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

static const size_t NUM_PARTICLES = 1000;
static const float  PARTICLE_ACCEL_RATE = 800.0f; // feet/second
static const float  MAX_SPEED = 1000.0f; // feet/second
static float MAX_SPREAD = 360.0f; // angles
static float PI = 3.14159265f;

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
        aftt::Nanoseconds(static_cast<int>(ageJitter * 500000000)));

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
    void onDrawFrame(aftt::Datetime const& datetime);
    void drawScene(aftt::Datetime const& datetime);

    uigen::GLWindow* window;
    aftt::Datetime prevTime;
    
    std::vector<Particle> particles;
    agtm::Vector2<float> sourcePosition;
    aftt::DatetimeInterval particleTimer;
    bool paused;
};

Client::Client(uigen::GLWindow& window)
: m_impl(new Impl())
{
    m_impl->paused = false;
    m_impl->window = &window;

    g_sourcePosition = agtm::Vector2<float>(0.0f, 0.0f);
    m_impl->particleTimer = aftt::DatetimeInterval(aftt::Seconds(0), aftt::Nanoseconds(0));
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
    double elapsedSeconds = elapsed.totalSeconds();
    m_impl->particleTimer += elapsed;
    if (m_impl->particleTimer >= aftt::DatetimeInterval(aftt::Seconds(1), aftt::Nanoseconds(0))) {
        m_impl->particleTimer -= aftt::DatetimeInterval(aftt::Seconds(1), aftt::Nanoseconds(0));
    }

    /*
    std::cout << "Client::onDrawFrame ["
        << " datetime: " << datetime
        << " prev: " << m_prevTime
        << " elapsed: " << elapsed
        << " ]" << std::endl;
    */
    
    m_impl->prevTime = datetime;

    // clear the screen
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // initialize the modelview matrix
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

/* TEST TRIANGLE
    GLfloat verticesTest[] = { -50.0f, -50.0f, 0.0f, 50.0f, -50.0f, 0.0f, 0.0f, 50.0f, 0.0f };
    glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_FLOAT, 0, verticesTest);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glDisableClientState(GL_VERTEX_ARRAY);
*/
    
    // for each particle in the particle list
    typedef std::vector<Particle>::iterator ParticleIter;
    GLfloat vertices[] = { -20.0f, -20.0f, 0.0f, 20.0f, -20.0f, 0.0f, 0.0f, 20.0f, 0.0f };
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_FLOAT, 0, vertices);

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
        agtg::ColorRGBA<float>& color = it->color;
        glColor4f(color.red(), color.green(), color.blue(), color.alpha());
        glDrawArrays(GL_TRIANGLES, 0, 3);
        
        // restore the modelview matrix
        glPopMatrix();
        
        if (it->age > it->maxAge) {
            initParticle(*it);
        }
    }
    
    glDisableClientState(GL_VERTEX_ARRAY);

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

} // namespace
