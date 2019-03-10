#ifndef PLAYER_H
#define PLAYER_H

#include <cstdint>
#include <glm/glm.hpp>

#include "voxelcoords.h"

namespace ou {

class Scene;
class Planet;

class Player {
    Scene const* m_scene;

    VoxelCoords m_position = {
        { 0, 0, 0 },
        { 4501787352203439, 5564338967149668, 9183814566471351 + 40371000000000 }
    };

    glm::dvec3 m_lookDirection = { 0, 0, -1 };
    glm::dvec3 m_upDirection = { 0, 1, 0 };

    Planet const* nearestPlanet() const;

public:
    Player(Scene const* scene);

    glm::dvec3 lookDirection() const;
    glm::dvec3 upDirection() const;
    VoxelCoords position() const;

    void render();
};
}

#endif // PLAYER_H
