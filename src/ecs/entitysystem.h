#ifndef SYSTEM_H
#define SYSTEM_H

namespace ou {

class ECSEngine;

class EntitySystem {
public:
    EntitySystem() = default;
    virtual ~EntitySystem() = default;
    virtual void update(ECSEngine& engine, float deltaTime) = 0;
};
}

#endif // SYSTEM_H
