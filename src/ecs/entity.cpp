#include "entity.h"
#include "ecsengine.h"
#include "entitysystem.h"

namespace ou {

Entity::Entity(std::vector<Component>&& components)
{
    for (Component& comp : components) {
        m_components.insert({ comp.type(), std::move(comp) });
    }
}

void Entity::added(ECSEngine* engine, Entity::ListIter iter)
{
    m_engine = engine;
    m_iter = iter;
}

void Entity::addComponent(Component&& component)
{
    if (m_engine) {
        m_engine->m_mappings[component.type()].insert(m_iter);
    }

    m_components.insert({ component.type(), std::move(component) });
}

void Entity::removeComponent(std::type_index type)
{
    if (m_components.count(type) == 0) {
        throw std::runtime_error("Component does not exist");
    }

    if (m_engine) {
        m_engine->m_mappings[type].erase(m_iter);
    }

    m_components.erase(type);
}

bool Entity::has(std::type_index idx) const
{
    return m_components.count(idx);
}

std::type_index Component::type() const
{
    return m_self->type();
}

const std::unordered_map<std::type_index, Component>& Entity::components() const
{
    return m_components;
}

Component::Interface::~Interface() = default;
}
