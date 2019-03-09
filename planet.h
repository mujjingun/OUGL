#ifndef PLANET_H
#define PLANET_H

#include <GL/glew.h>

#include <glm/glm.hpp>

#include "vertexarray.h"
#include "vertexbuffer.h"
#include "shader.h"
#include "voxelcoords.h"

namespace ou {

class Scene;

class Planet
{
    std::int64_t m_planetRadius = 6371000000000;

    VoxelCoords m_position = {
        {0, 0, 0},
        {4501787352203439, 5564338967149668, 9183814566471351}
    };

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
