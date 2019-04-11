#ifndef VERTEXBUFFER_H
#define VERTEXBUFFER_H

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "rawbufferview.h"

namespace ou {

class Texture;

class DeviceBuffer {
    GLuint m_id;

public:
    DeviceBuffer();
    ~DeviceBuffer();

    DeviceBuffer(DeviceBuffer const&) = delete;
    DeviceBuffer& operator=(DeviceBuffer const&) = delete;

    DeviceBuffer(DeviceBuffer&& other) noexcept;
    DeviceBuffer& operator=(DeviceBuffer&& other) noexcept;

    void allocateStorage(GLsizeiptr size, GLenum usage);
    void setData(RawBufferView data, GLenum usage);
    void use(GLenum target, GLuint index, GLintptr offset, GLsizeiptr size);
    void use(GLenum target, GLuint index);
    void use(GLenum target);

    void* map(GLenum access);
    void unmap();

    void copyTexture(Texture& tex, GLint level, glm::ivec3 offset,
        glm::uvec3 size, GLenum format, GLenum type, GLsizei bufSize, GLsizeiptr bufOffset);

    GLuint id() const;
};
}

#endif // VERTEXBUFFER_H
