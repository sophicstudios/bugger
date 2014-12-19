#include <game_sprite.h>
#include <agtm_matrixutil.h>

namespace game {

GLfloat Sprite::s_vertices[] = {
    -16.0f, -16.0f, 0.0f,
     16.0f, -16.0f, 0.0f,
    -16.0f,  16.0f, 0.0f,
     16.0f,  16.0f, 0.0f
};

Sprite::Sprite(std::shared_ptr<agtr::Image> const& image)
: m_width(image->size().width()),
  m_height(image->size().height()),
  m_frameWidth(1.0f / 8.0f),
  m_frame(0)
{
    glGenTextures(1, &m_texture);
    glBindTexture(GL_TEXTURE_2D, m_texture);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_width, m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, &((*image->data())[0]));
}

Sprite::~Sprite()
{
    glDeleteTextures(1, &m_texture);
}

GLuint Sprite::texture() const
{
    return m_texture;
}

void Sprite::update(agtm::Matrix4<float> const& matrix)
{
    agtm::Matrix4<float> m = agtm::MatrixUtil::transpose(matrix);
    GLfloat frameX = m_frame * m_frameWidth;
    GLfloat frameRight = frameX + m_frameWidth;

    // texture coords are flipped vertically
    GLfloat texCoords[] = {
        frameX, 1.0f,
        frameRight, 1.0f,
        frameX, 0.0f,
        frameRight, 0.0f
    };

    glEnable(GL_TEXTURE_2D);
    glTexCoordPointer(2, GL_FLOAT, 0, texCoords);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    glVertexPointer(3, GL_FLOAT, 0, s_vertices);
    glEnableClientState(GL_VERTEX_ARRAY);

    glPushMatrix();
    glMultMatrixf(m.arr());
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glPopMatrix();

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisable(GL_TEXTURE_2D);
}

} // namespace
