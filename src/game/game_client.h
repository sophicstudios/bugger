#ifndef INCLUDED_BUGGER_GAME_CLIENT_H
#define INCLUDED_BUGGER_GAME_CLIENT_H

#include <agte_engine.h>
#include <agte_eventsystem.h>
#include <agte_platform.h>
#include <agte_pool.h>
#include <agte_rendersystem.h>
#include <agte_space.h>
#include <agte_spritesystem.h>
#include <agtui_glview.h>
#include <aftfs_filesystem.h>
#include <memory>

namespace game {

class Client
{
public:
    Client(std::shared_ptr<agtui::GLView> glView,
           std::shared_ptr<aftfs::FileSystem> fileSystem);
    
    ~Client();
    
    void run();
    
    void stop();

    void onTimer();

    void onDraw();
    
    void onResize(agtm::Rect<float> const& bounds);
    
    //void onMouseEvent(agtui::MouseEvent const& event);
    
private:
    Client(Client const&);
    Client& operator=(Client const&);

    typedef agta::AssetPool<agta::Material> MaterialAssets;
    typedef agta::AssetPool<agta::Mesh> MeshAssets;
    typedef agta::AssetPool<agtg::ShaderProgram> ShaderAssets;

    void initSystems();
    void initAssets();

    void createAnts();
    
    std::shared_ptr<agtui::GLView> m_glView;
    std::shared_ptr<agte::Platform> m_platform;
    std::shared_ptr<agte::Engine> m_engine;
    std::shared_ptr<agte::Space> m_space;
    std::shared_ptr<agte::EventSystem> m_eventSystem;
    std::shared_ptr<agte::SpriteSystem> m_spriteSystem;
    std::shared_ptr<agte::RenderSystem> m_renderSystem;

    std::shared_ptr<MaterialAssets> m_materialAssets;
    std::shared_ptr<MeshAssets> m_meshAssets;
    std::shared_ptr<ShaderAssets> m_shaderAssets;
};

} // namespace

#endif // INCLUDED
