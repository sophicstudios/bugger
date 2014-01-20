// game_client.h
#ifndef INCLUDED_ATTA_GAME_CLIENT_H
#define INCLUDED_ATTA_GAME_CLIENT_H

#include <uigen_glwindow.h>
#include <util_imagepng.h>
#include <agtg_mesh.h>
#include <agtm_rect.h>
#include <aftt_datetime.h>
#include <tr1/memory>

namespace game {

class Client
{
public:
    typedef std::tr1::shared_ptr<util::ImagePNG> ImagePtr;

    Client(uigen::GLWindow& window, ImagePtr const& image);
    
    ~Client();
    
    void run();
    
    void pause();
    
    void resume();
    
    void stop();
    
    void onDrawFrame(aftt::Datetime const& datetime);
    
    void onMouseEvent(agtui::MouseEvent const& event);
    
private:
    Client(Client const&);
    Client& operator=(Client const&);
    
    class Impl;
    Impl* m_impl;
};

} // namespace

#endif // INCLUDED