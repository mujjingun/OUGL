#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include <GL/glew.h>

class RawBufferView;

namespace ou {

class Texture;
class RenderBuffer;

class FrameBuffer {
    GLuint m_id;

    static FrameBuffer sm_defaultBuffer;
    FrameBuffer(GLuint id);

public:
    FrameBuffer();
    ~FrameBuffer();

    FrameBuffer(FrameBuffer const&) = delete;
    FrameBuffer& operator=(FrameBuffer const&) = delete;

    FrameBuffer(FrameBuffer&& other) noexcept;
    FrameBuffer& operator=(FrameBuffer&& other) noexcept;

    static FrameBuffer& defaultBuffer();

    void clear(GLenum buffer, GLint drawbuffer, const GLfloat* value);

    void use(GLenum target) const;

    void bindTexture(GLenum target, Texture const& texture, GLint mipLevel = 0);
    void bindRenderBuffer(GLenum target, RenderBuffer const& renderbuffer);

    bool isComplete() const;

    GLuint id() const;
};
}
#endif // FRAMEBUFFER_H
