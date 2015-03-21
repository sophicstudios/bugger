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

bool createShaderProgram(GLuint* program, aftfs::Filesystem& filesystem,
                         std::vector<std::pair<std::string, GLenum> > const& shaders)
{
    if (shaders.empty()) {
        return false;
    }

    // Create the shader program object
    *program = glCreateProgram();

    std::vector<GLuint> attachedShaders;

    std::vector<std::pair<std::string, GLenum> >::const_iterator it = shaders.begin(), end = shaders.end();
    for (; it != end; ++it) {
        // Create a url of the shader source file location for use in the Filesystem calls
        aftu::URL shaderUrl(it->first);

        // Create filesystem entry and reader to read the file contents
        aftfs::Filesystem::DirectoryEntryPtr entry = filesystem.directoryEntry(shaderUrl);
        aftfs::Filesystem::FileReaderPtr fileReader = filesystem.openFileReader(shaderUrl);

        // Read the file contents into a character buffer
        char* buffer = new char[entry->size()];
        size_t bytesRead = 0;
        fileReader->read(buffer, entry->size(), &bytesRead);

        // Create a shader object
        GLuint shader = glCreateShader(it->second);

        // Assign the shader source to the shader object
        glShaderSource(shader, 1, &buffer, NULL);

        delete [] buffer;

        // Compile the shader
        glCompileShader(shader);

        // Check for compile errors
        GLint compileStatus = 0;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &compileStatus);

        if (compileStatus != GL_TRUE) {
            GLint logLength = 0;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);

            if (logLength > 0) {
                char* log = new char[logLength + 1];
                glGetShaderInfoLog(shader, logLength + 1, NULL, log);

                std::cout << "Shader compiler error ["
                    << " file: " << it->first
                    << " error: " << log
                    << " ]" << std::endl;

                delete [] log;
            }

            std::vector<GLuint>::iterator jit = attachedShaders.begin(), jend = attachedShaders.end();
            for (; jit != jend; ++jit) {
                glDetachShader(*program, *jit);
                glDeleteShader(*jit);
            }

            glDeleteProgram(*program);

            return false;
        }

        // Attach the compiled shader to the program
        glAttachShader(*program, shader);

        attachedShaders.push_back(shader);
    }

    // Link the shader program
    glLinkProgram(*program);

    // Check for link errors
    GLint linkStatus = 0;
    glGetProgramiv(*program, GL_LINK_STATUS, &linkStatus);

    if (linkStatus != GL_TRUE) {
        GLint logLength = 0;
        glGetProgramiv(*program, GL_INFO_LOG_LENGTH, &logLength);

        if (logLength > 0) {
            char* log = new char[logLength + 1];
            glGetProgramInfoLog(*program, logLength, NULL, log);

            std::cout << "Shader program link error ["
                << " log: " << log
                << " ]" << std::endl;

            std::vector<GLuint>::iterator jit = attachedShaders.begin(), jend = attachedShaders.end();
            for (; jit != jend; ++jit) {
                glDetachShader(*program, *jit);
                glDeleteShader(*jit);
            }

            glDeleteProgram(*program);

            return false;
        }
    }

    // Detach shaders as we don't need them anymore after successful link
    std::vector<GLuint>::iterator jit = attachedShaders.begin(), jend = attachedShaders.end();
    for (; jit != jend; ++jit) {
        glDetachShader(*program, *jit);
    }

    // Shader program is loaded and ready to use!
    return true;
}

} // namespace

Client::Client(std::shared_ptr<uigen::GLWindow> const& window,
               std::shared_ptr<aftfs::Filesystem> const& filesystem)
: m_window(window),
  m_paused(true),
  m_positionComponents(100)
{
    // create the image for the ant sprite
    aftu::URL imageUrl("images/antsprites.png");
    agtr::ImageLoaderPNG pngLoader;
    std::shared_ptr<agtr::Image> image = pngLoader.load(*filesystem, imageUrl);

    // create the Sprite object
    m_sprite = SpritePtr(new Sprite(image));

    for (size_t i = 0; i < 10; ++i) {
        // create the entity position
        float x = rand() % static_cast<size_t>(window->bounds().width());
        float y = rand() % static_cast<size_t>(window->bounds().height());
        agtm::Matrix4<float> position = createMatrix(x, y);

        // add the position to the entity
        m_positionComponents.addComponentForEntity(i, position);
    }

    std::vector<std::pair<std::string, GLenum> > shaders;
    shaders.push_back(std::make_pair("shaders/sprite.vsh", GL_VERTEX_SHADER));
    shaders.push_back(std::make_pair("shaders/sprite.fsh", GL_FRAGMENT_SHADER));

    GLuint program = 0;
    if (!createShaderProgram(&program, *filesystem, shaders)) {
        throw aftu::Exception("Unable to load shaders");
    }

    // subscribe to mouse events
    std::function<void (agtui::MouseEvent const&)> mouseEventHandler
        = std::bind(&Client::onMouseEvent, this, std::placeholders::_1);
    
    m_window->registerMouseEventHandler(mouseEventHandler);

    // subscribe to display refresh updates
    std::function<void (aftt::Datetime const&)> displayRefreshHandler
        = std::bind(&Client::onDrawFrame, this, std::placeholders::_1);

    m_window->displayTimer().registerDisplayRefreshHandler(displayRefreshHandler);

    std::function<void (agtm::Rect<float> const&)> resizeEventHandler
        = std::bind(&Client::onResizeEvent, this, std::placeholders::_1);

    m_window->registerResizeEventHandler(resizeEventHandler);
    
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
    std::cout << "Client::onDrawFrame" << std::endl;
    
    m_window->context().preRender();

    doDraw(datetime);
    
    m_window->context().postRender();
}

void Client::onResizeEvent(agtm::Rect<float> const& rect)
{
    std::cout << "Client::onResizeEvent" << std::endl;

    m_window->context().preRender();

    GLfloat width = static_cast<GLfloat>(rect.width());
    GLfloat height = static_cast<GLfloat>(rect.height());

    glViewport(0, 0, static_cast<GLsizei>(width), static_cast<GLsizei>(height));

    std::cout << "width: " << width << std::endl;
    std::cout << "height: " << height << std::endl;

    GLfloat right = width;
    GLfloat left = 0;
    GLfloat top = height;
    GLfloat bottom = 0;
    
    std::cout << "left: " << left << std::endl;
    std::cout << "right: " << right << std::endl;
    std::cout << "top: " << top << std::endl;
    std::cout << "bottom: " << bottom << std::endl;
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    
    glOrtho(left, right, bottom, top, -1.0f, 1.0f);

    doDraw(m_prevTime);

    m_window->context().postRender();
}

void Client::onMouseEvent(agtui::MouseEvent const& event)
{
    std::cout << "Client:onMouseEvent: " << event << std::endl;
    if (event.type() == agtui::MouseEvent::Type_MOUSEDOWN) {
    }
}

void Client::doDraw(aftt::Datetime const& datetime)
{
    std::cout << "Client::doDraw" << std::endl;

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
    glBindTexture(GL_TEXTURE_2D, m_sprite->texture());
    for (size_t i = 0; i < 10; ++i) {
        agtm::Matrix4<float>& position = m_positionComponents.componentForEntity(i);
        m_sprite->render(position);
    }
}

} // namespace
