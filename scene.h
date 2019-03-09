#ifndef SCENE_H
#define SCENE_H

#include <memory>
#include <unordered_map>
#include <chrono>
#include <glm/glm.hpp>

namespace ou {

class Scene {
    std::unique_ptr<class Planet> m_planet;
    std::unique_ptr<class Player> m_player;

    std::unordered_map<unsigned char, bool> m_keyStates;

    int m_windowWidth, m_windowHeight;

    std::chrono::system_clock::time_point m_lastFrameTime;
    std::chrono::system_clock::duration m_deltaTime;

    glm::ivec2 m_mousePos = { INT_MAX, INT_MAX };
    glm::ivec2 m_lastMousePos = { INT_MAX, INT_MAX };
    glm::ivec2 m_realMousePos;

    bool m_captureMouse = false;
    bool m_warpPointer = false;

    void mouseMove(int x, int y);
    void mouseLeave();

    void keyDown(unsigned char key);
    void keyUp(unsigned char key);

public:
    Scene();
    ~Scene();
    void render();

    int windowWidth() const;
    int windowHeight() const;
    bool isKeyPressed(unsigned char key) const;

    double deltaTime() const;

    const Player& player() const;

    glm::ivec2 mouseDelta() const;

    friend class Callbacks;
};
}

#endif // SCENE_H
