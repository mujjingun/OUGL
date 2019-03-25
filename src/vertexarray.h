#ifndef VERTEXARRAY_H
#define VERTEXARRAY_H

#include <GL/glew.h>

namespace ou {

class VertexBuffer;

class VertexArray {
    GLuint m_id;

public:
    VertexArray();
    ~VertexArray();

    VertexArray(VertexArray const&) = delete;
    VertexArray& operator=(VertexArray const&) = delete;

    VertexArray(VertexArray&& other) noexcept;
    VertexArray& operator=(VertexArray&& other) noexcept;

    class BufferBinding {
        VertexArray* m_array;
        GLuint m_index;

        BufferBinding(VertexArray* array, GLuint index);

    public:
        void bindVertexBuffer(const VertexBuffer& buf, GLintptr offset, GLsizei stride);
        void setBindingDivisor(GLuint divisor);
        GLuint index() const;

        friend VertexArray;
    };

    class Attribute {
        VertexArray* m_array;
        GLuint m_index;

        Attribute(VertexArray* array, GLuint index);

    public:
        void setFormat(GLuint size, GLenum type, GLboolean normalized, GLuint relativeoffset);
        void setBinding(BufferBinding const& binding);
        GLuint index() const;

        friend VertexArray;
    };

    Attribute enableVertexAttrib(GLuint attribindex);
    BufferBinding getBinding(GLuint bindingindex);

    void use() const;

    GLuint id() const;
};
}

#endif // VERTEXARRAY_H
