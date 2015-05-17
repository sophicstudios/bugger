#ifndef INCLUDED_ATTA_GAME_CLIENT_H
#define INCLUDED_ATTA_GAME_CLIENT_H

#include <aftfs_filesystem.h>
#include <aftt_datetime.h>
#include <agtm_matrix4.h>
#include <agta_engine.h>
#include <agta_componentmanager.h>
#include <agta_glwindow.h>
#include <memory>

namespace game {

class Sprite;

class Client
{
public:
    Client(std::shared_ptr<agta::GLWindow> const& window,
           std::shared_ptr<aftfs::Filesystem> const& filesystem);
    
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