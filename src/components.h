#ifndef COMPONENTS_H
#define COMPONENTS_H

#include "texture.h"
#include "devicebuffer.h"
#include "voxelcoords.h"
#include <glm/glm.hpp>
#include <memory>
#include <vector>

namespace ou {

struct SceneComponent {
    glm::ivec2 windowSize{};
    bool windowResized = false;

    VoxelCoords position;
    glm::dvec3 lookDirection = { 0, 0, -1 };
    glm::dvec3 upDirection = { 0, 1, 0 };
};

struct PlanetComponent {
    std::int64_t planetRadius;
    VoxelCoords position;
    float terrainFactor = 0.001f;
    std::shared_ptr<Texture> terrainTextures{};
    std::vector<glm::i64vec2> snapNums{};
    std::shared_ptr<DeviceBuffer> pbo{};
    GLsync sync = nullptr;
    std::int64_t playerTerrainHeight = 0.0f;
};
}

#endif // COMPONENTS_H
