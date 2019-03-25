#include "vertexarray.h"

#include "vertexbuffer.h"

#include <algorithm>

namespace ou {

GLuint VertexArray::id() const
{
    return m_id;
}

VertexArray::VertexArray()
{
    glCreateVertexArrays(1, &m_id);
}

VertexArray::~VertexArray()
{
    glDeleteVertexArrays(1, &m_id);
}

VertexArray::VertexArray(VertexArray&& other) noexcept
    : m_id(std::exchange(other.m_id, 0))
{
}

VertexArray& VertexArray::operator=(VertexArray&& other) noexcept
{
    glDeleteVertexArrays(1, &m_id);
    m_id = std::exchange(other.m_id, 0);
}

VertexArray::Attribute VertexArray::enableVertexAttrib(GLuint attribindex)
{
    glEnableVertexArrayAttrib(m_id, attribindex);
    return Attribute(this, attribindex);
}

VertexArray::BufferBinding VertexArray::getBinding(GLuint bindingindex)
{
    return BufferBinding(this, bindingindex);
}

void VertexArray::use() const
{
    glBindVertexArray(m_id);
}

VertexArray::BufferBinding::BufferBinding(VertexArray* array, GLuint index)
    : m_array(array)
    , m_index(index)
{
}

void VertexArray::BufferBinding::bindVertexBuffer(const VertexBuffer& buf, GLintptr offset, GLsizei stride)
{
    glVertexArrayVertexBuffer(m_array->id(), m_index, buf.id(), offset, stride);
}

void VertexArray::BufferBinding::setBindingDivisor(GLuint divisor)
{
    glVertexArrayBindingDivisor(m_array->id(), m_index, divisor);
}

GLuint VertexArray::BufferBinding::index() const
{
    return m_index;
}

VertexArray::Attribute::Attribute(VertexArray* array, GLuint index)
    : m_array(array)
    , m_index(index)
{
}

void VertexArray::Attribute::setFormat(GLuint size, GLenum type, GLboolean normalized, GLuint relativeoffset)
{
    glVertexArrayAttribFormat(m_array->id(), m_index, size, type, normalized, relativeoffset);
}

void VertexArray::Attribute::setBinding(const BufferBinding& binding)
{
    glVertexArrayAttribBinding(m_array->id(), m_index, binding.index());
}

GLuint VertexArray::Attribute::index() const
{
    return m_index;
}
}
