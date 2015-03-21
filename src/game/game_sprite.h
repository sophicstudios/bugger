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
    Sprite();
    
    Sprite(std::shared_ptr<agtr::Image> const& image, GLuint program);

    virtual ~Sprite();

    virtual void render(agtm::Matrix4<float> const& matrix);

    GLuint texture() const;
    
private:
    GLuint m_vertexBuffer;
    GLuint m_vertexArray;
    GLuint m_texture;
    GLuint m_program;
    GLsizei m_width;
    GLsizei m_height;
    GLfloat m_frameWidth;
    int m_frame;
};

} // namespace

#endif // INCLUDED
