#include "framebuffer.h"

#include "rawbufferview.h"

namespace ou {

GLuint FrameBuffer::id() const
{
    return m_id;
}

FrameBuffer FrameBuffer::sm_defaultBuffer = FrameBuffer(0);

FrameBuffer::FrameBuffer(GLuint id)
    : m_id(id)
{
}

FrameBuffer::FrameBuffer()
{
    glCreateFramebuffers(1, &m_id);
}

FrameBuffer::~FrameBuffer()
{
    glDeleteFramebuffers(1, &m_id);
}

FrameBuffer& FrameBuffer::defaultBuffer()
{
    return sm_defaultBuffer;
}

void FrameBuffer::clear(GLenum buffer, GLint drawbuffer, const GLfloat* value)
{
    glClearNamedFramebufferfv(m_id, buffer, drawbuffer, const_cast<GLfloat*>(value));
}

void FrameBuffer::use(GLenum target) const
{
    glBindFramebuffer(target, m_id);
}
}
