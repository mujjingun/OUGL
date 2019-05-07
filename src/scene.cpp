#include "scene.h"

#include "components.h"
#include "framebuffer.h"
#include "glquery.h"
#include "input.h"
#include "parameters.h"

#include "entitysystems/camerasystem.h"
#include "entitysystems/inputsystem.h"
#include "entitysystems/planetsystem.h"
#include "entitysystems/rendersystem.h"

#include <GL/freeglut.h>
#include <iostream>

#include <glm/gtx/string_cast.hpp>

namespace ou {

void Scene::reshapeWindow(int width, int height)
{
    SceneComponent& scene = m_engine.getOne<SceneComponent>();

    if (width == scene.windowSize.x && height == scene.windowSize.y) {
        return;
    }

    scene.windowSize = { width, height };
    scene.windowResized = true;
}

Scene::Scene()
    : m_lastFrameTime(std::chrono::system_clock::now())
    , m_queries(4)
{
    for (auto& query : m_queries) {
        query = ou::GLQuery(GL_TIME_ELAPSED);
    }

    glm::i64vec3 eye = { 4501787352203439, 5564338967149668, 9224185566471351 };

    // scene entity
    SceneComponent scene;
    scene.position = { { 0, 0, 0 }, eye };
    m_engine.addEntity(Entity({ scene, Input{}, Parameters{} }));

    // planets
    PlanetComponent planet1;
    planet1.position = VoxelCoords{ { 0, 0, 0 }, eye + glm::i64vec3(0, 0, -40371000000000) };
    planet1.radius = 6371000000000;
    planet1.terrainFactor = 0.0001;
    m_engine.addEntity(Entity({ planet1 }));

    PlanetComponent planet2;
    planet2.position = VoxelCoords{ { 0, 0, 0 }, eye + glm::i64vec3(6371000000000 + 5000000000000, 0, -40371000000000) };
    planet2.radius = 4000000000000;
    //m_engine.addEntity(Entity({ planet2 }));

    m_engine.addSystem(std::make_unique<InputSystem>(), 9);
    m_engine.addSystem(std::make_unique<CameraSystem>(), 1);
    m_engine.addSystem(std::make_unique<PlanetSystem>(), 1);
    m_engine.addSystem(std::make_unique<RenderSystem>(m_engine.getOne<Parameters>()), 0);
}

Scene::~Scene() = default;

void Scene::render()
{
    using namespace std::chrono;
    using namespace std::chrono_literals;

    // Update time stuff
    auto now = system_clock::now();
    auto deltaTime = now - m_lastFrameTime;
    m_lastFrameTime = now;

    // query for GPU time
    bool queryPending = false;
    if (m_queries.available()) {
        GLQuery& query = m_queries.push();
        glBeginQuery(GL_TIME_ELAPSED, query.id());
        queryPending = true;
    }

    // update & render
    m_engine.update(duration<float>(deltaTime).count());
    glutSwapBuffers();

    // retrieve GPU time query
    if (queryPending) {
        glEndQuery(GL_TIME_ELAPSED);
    }

    while (m_queries.count()) {
        GLQuery& top = m_queries.top();
        GLint elapsed = -1;
        glGetQueryObjectiv(top.id(), GL_QUERY_RESULT_NO_WAIT, &elapsed);
        if (elapsed != -1) {
            m_queries.pop();
            m_totalGpuTime += nanoseconds(elapsed);
        } else {
            break;
        }
    }

    // calculate CPU time
    m_elapsedTime += deltaTime;

    auto elapsed = system_clock::now() - now;
    m_totalWorkTime += elapsed;
    m_frameCount++;

    // do every second
    if (m_elapsedTime > 1s) {
        m_elapsedTime -= 1s;

        std::cout << "CPU Processing Time: "
                  << m_totalWorkTime.count() / m_frameCount * 1000.f
                  << "ms / 16ms" << std::endl;

        std::cout << "GPU Proceesing Time: "
                  << m_totalGpuTime.count() / m_frameCount * 1000.f
                  << "ms / 16ms" << std::endl;

        m_totalWorkTime = 0s;
        m_totalGpuTime = 0s;
        m_frameCount = 0;
    }
}

Input& Scene::input()
{
    return m_engine.getOne<Input>();
}
}
