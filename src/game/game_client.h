#ifndef INCLUDED_ATTA_GAME_CLIENT_H
#define INCLUDED_ATTA_GAME_CLIENT_H

#include <agte_engine.h>
#include <agtui_glview.h>
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

    void onDraw();
    
    void onResize(agtm::Rect<float> const& bounds);
    
    //void onMouseEvent(agtui::MouseEvent const& event);
    
private:
    Client(Client const&);
    Client& operator=(Client const&);

    std::shared_ptr<agte::Engine> m_engine;
};

} // namespace

#endif // INCLUDED
