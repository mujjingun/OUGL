#ifndef SCENE_H
#define SCENE_H

#include <memory>
#include <unordered_map>
#include <chrono>
#include <vector>
#include <glm/glm.hpp>

#include "framebuffer.h"
#include "texture.h"
#include "renderbuffer.h"
#include "shader.h"
#include "vertexarray.h"

namespace ou {

class Planet;
class Player;
struct Parameters;

class Scene {
    std::unique_ptr<Parameters> m_parameters;

    std::vector<Planet> m_planets;
    std::unique_ptr<Player> m_player;

    FrameBuffer m_hdrFrameBuffer;
    Texture m_hdrColorTexture;
    RenderBuffer m_hdrDepthRenderBuffer;
    Shader m_hdrShader;
    VertexArray m_hdrVao;

    std::chrono::system_clock::time_point m_lastFrameTime;
    std::chrono::system_clock::duration m_deltaTime;

    glm::dvec2 m_mousePos;
    glm::dvec2 m_realMousePos;

    glm::dvec2 m_lastMousePos;
    glm::dvec2 m_smoothedMouseDelta;
    bool m_mousePosInvalidated = true;
    bool m_captureMouse = false;
    bool m_warpPointer = false;

    void mouseClick();
    void mouseMove(int x, int y);
    void mouseEnter();

    std::unordered_map<unsigned char, bool> m_keyStates;

    void keyDown(unsigned char key);
    void keyUp(unsigned char key);

    int m_windowWidth = -1, m_windowHeight = -1;

    void reshapeWindow(int width, int height);

public:
    Scene();
    ~Scene();
    void render();

    int windowWidth() const;
    int windowHeight() const;
    bool isKeyPressed(unsigned char key) const;

    double deltaTime() const;

    std::vector<Planet> const& planets() const;
    const Player& player() const;
    const Parameters& params() const;

    glm::dvec2 mouseDelta() const;

    friend class Callbacks;
};
}

#endif // SCENE_H
