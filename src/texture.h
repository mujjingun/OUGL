#ifndef TEXTURE_H
#define TEXTURE_H

#include <GL/glew.h>

namespace ou {

class Texture {
    GLuint m_id;

public:
    Texture();
    Texture(GLenum target);
    ~Texture();

    Texture(Texture const&) = delete;
    Texture& operator=(Texture const&) = delete;

    Texture(Texture&& other) noexcept;
    Texture& operator=(Texture&& other) noexcept;

    GLuint id() const;

    void setWrapS(GLint param);
    void setWrapT(GLint param);
    void setMinFilter(GLint param);
    void setMagFilter(GLint param);

    void allocateStorage2D(GLsizei levels, GLenum internalFormat, GLsizei width, GLsizei height);
    void uploadTexture2D(GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height,
        GLenum format, GLenum type, const void* pixels);

    void allocateStoarge3D(GLsizei levels, GLenum internalFormat, GLsizei width, GLsizei height, GLsizei depth);
    void uploadTexture3D(GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth,
        GLenum format, GLenum type, const void* pixels);

    void use(GLuint unit) const;
    void useAsImage(GLuint unit, GLint level, GLboolean layered, GLint layer, GLenum access, GLenum format);

    void saveToImage(GLenum format, GLenum type, GLsizei width, GLsizei height, GLsizei depth, const char* filename) const;
};
}

#endif // TEXTURE_H
