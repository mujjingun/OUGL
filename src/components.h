#ifndef COMPONENTS_H
#define COMPONENTS_H

#include "circularbuffer.h"
#include "devicebuffer.h"
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

struct PlanetRenderStates;

struct PlanetComponent {
    std::int64_t radius;
    VoxelCoords position;
    double terrainFactor = 0.0012;
    double angle = 0.0;
    std::int64_t playerTerrainHeight = 0;

    std::shared_ptr<PlanetRenderStates> r{};
};
}

#endif // COMPONENTS_H
