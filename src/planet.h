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

    float m_terrainFactor;

    VertexArray m_vao;

    VertexBuffer m_gridBuf, m_instanceAttrBuf;

    Texture m_terrainTextures;
    VertexBuffer m_lodUboBuf, m_lodUpdUboBuf;

    Shader& shader();
    Shader& terrainShader();

    std::size_t m_vertexCount;

    std::vector<glm::i64vec2> m_snapNums;

    struct LodData {
        glm::vec2 center;
        glm::vec2 origin;
        float scale;
        int imgIdx;
        int parentIdx;
        int unused[1]{};
    };
    static_assert(sizeof(LodData) % 16 == 0, "ubo struct is not aligned to 16 bytes");
    std::vector<LodData> m_lodData;

public:
    Planet(Scene const* scene);
    Planet(Scene const* scene, std::int64_t planetRadius, VoxelCoords position);
    void render();
    VoxelCoords position() const;
    std::int64_t planetRadius() const;
    std::int64_t distanceFromGround(glm::i64vec3 centerdCoords) const;
};

}

#endif // PLANET_H
