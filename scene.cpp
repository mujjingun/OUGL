#include "scene.h"

#include "framebuffer.h"
#include "planet.h"
#include "player.h"

#include <GL/glew.h>

#include <GL/freeglut.h>

#include <iostream>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>

namespace ou {

int Scene::windowWidth() const
{
    return m_windowWidth;
}

int Scene::windowHeight() const
{
    return m_windowHeight;
}

bool Scene::isKeyPressed(unsigned char key) const
{
    auto it = m_keyStates.find(key);
    if (it != m_keyStates.end()) {
        return it->second;
    }
    return false;
}

double Scene::deltaTime() const
{
    return std::chrono::duration<double>(m_deltaTime).count();
}

const Player& Scene::player() const
{
    return *m_player;
}

glm::ivec2 Scene::mouseDelta() const
{
    if (m_lastMousePos.x == INT_MAX || m_mousePos.x == INT_MAX) {
        return { 0, 0 };
    }
    return m_mousePos - m_lastMousePos;
}

void Scene::mouseMove(int x, int y)
{
    if (m_captureMouse) {
        glm::ivec2 delta(x - m_realMousePos.x, y - m_realMousePos.y);
        glm::ivec2 center(x - m_windowWidth / 2, y - m_windowHeight / 2);
        if (std::abs(center.x) > m_windowWidth / 3 || std::abs(center.y) > m_windowHeight / 3) {
            m_warpPointer = true;
        }
        m_mousePos = m_lastMousePos + delta;
    } else {
        m_mousePos = { x, y };
    }

    m_realMousePos = { x, y };
}

void Scene::mouseLeave()
{
    m_mousePos = { INT_MAX, INT_MAX };
}

void Scene::keyDown(unsigned char key)
{
    m_keyStates[key] = true;
}

void Scene::keyUp(unsigned char key)
{
    m_keyStates[key] = false;

    if (key == 'c') {
        if (!m_captureMouse) {
            m_captureMouse = true;
            glutSetCursor(GLUT_CURSOR_NONE);
            m_lastMousePos = { 0, 0 };
            m_mousePos = { 0, 0 };
            m_warpPointer = true;
        } else {
            m_captureMouse = false;
            glutSetCursor(GLUT_CURSOR_INHERIT);
            m_mousePos = { INT_MAX, INT_MAX };
        }
    }
}

Scene::Scene()
    : m_planet(new Planet)
    , m_player(new Player)
{
}

Scene::~Scene() = default;

void Scene::render()
{
    auto now = std::chrono::system_clock::now();
    m_deltaTime = now - m_lastFrameTime;
    m_lastFrameTime = now;

    m_windowWidth = glutGet(GLUT_WINDOW_WIDTH);
    m_windowHeight = glutGet(GLUT_WINDOW_HEIGHT);

    float clearColor[] = { 0, 0, 0, 0 };
    FrameBuffer::defaultBuffer().clear(GL_COLOR, 0, clearColor);
    float clearDepth[] = { 0 };
    FrameBuffer::defaultBuffer().clear(GL_DEPTH, 0, clearDepth);

    m_player->render(*this);
    m_planet->render(*this);

    m_lastMousePos = m_mousePos;

    if (m_captureMouse && m_warpPointer) {
        glutWarpPointer(m_windowWidth / 2, m_windowHeight / 2);
        m_realMousePos = { m_windowWidth / 2, m_windowHeight / 2 };
        m_warpPointer = false;
    }
}
}
