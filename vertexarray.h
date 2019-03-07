#ifndef VERTEXARRAY_H
#define VERTEXARRAY_H

#include <GL/glew.h>

namespace ou {

class VertexBuffer;

class VertexArray
{
    GLuint m_id;

public:
    VertexArray();
    ~VertexArray();

    class Attribute {
        VertexArray *m_array;
        GLuint m_index;

        Attribute(VertexArray *m_array, GLuint m_index);

    public:
        void setFormat(GLuint size, GLenum type, GLboolean normalized, GLuint relativeoffset);
        void bindVertexBuffer(const VertexBuffer &buf, GLintptr offset, GLsizei stride);
        void setBindingDivisor(GLuint divisor);

        friend VertexArray;
    };

    Attribute enableVertexAttrib(GLuint attribindex);

    void use() const;

    GLuint id() const;
};

}

#endif // VERTEXARRAY_H
