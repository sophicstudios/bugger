#ifndef INCLUDED_ATTA_GAME_CLIENT_H
#define INCLUDED_ATTA_GAME_CLIENT_H

#include <agta_componentmanager.h>
#include <agta_engine.h>
#include <agtui_window.h>
#include <aftfs_filesystem.h>

#include <memory>

namespace game {

class Client
{
public:
    Client(std::shared_ptr<agtui::GLView> glView,
           std::shared_ptr<aftfs::Filesystem> filesystem);
    
    ~Client();
    
    void run();
    
    void stop();

    void onDrawFrame(agtg::RenderingContext& renderingContext);
    
    void onResizeEvent(agtm::Rect<float> const& rect);
    
    //void onMouseEvent(agtui::MouseEvent const& event);
    
private:
    Client(Client const&);
    Client& operator=(Client const&);

    std::shared_ptr<agta::Engine> m_engine;
};

} // namespace

#endif // INCLUDED