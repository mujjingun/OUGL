#include "scene.h"

#include "planet.h"
#include "player.h"
#include "framebuffer.h"

#include <GL/glew.h>

#include <GL/freeglut.h>

namespace ou {

int Scene::windowWidth() const
{
    return m_windowWidth;
}

int Scene::windowHeight() const
{
    return m_windowHeight;
}

const Player &Scene::player() const
{
    return *m_player;
}

Scene::Scene()
    : m_planet(new Planet)
    , m_player(new Player)
{
}

Scene::~Scene() = default;

void Scene::render()
{
    m_windowWidth = glutGet(GLUT_WINDOW_WIDTH);
    m_windowHeight = glutGet(GLUT_WINDOW_HEIGHT);

    float clearColor[] = {0, 0, 0, 0};
    FrameBuffer::defaultBuffer().clear(GL_COLOR, 0, clearColor);
    float clearDepth[] = {0};
    FrameBuffer::defaultBuffer().clear(GL_DEPTH, 0, clearDepth);

    m_planet->render(*this);
}

}
