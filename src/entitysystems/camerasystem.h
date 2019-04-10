#ifndef CAMERASYSTEM_H
#define CAMERASYSTEM_H

#include "entitysystem.h"

namespace ou {

class CameraSystem : public EntitySystem {
public:
    CameraSystem();

    // EntitySystem interface
public:
    void update(ECSEngine &engine, float deltaTime) override;
};
}

#endif // CAMERASYSTEM_H
