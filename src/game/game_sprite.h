#ifndef INCLUDED_SOPHICSTUDIOS_GAME_SPRITE_H
#define INCLUDED_SOPHICSTUDIOS_GAME_SPRITE_H

#include <game_gl.h>
#include <agta_component.h>
#include <agtm_matrix4.h>
#include <agtr_image.h>
#include <memory>

namespace game {

class Sprite
{
public:
    Sprite(std::shared_ptr<agtr::Image> const& image);

    virtual ~Sprite();

    virtual void update(agtm::Matrix4<float> const& matrix);

    GLuint texture() const;
    
private:
    static GLfloat s_vertices[];

    GLuint m_texture;
    GLsizei m_width;
    GLsizei m_height;
    GLfloat m_frameWidth;
    int m_frame;
};

} // namespace

#endif // INCLUDED
