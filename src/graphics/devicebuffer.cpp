#include "devicebuffer.h"
#include "texture.h"

#include <algorithm>

namespace ou {

GLuint DeviceBuffer::id() const
{
    return m_id;
}

DeviceBuffer::DeviceBuffer()
{
    glCreateBuffers(1, &m_id);
}

DeviceBuffer::~DeviceBuffer()
{
    glDeleteBuffers(1, &m_id);
}

DeviceBuffer::DeviceBuffer(DeviceBuffer&& other) noexcept
    : m_id(std::exchange(other.m_id, 0))
{
}

DeviceBuffer& DeviceBuffer::operator=(DeviceBuffer&& other) noexcept
{
    glDeleteBuffers(1, &m_id);
    m_id = std::exchange(other.m_id, 0);
}

void DeviceBuffer::allocateStorage(GLsizeiptr size, GLenum usage)
{
    glNamedBufferData(m_id, size, nullptr, usage);
}

void DeviceBuffer::setData(RawBufferView data, GLenum usage)
{
    glNamedBufferData(m_id, data.size(), data.data(), usage);
}

void DeviceBuffer::use(GLenum target, GLuint index, GLintptr offset, GLsizeiptr size)
{
    glBindBufferRange(target, index, m_id, offset, size);
}

void DeviceBuffer::use(GLenum target, GLuint index)
{
    glBindBufferBase(target, index, m_id);
}

void DeviceBuffer::use(GLenum target)
{
    glBindBuffer(target, m_id);
}

void* DeviceBuffer::map(GLenum access)
{
    return glMapNamedBuffer(m_id, access);
}

void DeviceBuffer::unmap()
{
    glUnmapNamedBuffer(m_id);
}

void DeviceBuffer::copyTexture(Texture& tex, GLint level, glm::ivec3 offset, glm::uvec3 size, GLenum format, GLenum type, GLsizei bufSize, GLsizeiptr bufOffset)
{
    int original;
    glGetIntegerv(GL_PIXEL_PACK_BUFFER_BINDING, &original);
    glBindBuffer(GL_PIXEL_PACK_BUFFER, m_id);
    glGetTextureSubImage(tex.id(), level, offset.x, offset.y, offset.z,
        size.x, size.y, size.z, format, type, bufSize, reinterpret_cast<void*>(bufOffset));
    glBindBuffer(GL_PIXEL_PACK_BUFFER, original);
}
}
