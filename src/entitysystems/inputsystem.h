#ifndef INPUTSYSTEM_H
#define INPUTSYSTEM_H

#include "entitysystem.h"

namespace ou {

class InputSystem : public EntitySystem {

public:
    InputSystem();

    // EntitySystem interface
public:
    void update(ECSEngine &engine, float deltaTime) override;
};
}

#endif // INPUTSYSTEM_H
