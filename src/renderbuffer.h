#ifndef RENDERBUFFER_H
#define RENDERBUFFER_H

#include <GL/glew.h>

namespace ou {

class RenderBuffer {
    GLuint m_id;

public:
    RenderBuffer();
    ~RenderBuffer();

    RenderBuffer(RenderBuffer const&) = delete;
    RenderBuffer& operator=(RenderBuffer const&) = delete;

    RenderBuffer(RenderBuffer&& other) noexcept;
    RenderBuffer& operator=(RenderBuffer&& other) noexcept;

    void allocateStorage(GLenum internalformat, GLsizei width, GLsizei height);

    GLuint id() const;
};
}

#endif // RENDERBUFFER_H
