#ifndef PLAYER_H
#define PLAYER_H

#include <cstdint>
#include <glm/glm.hpp>

namespace ou {

class Player {
    glm::i64vec3 m_voxel = {0, 0, 0};

    // in millimeters
    glm::i64vec3 m_position = {6371000000000, 0, 0};

    glm::vec3 m_lookDirection = {0, 0, -1};
    glm::vec3 m_upDirection = {0, 1, 0};

public:
    Player();

    glm::vec3 lookDirection() const;
    glm::vec3 upDirection() const;
};
}

#endif // PLAYER_H
