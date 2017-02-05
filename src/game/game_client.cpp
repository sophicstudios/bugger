#include <game_client.h>

#include <agte_platform.h>
#include <agte_rendersystem.h>
#include <agte_space.h>
#include <agte_surface.h>
#include <agte_orthographiccamera.h>
#include <agtc_transformcomponent.h>
#include <agtc_visual2dcomponent.h>
#include <agta_sprite2dmaterial.h>
#include <agta_mesh.h>
#include <agtg_colorrgba.h>
#include <agtg_shaderprogram.h>
#include <agtg_vertex.h>
#include <agtui_boxsizer.h>
#include <agtm_matrix3.h>
#include <agtm_vector2.h>
#include <agtm_vector3.h>
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

Client::Client(std::shared_ptr<agtui::GLView> glView,
               std::shared_ptr<aftfs::FileSystem> fileSystem)
{
    // initialize the platform object
    std::shared_ptr<agte::Platform> platform(new agte::Platform(fileSystem, glView));

    // create the engine
    m_engine = std::shared_ptr<agte::Engine>(new agte::Engine(platform));

    // create the EventSystem and add it to the engine
    //std::shared_ptr<agta::System> eventSystem(new agta::EventSystem());
    //m_engine->registerSystem(eventSystem);

    // create the RenderSystem and add it to the engine
    std::shared_ptr<agte::RenderSystem> renderSystem(new agte::RenderSystem(platform));
    m_engine->registerSystem(renderSystem);

    // create the main space and add it to the engine
    std::shared_ptr<agte::Space> space(new agte::Space());
    m_engine->addSpace("main", space);

    std::shared_ptr<agtui::BoxSizer> sizer(new agtui::BoxSizer(agtui::BoxSizer::Direction_VERTICAL));
    glView->setSizer(sizer);
    
    // create the Surface widget
    std::shared_ptr<agte::Surface> surface(new agte::Surface(glView->renderingContext()));
    sizer->push_back(surface, agtui::BoxSizer::Flags().sizeMode(agtui::BoxSizer::SizeMode_RELATIVE).size(1.0f));

    glView->addChild(surface);

    std::shared_ptr<agte::Camera> camera(new agte::OrthographicCamera(surface));
    renderSystem->addCamera(space, camera);

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
    typedef agte::ComponentPool<agtc::TransformComponent> TransformComponents;
    std::shared_ptr<TransformComponents> transformComponents(new TransformComponents(space));
    
    // register the component managers with the respective systems
    renderSystem->addTransformComponents(space, transformComponents);

    typedef agte::ComponentPool<agtc::Visual2dComponent> Visual2dComponents;
    std::shared_ptr<Visual2dComponents> visual2dComponents(new Visual2dComponents(space));

    renderSystem->addVisual2dComponents(space, visual2dComponents);

    // create the sprite material for the entities
    glView->renderingContext()->makeCurrent();

    std::shared_ptr<agtg::ShaderProgram> shaderProgram(new agtg::ShaderProgram());
    
    std::shared_ptr<agta::Sprite2dMaterial> sprite(new agta::Sprite2dMaterial(fileSystem));

    std::vector<agtg::Vertex<float> > vertices;
    vertices.push_back(agtg::Vertex<float>(agtm::Vector3<float>(-1.0f, -1.0f, 0.0f)));
    vertices.push_back(agtg::Vertex<float>(agtm::Vector3<float>( 1.0f, -1.0f, 0.0f)));
    vertices.push_back(agtg::Vertex<float>(agtm::Vector3<float>(-1.0f,  1.0f, 0.0f)));
    vertices.push_back(agtg::Vertex<float>(agtm::Vector3<float>( 1.0f,  1.0f, 0.0f)));

    std::shared_ptr<agta::Mesh> mesh(new agta::Mesh(vertices));
    // mesh->vertices(vertices);
    // mesh->textureCoordinates(texCoords);
    // mesh->normals(normals);

    // create the entities and related components
    agte::Entity e1 = space->createEntity();

    agtc::TransformComponent& p1 = transformComponents->createComponent(e1);
    p1.translate(agtm::Vector3<float>(50.0f, 0.0f, 0.0f));
    p1.scale(agtm::Vector3<float>(16.0f, 16.0f, 0.0f));

    agtc::Visual2dComponent& v1 = visual2dComponents->createComponent(e1);
    v1.material(sprite);
    v1.mesh(mesh);

    agte::Entity e2 = space->createEntity();

    agtc::TransformComponent& p2 = transformComponents->createComponent(e2);
    p2.translate(agtm::Vector3<float>(-50.0f, 0.0f, 0.0f));
    p2.scale(agtm::Vector3<float>(64.0f, 64.0f, 0.0f));

    agtc::Visual2dComponent& v2 = visual2dComponents->createComponent(e2);
    v2.material(sprite);
    v2.mesh(mesh);
    
    //space->destroyEntity(circle);

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

    std::function<void (agtm::Rect<float> const&)> resizeEventHandler
        = std::bind(&Client::onResize, this, std::placeholders::_1);

    glView->addResizeEventHandler("client", resizeEventHandler);

    std::function<void ()> drawEventHandler = std::bind(&Client::onDraw, this);

    glView->addDrawEventHandler("client", drawEventHandler);
}

Client::~Client()
{}

void Client::run()
{
    std::cout << "Client::run" << std::endl;
    //onDraw();
}

void Client::stop()
{
    std::cout << "Client::stop" << std::endl;
}

void Client::onDraw()
{
    m_engine->update();
}

void Client::onResize(agtm::Rect<float> const& bounds)
{
    m_engine->update();
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
