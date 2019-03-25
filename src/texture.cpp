#include "texture.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include <algorithm>
#include <iostream>
#include <vector>

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

void Texture::uploadTexture2D(GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height,
    GLenum format, GLenum type, const void* pixels)
{
    glTextureSubImage2D(m_id, level, xoffset, yoffset, width, height, format, type, pixels);
}

void Texture::allocateStoarge3D(GLsizei levels, GLenum internalFormat, GLsizei width, GLsizei height, GLsizei depth)
{
    glTextureStorage3D(m_id, levels, internalFormat, width, height, depth);
}

void Texture::uploadTexture3D(GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth,
    GLenum format, GLenum type, const void* pixels)
{
    glTextureSubImage3D(m_id, level, xoffset, yoffset, zoffset, width, height, depth, format, type, pixels);
}

void Texture::use(GLuint unit) const
{
    glBindTextureUnit(unit, m_id);
}

void Texture::useAsImage(GLuint unit, GLint level, GLboolean layered, GLint layer, GLenum access, GLenum format)
{
    glBindImageTexture(unit, m_id, level, layered, layer, access, format);
}

void Texture::saveToImage(GLenum format, GLenum type, GLsizei width, GLsizei height, GLsizei depth, const char *filename) const
{
    std::cout << width * height * depth * 4 / (1024 * 1024) << "MB" << std::endl;

    std::vector<GLfloat> buf(width * height * depth);
    glGetTextureImage(m_id, 0, format, type, buf.size() * 4, buf.data());

    std::vector<unsigned char> buf2(width * height);

    for (int l = 0; l < depth; ++l) {
        std::string fn = filename + std::to_string(l) + ".png";
        for (int i = 0; i < height; ++i) {
            for (int j = 0; j < width; ++j) {
                buf2[i * width + j] = static_cast<unsigned char>(buf[l * (width * height) + i * width + j] * 256);
            }
        }

        stbi_write_png(fn.c_str(), width, height, 1, buf2.data(), width);
    }

    std::cout << "Done." << std::endl;
}

Texture::Texture()
    : m_id(0)
{
}

Texture::Texture(GLenum target)
{
    glCreateTextures(target, 1, &m_id);
}

Texture::~Texture()
{
    glDeleteTextures(1, &m_id);
}

Texture::Texture(Texture&& other) noexcept
    : m_id(std::exchange(other.m_id, 0))
{
}

Texture& Texture::operator=(Texture&& other) noexcept
{
    glDeleteTextures(1, &m_id);
    m_id = std::exchange(other.m_id, 0);
}
}
