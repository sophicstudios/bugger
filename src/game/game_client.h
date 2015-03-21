#ifndef INCLUDED_ATTA_GAME_CLIENT_H
#define INCLUDED_ATTA_GAME_CLIENT_H

#include <uigen_glwindow.h>
#include <aftfs_filesystem.h>
#include <aftt_datetime.h>
#include <agtm_matrix4.h>
#include <agta_engine.h>
#include <agta_componentmanager.h>
#include <memory>

namespace game {

class Sprite;

class Client
{
public:
    Client(std::shared_ptr<uigen::GLWindow> const& window,
           std::shared_ptr<aftfs::Filesystem> const& filesystem);
    
    ~Client();
    
    void run();
    
    void stop();

    void onDrawFrame(aftt::Datetime const& datetime);
    
    void onResizeEvent(agtm::Rect<float> const& rect);
    
    void onMouseEvent(agtui::MouseEvent const& event);
    
private:
    typedef std::shared_ptr<Sprite> SpritePtr;

    Client(Client const&);
    Client& operator=(Client const&);

    void doDraw(aftt::Datetime const& datetime);
    
    std::shared_ptr<uigen::GLWindow> m_window;
    aftt::Datetime m_prevTime;

    SpritePtr m_sprite;

    std::shared_ptr<agta::Engine> m_engine;
    agta::ComponentManager<agtm::Matrix4<float> > m_positionComponents;

    bool m_paused;
};

} // namespace

#endif // INCLUDED