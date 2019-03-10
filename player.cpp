#include "player.h"

#include "scene.h"
#include "planet.h"
#include "parameters.h"

#include <glm/glm.hpp>
#include <algorithm>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>
#include <iostream>

namespace ou {

glm::dvec3 Player::lookDirection() const
{
    return m_lookDirection;
}

glm::dvec3 Player::upDirection() const
{
    return m_upDirection;
}

VoxelCoords Player::position() const
{
    return m_position;
}

void Player::render()
{
    glm::dvec3 right = glm::cross(m_upDirection, m_lookDirection);

    double speed = 30000000000 * 1000.0; // mm/s
    if (Planet const* nearest = nearestPlanet()) {
        VoxelCoords diff = nearest->position() - m_position;
        double distance = glm::length(glm::dvec3(diff.pos)) - nearest->planetRadius();
        speed = distance * 3;
    }

    double moveAmount = m_scene->deltaTime() * speed;

    if (m_scene->isKeyPressed('a')) {
        m_position = m_position + VoxelCoords{ {}, right * moveAmount };
    }
    if (m_scene->isKeyPressed('d')) {
        m_position = m_position - VoxelCoords{ {}, right * moveAmount };
    }
    if (m_scene->isKeyPressed('r')) {
        m_position = m_position + VoxelCoords{ {}, m_upDirection * moveAmount };
    }
    if (m_scene->isKeyPressed('f')) {
        m_position = m_position - VoxelCoords{ {}, m_upDirection * moveAmount };
    }
    if (m_scene->isKeyPressed('w')) {
        m_position = m_position + VoxelCoords{ {}, m_lookDirection * moveAmount };
    }
    if (m_scene->isKeyPressed('s')) {
        m_position = m_position - VoxelCoords{ {}, m_lookDirection * moveAmount };
    }

    glm::dvec2 angle = glm::dvec2(m_scene->mouseDelta()) * glm::radians(m_scene->params().anglePerPixel);
    m_lookDirection = glm::rotate(glm::dmat4(1.0), angle.y, right)
        * glm::rotate(glm::dmat4(1.0), -angle.x, m_upDirection)
        * glm::dvec4(m_lookDirection, 1.0);
    m_upDirection = glm::rotate(glm::dmat4(1.0), angle.y, right) * glm::dvec4(m_upDirection, 1.0);
}

const Planet* Player::nearestPlanet() const
{
    double minDistance = std::numeric_limits<double>::infinity();
    const Planet* nearest = nullptr;

    for (Planet const& p : m_scene->planets()) {
        VoxelCoords diff = p.position() - m_position;
        if (diff.voxel != glm::i64vec3(0, 0, 0)) {
            // skip extrememly far planets
            continue;
        }

        double dist = glm::length(glm::dvec3(diff.pos));
        if (dist < minDistance) {
            minDistance = dist;
            nearest = &p;
        }
    }

    return nearest;
}

Player::Player(const Scene* scene)
    : m_scene(scene)
{
}
}
