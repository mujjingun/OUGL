#include "player.h"

#include "scene.h"

#include <glm/glm.hpp>

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

void Player::render(Scene const& scene)
{
    glm::dvec3 right = glm::cross(m_upDirection, m_lookDirection);
    double speed = 30000000000 * 1000.0;
    double moveAmount = scene.deltaTime() * speed;

    if (scene.isKeyPressed('a')) {
        m_position = m_position + VoxelCoords{ {}, right * moveAmount };
    }
    if (scene.isKeyPressed('d')) {
        m_position = m_position - VoxelCoords{ {}, right * moveAmount };
    }
    if (scene.isKeyPressed('r')) {
        m_position = m_position + VoxelCoords{ {}, m_upDirection * moveAmount };
    }
    if (scene.isKeyPressed('f')) {
        m_position = m_position - VoxelCoords{ {}, m_upDirection * moveAmount };
    }
    if (scene.isKeyPressed('w')) {
        m_position = m_position + VoxelCoords{ {}, m_lookDirection * moveAmount };
    }
    if (scene.isKeyPressed('s')) {
        m_position = m_position - VoxelCoords{ {}, m_lookDirection * moveAmount };
    }

    glm::dvec2 angle = glm::dvec2(scene.mouseDelta()) * glm::radians(1.0);
    m_lookDirection = glm::rotate(glm::dmat4(1.0), angle.y, right)
        * glm::rotate(glm::dmat4(1.0), -angle.x, m_upDirection)
        * glm::dvec4(m_lookDirection, 1.0);
    m_upDirection = glm::rotate(glm::dmat4(1.0), angle.y, right) * glm::dvec4(m_upDirection, 1.0);
}

Player::Player()
{
}
}
