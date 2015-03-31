#include <game_sprite.h>
#include <agtm_matrixutil.h>

namespace game {

struct Vertex
{
    float position[3];
    float color[4];
};

static const Vertex s_vertices[] = {
    {{-16.0f, -16.0f, 0.0f}, {1.0f, 0.0f, 0.0f, 1.0f}},
    {{ 16.0f, -16.0f, 0.0f}, {0.0f, 1.0f, 0.0f, 1.0f}},
    {{-16.0f,  16.0f, 0.0f}, {0.0f, 0.0f, 1.0f, 1.0f}},
    {{ 16.0f,  16.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 1.0f}}
};

Sprite::Sprite(std::shared_ptr<agtr::Image> const& image)
: m_width(image->size().width()),
  m_height(image->size().height()),
  m_frameWidth(1.0f / 8.0f),
  m_frame(0)
{
    glGenBuffers(1, &m_vertexBuffer);
    glBindBuffer(GL_VERTEX_ARRAY, m_vertexBuffer);
    glBufferData(GL_VERTEX_ARRAY, sizeof(s_vertices), s_vertices, GL_STATIC_DRAW);

    //glGenVertexArraysAPPLE(1, &m_vertexArray);
    //glBindVertexArrayAPPLE(m_vertexArray);
    //glEnableVertexAttribArray(m_vertexArray);
    //glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
    //glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

    glGenTextures(1, &m_texture);
    glBindTexture(GL_TEXTURE_2D, m_texture);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_width, m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, &((*image->data())[0]));
}

Sprite::~Sprite()
{
    glDeleteTextures(1, &m_texture);
    glDeleteVertexArraysAPPLE(1, &m_vertexArray);
    glDeleteBuffers(1, &m_vertexBuffer);
}

GLuint Sprite::texture() const
{
    return m_texture;
}

void Sprite::render(agtm::Matrix4<float> const& matrix)
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

    //glEnable(GL_TEXTURE_2D);
    //glTexCoordPointer(2, GL_FLOAT, 0, texCoords);
    //glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    //glVertexPointer(3, GL_FLOAT, 0, s_vertices);
    //glEnableClientState(GL_VERTEX_ARRAY);

    //glVertexAttribPointer(<#GLuint index#>, <#GLint size#>, <#GLenum type#>, <#GLboolean normalized#>, <#GLsizei stride#>, <#const GLvoid *pointer#>)
    //glPushMatrix();
    //glMultMatrixf(m.arr());
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    //glPopMatrix();

    //glDisableClientState(GL_VERTEX_ARRAY);
    //glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    //glDisable(GL_TEXTURE_2D);
}

} // namespace
