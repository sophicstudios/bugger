// game_client.h
#ifndef INCLUDED_ATTA_GAME_CLIENT_H
#define INCLUDED_ATTA_GAME_CLIENT_H

#include <uigen_glwindow.h>
#include <agtg_mesh.h>
#include <agtm_rect.h>
#include <aftt_datetime.h>

namespace game {

class Client
{
public:
    Client(uigen::GLWindow& window);
    
    ~Client();
    
    void run();
    
    void pause();
    
    void resume();
    
    void stop();
    
    void onDrawFrame(aftt::Datetime const& datetime);
    
private:
    Client(Client const&);
    Client& operator=(Client const&);
    
    class Impl;
    Impl* m_impl;
    
};

} // namespace

#endif // INCLUDED