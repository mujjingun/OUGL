#include "planetsystem.h"
#include "ecsengine.h"
#include "components.h"

namespace ou {

PlanetSystem::PlanetSystem()
{

}

void PlanetSystem::update(ECSEngine &engine, float deltaTime)
{
    for (auto& planet : engine.iterate<PlanetComponent>()) {
        planet.get<PlanetComponent>().angle += static_cast<double>(deltaTime) * glm::radians(0.0);
    }
}
}
