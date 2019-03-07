#ifndef PLANET_H
#define PLANET_H

#include <GL/glew.h>

#include "vertexarray.h"
#include "vertexbuffer.h"
#include "shader.h"

namespace ou {

class Scene;

class Planet
{
    VertexArray m_vao;

    VertexBuffer m_buf, m_instanceAttrBuf;

    Shader m_shader;

    std::size_t m_vertexCount;

public:
    Planet();
    ~Planet();
    void render(Scene const& scene);
};

}

#endif // PLANET_H
