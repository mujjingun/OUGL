#ifndef INPUT_H
#define INPUT_H

#include <glm/glm.hpp>
#include <unordered_map>

namespace ou {

class Input {
    friend class Callbacks;
    friend class InputSystem;

    // keyboard
    std::unordered_map<unsigned char, bool> m_keyStates;

    void keyDown(unsigned char key);
    void keyUp(unsigned char key);

    // mouse
    glm::ivec2 m_lastRealMousePos{};
    glm::ivec2 m_realMousePos{};
    glm::dvec2 m_logicalMousePos{};
    glm::dvec2 m_destLogicalMousePos{};
    glm::dvec2 m_smoothedMouseDelta{};

    bool m_mouseInvalidated = false;
    bool m_leftDown = false;
    bool m_rightDown = false;
    bool m_doCaptureMouse = true;
    bool m_mouseCaptured = false;

    void mouseClick(int button, int event);
    void mouseMove(int x, int y);
    void mouseEnter();
    void mouseLeft();

public:
    Input();

    bool isKeyPressed(unsigned char key) const;

    glm::dvec2 mouseDelta() const;
};
}

#endif // INPUT_H
