#include <game_client.h>

#include <agte_platform.h>
#include <agte_rendersystem.h>
#include <agte_space.h>
#include <agte_surface.h>
#include <agte_orthographiccamera.h>
#include <agtc_transformcomponent.h>
#include <agtc_visual2dcomponent.h>
#include <agta_assetpool.h>
#include <agta_material.h>
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
#include <aftl_logger.h>
#include <functional>
#include <algorithm>
#include <string>
#include <cmath>
#include <cstdlib>

namespace game {

namespace {

const size_t MAX_MATERIALS = 2048;
const size_t MAX_MESHES = 1024;
const size_t MAX_SHADERS = 8;

} // namespace

Client::Client(std::shared_ptr<agtui::GLView> glView,
               std::shared_ptr<aftfs::FileSystem> fileSystem)
{
    AFTL_LOG_INFO << "Client::Client()" << AFTL_LOG_END;
    
    //aftl::log_info() << "Client::Client()" << aftl::log_end();

    // initialize the platform object
    std::shared_ptr<agte::Platform> platform(new agte::Platform(fileSystem, glView));

    // create the engine
    m_engine = std::shared_ptr<agte::Engine>(new agte::Engine(platform));

    // create the main space and add it to the engine
    std::shared_ptr<agte::Space> space(new agte::Space());
    m_engine->addSpace("main", space);

    // create the EventSystem and add it to the engine
    //std::shared_ptr<agta::System> eventSystem(new agta::EventSystem());
    //m_engine->registerSystem(eventSystem);

    // create the RenderSystem and add it to the engine
    std::shared_ptr<agte::RenderSystem> renderSystem(new agte::RenderSystem(platform));
    m_engine->registerSystem(renderSystem);

    std::shared_ptr<agtui::BoxSizer> sizer(new agtui::BoxSizer(agtui::BoxSizer::Direction_VERTICAL));
    glView->setSizer(sizer);
    
    // create the Surface widget
    std::shared_ptr<agte::Surface> surface(new agte::Surface(glView->renderingContext()));
    sizer->push_back(surface, agtui::BoxSizer::Flags().sizeMode(agtui::BoxSizer::SizeMode_RELATIVE).size(1.0f));

    glView->addChild(surface);

    std::shared_ptr<agte::Camera> camera(new agte::OrthographicCamera(surface));
    renderSystem->addCamera(space, camera);

    // Create an AssetPool for Materials
    typedef agta::AssetPool<agta::Material> MaterialAssets;
    std::shared_ptr<MaterialAssets> materialAssets(new MaterialAssets(MAX_MATERIALS));
    renderSystem->addMaterialAssets(space, materialAssets);

    // Create an AssetPool for Meshes
    typedef agta::AssetPool<agta::Mesh> MeshPool;
    std::shared_ptr<MeshPool> meshAssets(new MeshPool(MAX_MESHES));
    renderSystem->addMeshAssets(space, meshAssets);

    // Create an AssetPool for Shaders
    typedef agta::AssetPool<agtg::ShaderProgram> ShaderPool;
    std::shared_ptr<ShaderPool> shaderAssets(new ShaderPool(MAX_SHADERS));
    renderSystem->addShaderAssets(space, shaderAssets);

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

    typedef agte::ComponentPool<agtc::Visual2dComponent> Visual2dComponents;
    std::shared_ptr<Visual2dComponents> visual2dComponents(new Visual2dComponents(space));

    // register the component managers with the respective systems
    renderSystem->addTransformComponents(space, transformComponents);
    renderSystem->addVisual2dComponents(space, visual2dComponents);

    // Load the current rendering context
    std::shared_ptr<agtg::RenderingContext> context = glView->renderingContext();
    context->makeCurrent();

    // Load the sprite shader
    size_t spriteShaderId = shaderAssets->createAsset();
    agtg::ShaderProgram& spriteShader = shaderAssets->assetForId(spriteShaderId);
    spriteShader.addVertexShader(*fileSystem, "shaders/sprite.vsh");
    spriteShader.addFragmentShader(*fileSystem, "shaders/sprite.fsh");
    spriteShader.link();

    // Create a square mesh
    std::vector<agtm::Vector3<float> > coords;
    coords.push_back(agtm::Vector3<float>(-1.0f, -1.0f, 0.0f));
    coords.push_back(agtm::Vector3<float>( 1.0f, -1.0f, 0.0f));
    coords.push_back(agtm::Vector3<float>(-1.0f,  1.0f, 0.0f));
    coords.push_back(agtm::Vector3<float>( 1.0f,  1.0f, 0.0f));

    std::vector<agtm::Vector2<float> > texCoords;
    texCoords.push_back(agtm::Vector2<float>(0.0f, 0.125f));
    texCoords.push_back(agtm::Vector2<float>(0.125f, 0.125f));
    texCoords.push_back(agtm::Vector2<float>(0.0f, 0.0f));
    texCoords.push_back(agtm::Vector2<float>(0.125f, 0.0f));

    // Create an mesh asset for rendering the vertices
    size_t meshId = meshAssets->createAsset();
    agta::Mesh& mesh = meshAssets->assetForId(meshId);
    mesh.coords(coords);
    mesh.texCoords(texCoords);

    // create the image for the ant sprite
    aftu::URL imageUrl("images/antsprites.png");
    agtr::ImageLoaderPNG pngLoader;
    std::shared_ptr<agtr::Image> antImage = pngLoader.load(*fileSystem, imageUrl);

    //agtg::Texture antTexture(image);

    size_t antMaterialId = materialAssets->createAsset();
    agta::Material& antMaterial = materialAssets->assetForId(antMaterialId);
    antMaterial.texture(*antImage);

    // create the sprite material for the entities
    //std::shared_ptr<agta::Sprite2dMaterial> sprite(new agta::Sprite2dMaterial(shaderProgram));

    // create the entities and related components
    agte::Entity e1 = space->createEntity();

    agtc::TransformComponent& p1 = transformComponents->createComponent(e1);
    p1.translate(agtm::Vector3<float>(100.0f, 0.0f, 0.0f));
    p1.scale(agtm::Vector3<float>(32.0f, 32.0f, 0.0f));

    agtc::Visual2dComponent& v1 = visual2dComponents->createComponent(e1);
    v1.shaderId(spriteShaderId);
    v1.meshId(meshId);
    v1.materialId(antMaterialId);

    agte::Entity e2 = space->createEntity();

    agtc::TransformComponent& p2 = transformComponents->createComponent(e2);
    p2.translate(agtm::Vector3<float>(-50.0f, 0.0f, 0.0f));
    p2.scale(agtm::Vector3<float>(64.0f, 64.0f, 0.0f));

    agtc::Visual2dComponent& v2 = visual2dComponents->createComponent(e2);
    v2.shaderId(spriteShaderId);
    v2.meshId(meshId);
    v2.materialId(antMaterialId);

    //space->destroyEntity(circle);

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
    AFTL_LOG_INFO << "Client::run" << AFTL_LOG_END;
}

void Client::stop()
{
    AFTL_LOG_INFO << "Client::stop" << AFTL_LOG_END;
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
