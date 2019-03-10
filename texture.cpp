#include "texture.h"

#include <algorithm>

namespace ou {

GLuint Texture::id() const
{
    return m_id;
}

void Texture::setWrapS(GLint param)
{
    glTextureParameteri(m_id, GL_TEXTURE_WRAP_S, param);
}

void Texture::setWrapT(GLint param)
{
    glTextureParameteri(m_id, GL_TEXTURE_WRAP_T, param);
}

void Texture::setMinFilter(GLint param)
{
    glTextureParameteri(m_id, GL_TEXTURE_MIN_FILTER, param);
}

void Texture::setMagFilter(GLint param)
{
    glTextureParameteri(m_id, GL_TEXTURE_MAG_FILTER, param);
}

void Texture::allocateStorage2D(GLsizei levels, GLenum internalFormat, GLsizei width, GLsizei height)
{
    glTextureStorage2D(m_id, levels, internalFormat, width, height);
}

void Texture::setSubImage2D(GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height,
    GLenum format, GLenum type, const void* pixels){
    glTextureSubImage2D(m_id, level, xoffset, yoffset, width, height, format, type, pixels);
}

void Texture::use(GLuint unit) const
{
    glBindTextureUnit(unit, m_id);
}

Texture::Texture(GLenum target)
{
    glCreateTextures(target, 1, &m_id);
}

Texture::~Texture()
{
    glDeleteTextures(1, &m_id);
}

Texture::Texture(Texture&& other)
    : m_id(std::exchange(other.m_id, 0))
{
}

Texture& Texture::operator=(Texture&& other)
{
    glDeleteTextures(1, &m_id);
    m_id = std::exchange(other.m_id, 0);
}
}
