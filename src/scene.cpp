#include "scene.h"

#include "components.h"
#include "framebuffer.h"
#include "input.h"
#include "parameters.h"

#include "entitysystems/camerasystem.h"
#include "entitysystems/inputsystem.h"
#include "entitysystems/rendersystem.h"

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
{
    // scene entity
    SceneComponent scene;
    scene.position = { { 0, 0, 0 }, { 4501787352203439, 5564338967149668, 9224185566471351 } };
    m_engine.addEntity(Entity({ scene, Input{}, Parameters{} }));

    // planets
    PlanetComponent planet1;
    planet1.position = VoxelCoords{ { 0, 0, 0 }, { 4501787352203439, 5564338967149668, 9183814566471351 } };
    planet1.planetRadius = 6371000000000;
    m_engine.addEntity(Entity({ planet1 }));

    PlanetComponent planet2;
    planet2.position = VoxelCoords{ { 0, 0, 0 }, { 4522158352203439, 5564338967149668, 9204185566471351 } };
    planet2.planetRadius = 4000000000000;
    m_engine.addEntity(Entity({ planet2 }));

    m_engine.addSystem(std::make_unique<InputSystem>(), 9);
    m_engine.addSystem(std::make_unique<CameraSystem>(), 1);
    m_engine.addSystem(std::make_unique<RenderSystem>(m_engine.getOne<Parameters>()), 0);
}

Scene::~Scene() = default;

void Scene::render()
{
    // Update time stuff
    auto now = std::chrono::system_clock::now();
    auto deltaTime = now - m_lastFrameTime;
    m_lastFrameTime = now;

    // update & render
    m_engine.update(std::chrono::duration<float>(deltaTime).count());
    auto elapsed = std::chrono::system_clock::now() - now;
    std::cout << std::chrono::duration<float>(elapsed).count() * 1000.f << "ms\n";
}

Input& Scene::input()
{
    return m_engine.getOne<Input>();
}
}
