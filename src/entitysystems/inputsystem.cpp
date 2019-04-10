#include "inputsystem.h"
#include "components.h"
#include "ecsengine.h"
#include "input.h"
#include "parameters.h"

// clang-format off
#include <GL/glew.h>
#include <GL/freeglut.h>
// clang-format on

#include <iostream>
#include <glm/gtx/string_cast.hpp>

namespace ou {

InputSystem::InputSystem()
{
}

void InputSystem::update(ECSEngine& engine, float deltaTime)
{
    // Update mouse cursor position
    Parameters params = engine.getOne<Parameters>();
    SceneComponent scene = engine.getOne<SceneComponent>();
    Input& input = engine.getOne<Input>();

    input.m_destLogicalMousePos += input.m_realMousePos - input.m_lastRealMousePos;
    input.m_lastRealMousePos = input.m_realMousePos;

    // apply smoothing
    double smoothing = 1 - glm::exp(-double(deltaTime) * params.smoothingFactor);
    input.m_smoothedMouseDelta = (input.m_destLogicalMousePos - input.m_logicalMousePos) * smoothing;
    input.m_logicalMousePos += input.m_smoothedMouseDelta;

    // capture mouse
    if (input.m_mouseCaptured) {
        if (input.m_realMousePos.x > scene.windowSize.x * .6
            || input.m_realMousePos.x < scene.windowSize.x * .4
            || input.m_realMousePos.y > scene.windowSize.y * .6
            || input.m_realMousePos.y < scene.windowSize.y * .4) {
            glutWarpPointer(scene.windowSize.x / 2, scene.windowSize.y / 2);
            input.m_mouseInvalidated = true;
        }
    }
}
}
