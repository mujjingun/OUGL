#ifndef TEXTURE_H
#define TEXTURE_H

#include <GL/glew.h>

namespace ou {

class Texture {
    GLuint m_id;

public:
    Texture(GLenum target);
    ~Texture();

    Texture(Texture const&) = delete;
    Texture& operator=(Texture const&) = delete;

    Texture(Texture&& other);
    Texture& operator=(Texture&& other);

    GLuint id() const;

    void setWrapS(GLint param);
    void setWrapT(GLint param);
    void setMinFilter(GLint param);
    void setMagFilter(GLint param);

    void allocateStorage2D(GLsizei levels, GLenum internalFormat, GLsizei width, GLsizei height);
    void setSubImage2D(GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height,
        GLenum format, GLenum type, const void* pixels);

    void use(GLuint unit) const;
};
}

#endif // TEXTURE_H
