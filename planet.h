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
    Scene const *m_scene;

    std::int64_t m_planetRadius;

    VoxelCoords m_position;

    VertexArray m_vao;

    VertexBuffer m_buf, m_instanceAttrBuf;

    Shader& shader();

    std::size_t m_vertexCount;

public:
    Planet(Scene const* scene);
    Planet(Scene const* scene, std::int64_t planetRadius, VoxelCoords position);
    void render();
    VoxelCoords position() const;
    std::int64_t planetRadius() const;
};

}

#endif // PLANET_H
