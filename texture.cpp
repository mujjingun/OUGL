#include "texture.h"

#include <algorithm>

namespace ou {

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
