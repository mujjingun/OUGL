#ifndef SCENE_H
#define SCENE_H

#include <chrono>
#include <glm/glm.hpp>
#include <memory>
#include <unordered_map>
#include <vector>

#include "ecsengine.h"
#include "input.h"

namespace ou {

class Scene {
    ECSEngine m_engine{};

    std::chrono::system_clock::time_point m_lastFrameTime;

    std::chrono::duration<float> m_elapsedTime{};
    std::chrono::duration<float> m_totalWorkTime{};
    int m_frameCount{};

public:
    Scene();
    ~Scene();
    void render();

    void reshapeWindow(int width, int height);
    Input& input();
};
}

#endif // SCENE_H
