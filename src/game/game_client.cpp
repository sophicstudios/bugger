#include <game_client.h>
#include <game_sprite.h>

#include <agta_platform.h>
#include <agta_rendersystem.h>
#include <agta_space.h>
#include <agta_transformcomponent.h>
#include <agtg_colorrgba.h>
#include <agtg_orthographiccamera.h>
#include <agtm_matrix3.h>
#include <agtm_vector2.h>
#include <agtr_image.h>
#include <agtr_imageloaderpng.h>
#include <aftt_datetimeinterval.h>
#include <aftt_systemtime.h>

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

Client::Client(std::shared_ptr<agtui::GLWindow> window,
               std::shared_ptr<aftfs::Filesystem> filesystem)
{
    // initialize the platform object
    std::shared_ptr<agta::Platform> platform(new agta::Platform(filesystem, window));

    // create the engine
    m_engine = std::shared_ptr<agta::Engine>(new agta::Engine(platform));

    // create the EventSystem and add it to the engine
    //std::shared_ptr<agta::System> eventSystem(new agta::EventSystem());
    //m_engine->registerSystem(eventSystem);

    // create the RenderSystem and add it to the engine
    std::shared_ptr<agta::System> renderSystem(new agta::RenderSystem(platform));
    m_engine->registerSystem(renderSystem);

    // create the main space and add it to the engine
    std::shared_ptr<agta::Space> space(new agta::Space());
    m_engine->addSpace("main", space);

    std::shared_ptr<agtui::GLView> view(new agtui::GLView(window->renderingContext()));
    window->addChild(view);

    std::shared_ptr<agtui::BoxSizer> sizer(new agtui::BoxSizer());

    // create the ECSView widget

    // create a Box sizer to control the ECSView size

    // add the ECSView to the sizer

    // create a GLSurface widget
    // the GLSurface is used to define the viewport for the camera. The
    // camera will need to have knowledge of this viewport to calculate the
    // correct projection matrix

    // create the Box sizer to set the size of the GLSurface

    // set the window's sizer

    //std::shared_ptr<agtg::Camera> camera(new agtg::OrthographicCamera(glSurface->bounds()));
    //space->addCamera(camera);

    //***** PHYSICS SYSTEM *****
    // A PhysicsSystem works on entities with components of type:
    //    TransformComponent, PhysicsComponent

    //EntityTypeMap<agta::TransformComponent, agta::PhysicsComponent> transformPhysicsEntityMap;

    //std::shared_ptr<agta::System> physicsSystem(new agta::PhysicsSystem(platform));

    //***** RENDER SYSTEM *****
    // A RenderSystem works on entities with components of type:
    // TransformComponent, VisualComponent

    //EntityTypeMap<agta::TransformComponent, agta::VisualComponent> transformVisualEntityMap;

    // create the component managers for the main space
    typedef agta::ComponentPool<agta::TransformComponent> TransformComponents;
    std::shared_ptr<TransformComponents> transformComponents(new TransformComponents(space));
    
    // register the component managers with the respective systems
    //renderSystem->registerTransform2dComponents(mainSpace, transform2dManager);
    //renderSystem->registerVisual2dComponents(mainSpace, visual2dManager);

    // create the entities and related components
    agta::Entity circle = space->createEntity();

    agta::TransformComponent& pos = transformComponents->createComponent(circle);
    pos.x(10);
    pos.y(20);

    space->destroyEntity(circle);

    // create the image for the ant sprite
    //aftu::URL imageUrl("images/antsprites.png");
    //agtr::ImageLoaderPNG pngLoader;
    //std::shared_ptr<agtr::Image> image = pngLoader.load(*filesystem, imageUrl);

    // create the Sprite object
    //m_sprite = SpritePtr(new Sprite(image));

    /*
    for (size_t i = 0; i < 10; ++i) {
        // create the entity position
        float x = rand() % static_cast<size_t>(window->bounds().width());
        float y = rand() % static_cast<size_t>(window->bounds().height());
        agtm::Matrix4<float> position = createMatrix(x, y);

        // add the position to the entity
        m_positionComponents.addComponentForEntity(i, position);
    }
    */

    //std::function<void (agtm::Rect<float> const&, agtg::RenderingContext&)> resizeEventHandler
    //    = std::bind(&RenderSystem::onResizeEvent, this, std::placeholders::_1, std::placeholders::_2);

    //platform->glWindow()->registerResizeEventHandler(resizeEventHandler);

    //std::function<void (agtg::RenderingContext&)> drawEventHandler
    //    = std::bind(&Client::onDrawEvent, this, std::placeholders::_1);

    // show the window
    window->show();
}

Client::~Client()
{}

void Client::run()
{
    std::cout << "Client::run" << std::endl;
}

void Client::stop()
{
    std::cout << "Client::stop" << std::endl;
}

/*
void Client::onMouseEvent(agtui::MouseEvent const& event)
{
    std::cout << "Client:onMouseEvent: " << event << std::endl;
}
*/

/*
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

    // update render objects
    glBindTexture(GL_TEXTURE_2D, m_sprite->texture());
    for (size_t i = 0; i < 10; ++i) {
        agtm::Matrix4<float>& position = m_positionComponents.componentForEntity(i);
        m_sprite->render(position);
    }
}
*/

} // namespace
