#ifndef COMPONENTS_H
#define COMPONENTS_H

#include "texture.h"
#include "devicebuffer.h"
#include "voxelcoords.h"
#include "circularbuffer.h"
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

struct PBOSync {
    DeviceBuffer buf;
    int texIdx;
    GLsync sync;
};

struct PlanetComponent {
    std::int64_t radius;
    VoxelCoords position;
    double terrainFactor = 0.0012;
    std::shared_ptr<Texture> lod0Textures{};
    std::shared_ptr<Texture> terrainTextures{};
    std::shared_ptr<Texture> heightBases{};
    std::vector<glm::i64vec2> snapNums{};
    std::shared_ptr<CircularBuffer<PBOSync>> pbos{};
    std::int64_t playerTerrainHeight = 0.0f;
    std::int64_t baseHeight = 0.0f;
    glm::vec2 storedBase{};
    int baseTexIdx = 0;
};
}

#endif // COMPONENTS_H
