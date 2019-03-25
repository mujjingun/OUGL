#include "renderbuffer.h"

#include <algorithm>

namespace ou {

GLuint RenderBuffer::id() const
{
    return m_id;
}

RenderBuffer::RenderBuffer()
{
    glCreateRenderbuffers(1, &m_id);
}

RenderBuffer::~RenderBuffer()
{
    glDeleteRenderbuffers(1, &m_id);
}

RenderBuffer::RenderBuffer(RenderBuffer&& other) noexcept
    : m_id(std::exchange(other.m_id, 0))
{
}

RenderBuffer& RenderBuffer::operator=(RenderBuffer&& other) noexcept
{
    glDeleteRenderbuffers(1, &m_id);
    m_id = std::exchange(other.m_id, 0);
}

void RenderBuffer::allocateStorage(GLenum internalformat, GLsizei width, GLsizei height)
{
    glNamedRenderbufferStorage(m_id, internalformat, width, height);
}
}
