#ifndef COMPONENTS_H
#define COMPONENTS_H

#include "texture.h"
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
    float terrainFactor = 0.0005f;
    std::shared_ptr<Texture> terrainTextures{};
    std::vector<glm::i64vec2> snapNums;
};
}

#endif // COMPONENTS_H
