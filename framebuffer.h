#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include <GL/glew.h>

class RawBufferView;

namespace ou {

class FrameBuffer {
    GLuint m_id;

    static FrameBuffer sm_defaultBuffer;
    FrameBuffer(GLuint id);

public:
    FrameBuffer();
    ~FrameBuffer();

    FrameBuffer(FrameBuffer const&) = delete;
    FrameBuffer& operator=(FrameBuffer const&) = delete;

    FrameBuffer(FrameBuffer&& other);
    FrameBuffer& operator=(FrameBuffer&& other);

    static FrameBuffer& defaultBuffer();

    void clear(GLenum buffer, GLint drawbuffer, const GLfloat* value);

    void use(GLenum target) const;

    GLuint id() const;
};
}
#endif // FRAMEBUFFER_H
