#include "player.h"

namespace ou {

glm::vec3 Player::lookDirection() const
{
    return m_lookDirection;
}

glm::vec3 Player::upDirection() const
{
    return m_upDirection;
}

Player::Player()
{
}
}
