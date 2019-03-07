#include "vertexarray.h"

#include "vertexbuffer.h"

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

VertexArray::Attribute VertexArray::enableVertexAttrib(GLuint attribindex)
{
    glEnableVertexArrayAttrib(m_id, attribindex);
    return Attribute(this, attribindex);
}

void VertexArray::use() const
{
    glBindVertexArray(m_id);
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

void VertexArray::Attribute::bindVertexBuffer(const VertexBuffer& buf, GLintptr offset, GLsizei stride)
{
    glVertexArrayVertexBuffer(m_array->id(), m_index, buf.id(), offset, stride);
}

void VertexArray::Attribute::setBindingDivisor(GLuint divisor)
{
    glVertexArrayBindingDivisor(m_array->id(), m_index, divisor);
}
}
