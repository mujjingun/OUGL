#include "ecsengine.h"
#include <algorithm>
#include <iostream>

namespace ou {

std::mt19937& ECSEngine::rand()
{
    return m_gen;
}

ECSEngine::ECSEngine()
    : m_entities{}
{
}

void ECSEngine::addEntity(Entity&& entity)
{
    m_entities.push_back(std::move(entity));
    ListIter it = std::prev(m_entities.end());
    m_entities.back().added(this, it);
    for (auto const& comp : m_entities.back().components()) {
        m_mappings[comp.first].insert(it);
    }
}

void ECSEngine::removeEntities(ECSEngine::Iterator first, ECSEngine::Iterator last, std::function<bool(Entity&)> pred)
{
    Iterator it = first;
    for (; it != last;) {
        if (!pred(*it)) {
            ++it;
            continue;
        }
        auto list_iter = *it.it;
        bool incremented = false;
        for (auto const& comp : it->components()) {
            auto& mapping = m_mappings[comp.first];
            auto found = mapping.find(list_iter);
            if (comp.first != it.prime_key) {
                mapping.erase(found);
            } else {
                it.it = mapping.erase(found);
                it.moveToNext();
                incremented = true;
            }
        }
        if (!incremented) {
            ++it;
        }
        m_entities.erase(list_iter);
    }
}

std::size_t ECSEngine::countEntity() const
{
    return m_entities.size();
}

void ECSEngine::addSystem(std::unique_ptr<EntitySystem>&& system, int priority)
{
    m_systems.insert({ priority, std::move(system) });
}

void ECSEngine::update(float deltaTime)
{
    for (auto const& pair : m_systems) {
        pair.second->update(*this, deltaTime);
    }
}

ECSEngine::Range::Range(ECSEngine* engine, std::vector<std::type_index>&& keys)
{
    std::type_index prime_key = *std::min_element(keys.begin(), keys.end(),
        [&](std::type_index t1, std::type_index t2) {
            return engine->m_mappings[t1].size() < engine->m_mappings[t2].size();
        });

    keys.erase(std::remove(keys.begin(), keys.end(), prime_key), keys.end());

    auto& map = engine->m_mappings[prime_key];
    m_begin = Iterator(map.begin(), map.end(), prime_key, keys);
    m_end = Iterator(map.end(), map.end(), prime_key, keys);
}

ECSEngine::Iterator ECSEngine::Range::begin()
{
    return m_begin;
}

ECSEngine::Iterator ECSEngine::Range::end()
{
    return m_end;
}

ECSEngine::Iterator::Iterator(MappingIter it, MappingIter end, std::type_index prime_key,
    const std::vector<std::type_index>& other_keys)
    : it(it)
    , end(end)
    , prime_key(prime_key)
    , other_keys(other_keys)
{
    moveToNext();
}

void ECSEngine::Iterator::moveToNext()
{
    if (other_keys.size() > 0) {
        it = std::find_if(it, end, [&](ListIter it) {
            return std::all_of(other_keys.begin(), other_keys.end(),
                [&](std::type_index type) { return it->has(type); });
        });
    }
}

ECSEngine::Iterator& ECSEngine::Iterator::operator++()
{
    if (it == end) {
        throw std::runtime_error("Attempt to increment the past-the-end iterator");
    }
    ++it;
    moveToNext();
    return *this;
}

ECSEngine::Iterator ECSEngine::Iterator::operator++(int)
{
    Iterator prev = *this;
    ++*this;
    return prev;
}

bool ECSEngine::Iterator::operator==(Iterator other) const
{
    return it == other.it;
}

bool ECSEngine::Iterator::operator!=(Iterator other) const
{
    return !(*this == other);
}

Entity& ECSEngine::Iterator::operator*() const
{
    return **it;
}

Entity* ECSEngine::Iterator::operator->() const
{
    return &*(*this);
}
}
