// game_client.h
#ifndef INCLUDED_WRANGLER_GAME_CLIENT_H
#define INCLUDED_WRANGLER_GAME_CLIENT_H

#include <uimac_openglwindow.h>
#include <agtg_mesh.h>
#include <agtm_rect.h>
#include <aftt_datetime.h>

namespace wrangler {
namespace game {

class Client
{
public:
    Client();
    
    ~Client();
    
    void run();
    
    void onResize(aegis::agtm::Rect<float> const& bounds);
    
    void onDrawFrame(aegis::aftt::Datetime const& datetime);
    
private:
    Client(Client const&);
    Client& operator=(Client const&);
    
    class Impl;
    Impl* m_impl;
    
};

} // namespace
} // namespace

#endif // INCLUDED