#include <game_client.h>
#include <game_gl.h>
#include <game_sprite.h>

#include <uigen_displaytimer.h>

#include <agtg_colorrgba.h>
#include <agtm_vector2.h>
#include <agtm_matrix3.h>
#include <agtr_image.h>
#include <agtr_imageloaderpng.h>

#include <aftt_systemtime.h>
#include <aftt_datetimeinterval.h>

#include <iostream>
#include <functional>
#include <algorithm>
#include <cmath>
#include <cstdlib>

namespace game {

namespace {

//static const size_t NUM_PARTICLES = 20;
//static const float  PARTICLE_ACCEL_RATE = 800.0f; // feet/second
//static const float  MAX_SPEED = 1000.0f; // feet/second
//static float MAX_SPREAD = 360.0f; // angles
//static float PI = 3.14159265f;

agtm::Matrix4<float> createMatrix(float x, float y)
{
    return agtm::Matrix4<float>(
        1.0f, 0.0f, 0.0f,    x,
        0.0f, 1.0f, 0.0f,    y,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f);
}

} // namespace

Client::Client(std::shared_ptr<uigen::GLWindow> const& window,
               std::shared_ptr<aftfs::Filesystem> const& filesystem)
: m_window(window),
  m_paused(true)
{
    // create the image for the ant sprite
    aftu::URL imageUrl("images/antsprites.png");
    agtr::ImageLoaderPNG pngLoader;
    std::shared_ptr<agtr::Image> image = pngLoader.load(*filesystem, imageUrl);

    // create the sprite object
    m_sprites.push_back(SpritePtr(new Sprite(image)));

    // create some positions for a couple ants
    PositionList positionList;
    positionList.push_back(createMatrix(0.0f, 0.f));
    positionList.push_back(createMatrix(50.0f, 50.0f));
    positionList.push_back(createMatrix(-70.0f, 30.0f));

    // associate the sprite with these positions
    m_spritePositionsList.push_back(positionList);

    // subscribe to mouse events
    std::function<void (agtui::MouseEvent const&)> mouseEventHandler
        = std::bind(&Client::onMouseEvent, this, std::placeholders::_1);
    
    m_window->registerMouseEventHandler(mouseEventHandler);

    // subscribe to display refresh updates
    std::function<void (aftt::Datetime const&)> displayRefreshHandler
        = std::bind(&Client::onDrawFrame, this, std::placeholders::_1);

    m_window->displayTimer().registerDisplayRefreshHandler(displayRefreshHandler);

    // show the window
    m_window->show();
}

Client::~Client()
{
    if (!m_paused) {
        m_window->displayTimer().stop();
    }
}

void Client::run()
{
    std::cout << "Client::run" << std::endl;
    if (m_paused) {
        m_paused = false;

        m_prevTime = aftt::SystemTime::nowAsDatetimeUTC();

        m_window->displayTimer().start();
    }
}

void Client::stop()
{
    std::cout << "Client::stop" << std::endl;
    if (!m_paused) {
        m_paused = true;

        m_window->displayTimer().stop();
    }
}

void Client::onDrawFrame(aftt::Datetime const& datetime)
{
    m_window->context().preRender();

    glMatrixMode(GL_MODELVIEW);

    aftt::DatetimeInterval elapsed = datetime - m_prevTime;

    m_prevTime = datetime;

    // clear the screen
    glClearColor(0.9f, 0.9f, 0.9f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // initialize the modelview matrix
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    glEnable(GL_CULL_FACE);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // update input

    // update post input transformations

    // update physics

    // update post physics transformations

    // update render objects
    for (size_t i = 0; i < m_sprites.size(); ++i) {
        PositionList& positions = m_spritePositionsList[i];
        Sprite& sprite = *m_sprites[i];

        glBindTexture(GL_TEXTURE_2D, sprite.texture());

        for (size_t j = 0; j < positions.size(); ++j) {
            sprite.update(positions[j]);
        }
    }


    m_window->context().postRender();
}

void Client::onMouseEvent(agtui::MouseEvent const& event)
{
    std::cout << "mouse event: " << event << std::endl;
    if (event.type() == agtui::MouseEvent::Type_MOUSEDOWN) {
    }
}

} // namespace
