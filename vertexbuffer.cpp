#include "vertexbuffer.h"

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

void VertexBuffer::setData(RawBufferView data, GLenum usage)
{
    glNamedBufferData(m_id, data.size(), data.data(), usage);
}

}
