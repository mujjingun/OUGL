#include "camerasystem.h"
#include "components.h"
#include "ecsengine.h"
#include "input.h"
#include "parameters.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

namespace ou {

CameraSystem::CameraSystem()
{
}

void CameraSystem::update(ECSEngine& engine, float deltaTime)
{
    SceneComponent& scene = engine.getOne<SceneComponent>();
    Parameters const& params = engine.getOne<Parameters>();

    glm::dvec3 right = glm::cross(scene.upDirection, scene.lookDirection);

    // move camera
    double speed = 30000000000 * 1000.0; // mm/s

    const PlanetComponent* nearest = [&] {
        double minDistance = std::numeric_limits<double>::infinity();
        const PlanetComponent* nearest = nullptr;

        for (Entity& ent : engine.iterate<PlanetComponent>()) {
            PlanetComponent& planet = ent.get<PlanetComponent>();

            VoxelCoords diff = planet.position - scene.position;
            if (diff.voxel != glm::i64vec3(0, 0, 0)) {
                // skip extrememly far planets
                continue;
            }

            double dist = glm::length(glm::dvec3(diff.pos));
            if (dist < minDistance) {
                minDistance = dist;
                nearest = &planet;
            }
        }

        return nearest;
    }();

    std::int64_t altitude = 0;

    if (nearest) {
        glm::i64vec3 diff = (scene.position - nearest->position).pos;
        auto distanceFromCenter = std::int64_t(glm::length(glm::dvec3(diff)));
        altitude = distanceFromCenter - nearest->planetRadius - nearest->playerTerrainHeight;
        speed = altitude * 3;
    }

    double moveAmount = static_cast<double>(deltaTime) * speed;

    Input const& input = engine.getOne<Input>();
    if (input.isKeyPressed('a')) {
        scene.position += VoxelCoords{ {}, right * moveAmount };
    }
    if (input.isKeyPressed('d')) {
        scene.position -= VoxelCoords{ {}, right * moveAmount };
    }
    if (input.isKeyPressed('r')) {
        scene.position += VoxelCoords{ {}, scene.upDirection * moveAmount };
    }
    if (input.isKeyPressed('f')) {
        scene.position -= VoxelCoords{ {}, scene.upDirection * moveAmount };
    }
    if (input.isKeyPressed('w')) {
        scene.position += VoxelCoords{ {}, scene.lookDirection * moveAmount };
    }
    if (input.isKeyPressed('s')) {
        scene.position -= VoxelCoords{ {}, scene.lookDirection * moveAmount };
    }

    // stop at player height
    if (nearest) {
        glm::i64vec3 diff = (scene.position - nearest->position).pos;
        if (altitude < params.playerHeight) {
            double eyeHeight = altitude - params.playerHeight;
            glm::i64vec3 delta = glm::normalize(glm::dvec3(diff)) * eyeHeight;
            scene.position -= VoxelCoords{ {}, delta };
        }
    }

    // rotate screen
    glm::dvec2 angle = glm::dvec2(input.mouseDelta()) * glm::radians(params.anglePerPixel);
    scene.lookDirection = glm::rotate(glm::dmat4(1.0), angle.y, right)
        * glm::rotate(glm::dmat4(1.0), -angle.x, scene.upDirection)
        * glm::dvec4(scene.lookDirection, 1.0);
    scene.upDirection = glm::rotate(glm::dmat4(1.0), angle.y, right) * glm::dvec4(scene.upDirection, 1.0);
}
}
