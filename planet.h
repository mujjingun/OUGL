#ifndef PLANET_H
#define PLANET_H

#include <GL/glew.h>

#include <glm/glm.hpp>

#include "vertexarray.h"
#include "vertexbuffer.h"
#include "shader.h"
#include "voxelcoords.h"
#include "texture.h"

namespace ou {

class Scene;

class Planet
{
    Scene const *m_scene;

    std::int64_t m_planetRadius;

    VoxelCoords m_position;

    VertexArray m_vao;

    VertexBuffer m_gridBuf, m_instanceAttrBuf;

    Texture m_terrainTextures;

    Shader& shader();
    Shader& terrainShader();

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
