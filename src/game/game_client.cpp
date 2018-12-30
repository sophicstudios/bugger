#include <game_client.h>

#include <agte_surface.h>
#include <agte_orthographiccamera.h>
#include <agtc_transformcomponent.h>
#include <agtc_visual2dcomponent.h>
#include <agtc_spriteanimationcomponent.h>
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
#include <random>

namespace game {

namespace {

const size_t MAX_MATERIALS = 2048;
const size_t MAX_MESHES = 1024;
const size_t MAX_SHADERS = 8;

} // namespace

Client::Client(std::shared_ptr<agtui::GLView> glView,
               std::shared_ptr<aftfs::FileSystem> fileSystem)
: m_glView(glView)
{
    AFTL_LOG_INFO << "Client::Client()" << AFTL_LOG_END;

    // initialize the platform object
    m_platform = std::make_shared<agte::Platform>(fileSystem, m_glView);

    // create the engine
    m_engine = std::make_shared<agte::Engine>(m_platform);
    
    // create the main space and add it to the engine
    m_space = std::make_shared<agte::Space>();
    m_engine->addSpace("main", m_space);

    initSystems();
    initAssets();

    std::shared_ptr<agtui::BoxSizer> sizer(new agtui::BoxSizer(agtui::BoxSizer::Direction_VERTICAL));
    m_glView->setSizer(sizer);
    
    // create the Surface widget
    std::shared_ptr<agte::Surface> surface(new agte::Surface(m_glView->renderingContext()));
    sizer->push_back(surface, agtui::BoxSizer::Flags().sizeMode(agtui::BoxSizer::SizeMode_RELATIVE).size(1.0f));

    m_glView->addChild(surface);

    std::shared_ptr<agte::Camera> camera(new agte::OrthographicCamera(surface));
    m_renderSystem->addCamera(m_space, camera);

    // Load the current rendering context
    std::shared_ptr<agtg::RenderingContext> context = m_glView->renderingContext();
    context->makeCurrent();

    // Load the sprite shader
    size_t spriteShaderId = m_shaderAssets->createAsset();
    agtg::ShaderProgram& spriteShader = m_shaderAssets->assetForId(spriteShaderId);
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
    size_t meshId = m_meshAssets->createAsset();
    agta::Mesh& mesh = m_meshAssets->assetForId(meshId);
    mesh.coords(coords);
    mesh.texCoords(texCoords);

    // create the image for the ant sprite
    aftu::URL imageUrl("images/antsprites.png");
    agtr::ImageLoaderPNG pngLoader;
    std::shared_ptr<agtr::Image> antImage = pngLoader.load(*fileSystem, imageUrl);

    size_t antMaterialId = m_materialAssets->createAsset();
    agta::Material& antMaterial = m_materialAssets->assetForId(antMaterialId);
    antMaterial.texture(*antImage);

    for (size_t i = 0; i < 10000; ++i)
    {
        // create the entities and related components
        agte::Space::Entity e = m_space->createEntity();
        std::stringstream s;
        s << "Entity" << i;
        e.name(s.str());

        agtc::TransformComponent p1;
        float xneg = rand() > (RAND_MAX / 2) ? 1.0f : -1.0f;
        float yneg = rand() > (RAND_MAX / 2) ? 1.0f : -1.0f;
        float x = xneg * 400.0f * (static_cast<float>(rand()) / static_cast<float>(RAND_MAX));
        float y = yneg * 400.0f * (static_cast<float>(rand()) / static_cast<float>(RAND_MAX));
        p1.translate(agtm::Vector3<float>(x, y, 0.0f));
        p1.scale(agtm::Vector3<float>(16.0f, 16.0f, 0.0f));
        e.assign(p1);

        agtc::Visual2dComponent v1;
        v1.shaderId(spriteShaderId);
        v1.meshId(meshId);
        v1.materialId(antMaterialId);
        v1.spriteSize(agtm::Size2d<float>(0.125f, 0.125f));
        e.assign(v1);

        agtc::SpriteAnimationComponent sa1;
        sa1.framesPerSecond(3);
        sa1.numberOfFrames(3);
        e.assign(sa1);
    }

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
    m_glView->addResizeEventHandler("client", resizeEventHandler);

    std::function<void ()> drawEventHandler = std::bind(&Client::onDraw, this);
    m_glView->addDrawEventHandler("client", drawEventHandler);

    std::function<void ()> timerHandler = std::bind(&Client::onTimer, this);
    m_glView->displayTimer()->registerCallback(timerHandler);
}

Client::~Client()
{}

void Client::run()
{
    AFTL_LOG_INFO << "Client::run" << AFTL_LOG_END;
    m_glView->displayTimer()->start();

}

void Client::stop()
{
    AFTL_LOG_TRACE << "Client::stop" << AFTL_LOG_END;
    m_glView->displayTimer()->stop();
}

void Client::onTimer()
{
    AFTL_LOG_TRACE << "Client::onTimer" << AFTL_LOG_END;
    m_engine->update();
}

void Client::onDraw()
{
    m_engine->update();
}

void Client::onResize(agtm::Rect<float> const& bounds)
{
    m_engine->update();
}

void Client::initSystems()
{
    //m_eventSystem = std::make_shared<agte::EventSystem>();
    m_spriteSystem = std::make_shared<agte::SpriteSystem>();
    m_renderSystem = std::make_shared<agte::RenderSystem>();

    //m_engine->addSystem(m_eventSystem);
    m_engine->addSystem(m_spriteSystem);
    m_engine->addSystem(m_renderSystem);
}

void Client::initAssets()
{
    // Create an AssetPool for Materials
    m_materialAssets = std::make_shared<MaterialAssets>(MAX_MATERIALS);
    m_renderSystem->addMaterialAssets(m_space, m_materialAssets);

    // Create an AssetPool for Meshes
    m_meshAssets = std::make_shared<MeshAssets>(MAX_MESHES);
    m_renderSystem->addMeshAssets(m_space, m_meshAssets);

    // Create an AssetPool for Shaders
    m_shaderAssets = std::make_shared<ShaderAssets>(MAX_SHADERS);
    m_renderSystem->addShaderAssets(m_space, m_shaderAssets);
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
