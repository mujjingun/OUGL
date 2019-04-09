#ifndef VERTEXBUFFER_H
#define VERTEXBUFFER_H

#include <GL/glew.h>

#include "rawbufferview.h"

namespace ou {

class DeviceBuffer {
    GLuint m_id;

public:
    DeviceBuffer();
    ~DeviceBuffer();

    DeviceBuffer(DeviceBuffer const&) = delete;
    DeviceBuffer& operator=(DeviceBuffer const&) = delete;

    DeviceBuffer(DeviceBuffer&& other) noexcept;
    DeviceBuffer& operator=(DeviceBuffer&& other) noexcept;

    void setData(RawBufferView data, GLenum usage);
    void use(GLenum target, GLuint index, GLintptr offset, GLsizeiptr size);
    void use(GLenum target, GLuint index);
    void use(GLenum target);

    GLuint id() const;
};
}

#endif // VERTEXBUFFER_H
