#include "vertexbuffer.h"

#include <algorithm>

namespace ou {

GLuint VertexBuffer::id() const
{
    return m_id;
}

VertexBuffer::VertexBuffer()
{
    glCreateBuffers(1, &m_id);
}

VertexBuffer::~VertexBuffer()
{
    glDeleteBuffers(1, &m_id);
}

VertexBuffer::VertexBuffer(VertexBuffer&& other) noexcept
    : m_id(std::exchange(other.m_id, 0))
{
}

VertexBuffer& VertexBuffer::operator=(VertexBuffer&& other) noexcept
{
    glDeleteBuffers(1, &m_id);
    m_id = std::exchange(other.m_id, 0);
}

void VertexBuffer::setData(RawBufferView data, GLenum usage)
{
    glNamedBufferData(m_id, data.size(), data.data(), usage);
}

void VertexBuffer::use(GLenum target, GLuint index, GLintptr offset, GLsizeiptr size) const
{
    glBindBufferRange(target, index, m_id, offset, size);
}

void VertexBuffer::use(GLenum target, GLuint index) const
{
    glBindBufferBase(target, index, m_id);
}
}
