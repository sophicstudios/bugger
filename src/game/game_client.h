#ifndef INCLUDED_ATTA_GAME_CLIENT_H
#define INCLUDED_ATTA_GAME_CLIENT_H

#include <uigen_glwindow.h>
#include <aftfs_filesystem.h>
#include <aftt_datetime.h>
#include <agtm_matrix4.h>
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
    
    void onMouseEvent(agtui::MouseEvent const& event);
    
private:
    typedef std::shared_ptr<Sprite> SpritePtr;
    typedef std::vector<SpritePtr> SpriteList;
    typedef std::vector<agtm::Matrix4<float> > PositionList;
    typedef std::vector<PositionList> SpritePositionsList;

    Client(Client const&);
    Client& operator=(Client const&);

    std::shared_ptr<uigen::GLWindow> m_window;
    aftt::Datetime m_prevTime;
    SpriteList m_sprites;
    SpritePositionsList m_spritePositionsList;

    bool m_paused;
};

} // namespace

#endif // INCLUDED