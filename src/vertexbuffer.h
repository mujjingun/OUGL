#ifndef VERTEXBUFFER_H
#define VERTEXBUFFER_H

#include <GL/glew.h>

#include "rawbufferview.h"

namespace ou {

class VertexBuffer {
    GLuint m_id;

public:
    VertexBuffer();
    ~VertexBuffer();

    VertexBuffer(VertexBuffer const&) = delete;
    VertexBuffer& operator=(VertexBuffer const&) = delete;

    VertexBuffer(VertexBuffer&& other) noexcept;
    VertexBuffer& operator=(VertexBuffer&& other) noexcept;

    void setData(RawBufferView data, GLenum usage);
    void use(GLenum target, GLuint index, GLintptr offset, GLsizeiptr size) const;
    void use(GLenum target, GLuint index) const;

    GLuint id() const;
};
}

#endif // VERTEXBUFFER_H
