#ifndef PLANETSYSTEM_H
#define PLANETSYSTEM_H

#include "entitysystem.h"

namespace ou {

class PlanetSystem : public EntitySystem
{
public:
    PlanetSystem();

    void update(ECSEngine &engine, float deltaTime) override;
};
}

#endif // PLANETSYSTEM_H
