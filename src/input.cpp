#include "input.h"

// clang-format off
#include <GL/glew.h>
#include <GL/freeglut.h>
// clang-format on

#include <iostream>

namespace ou {

void Input::keyDown(unsigned char key)
{
    m_keyStates[key] = true;
}

void Input::keyUp(unsigned char key)
{
    m_keyStates[key] = false;
}

void Input::mouseClick(int button, int event)
{
    if (button == GLUT_LEFT_BUTTON) {
        m_leftDown = (event == GLUT_DOWN);
    } else if (button == GLUT_RIGHT_BUTTON) {
        m_rightDown = (event == GLUT_DOWN);
    }

    if (m_doCaptureMouse) {
        if (button == GLUT_LEFT_BUTTON && event == GLUT_DOWN) {
            if (m_mouseCaptured) {
                glutSetCursor(GLUT_CURSOR_INHERIT);
                m_mouseCaptured = false;
            } else {
                glutSetCursor(GLUT_CURSOR_NONE);
                m_mouseCaptured = true;
            }
        }
    }
}

void Input::mouseMove(int x, int y)
{
    m_realMousePos = { x, y };
    if (m_mouseInvalidated) {
        m_mouseInvalidated = false;
        m_lastRealMousePos = m_realMousePos;
    }
}

void Input::mouseEnter()
{
    m_mouseInvalidated = true;
}

void Input::mouseLeft()
{
}

Input::Input()
{
}

bool Input::isKeyPressed(unsigned char key) const
{
    auto it = m_keyStates.find(key);
    if (it != m_keyStates.end()) {
        return it->second;
    }
    return false;
}

glm::dvec2 Input::mouseDelta() const
{
    return m_smoothedMouseDelta;
}
}
