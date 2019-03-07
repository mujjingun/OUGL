#ifndef VERTEXBUFFER_H
#define VERTEXBUFFER_H

#include <GL/glew.h>

#include "rawbufferview.h"

namespace ou {

class VertexBuffer
{
    GLuint m_id;

public:
    VertexBuffer();
    ~VertexBuffer();

    void setData(RawBufferView data, GLenum usage);

    GLuint id() const;
};

}

#endif // VERTEXBUFFER_H
