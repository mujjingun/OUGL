#include "framebuffer.h"

#include "rawbufferview.h"
#include "texture.h"
#include "renderbuffer.h"

#include <algorithm>

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

FrameBuffer::FrameBuffer(FrameBuffer &&other) noexcept
    : m_id(std::exchange(other.m_id, 0))
{
}

FrameBuffer &FrameBuffer::operator=(FrameBuffer &&other) noexcept
{
    glDeleteFramebuffers(1, &m_id);
    m_id = std::exchange(other.m_id, 0);
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

void FrameBuffer::bindTexture(GLenum target, const Texture &texture, GLint mipLevel)
{
    glNamedFramebufferTexture(m_id, target, texture.id(), mipLevel);
}

void FrameBuffer::bindRenderBuffer(GLenum target, const RenderBuffer &renderbuffer)
{
    glNamedFramebufferRenderbuffer(m_id, target, GL_RENDERBUFFER, renderbuffer.id());
}

bool FrameBuffer::isComplete() const
{
    return glCheckNamedFramebufferStatus(m_id, GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE;
}
}
