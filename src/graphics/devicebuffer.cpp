#include "devicebuffer.h"

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
}
