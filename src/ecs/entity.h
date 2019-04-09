#ifndef ENTITY_H
#define ENTITY_H

#include <list>
#include <memory>
#include <typeindex>
#include <typeinfo>
#include <unordered_map>
#include <vector>

namespace ou {

class EntitySystem;
class ECSEngine;
class Component {
private:
    struct Interface {
        virtual ~Interface();
        virtual std::type_index type() const = 0;
        virtual Component clone() const = 0;
    };

    template <typename T>
    struct Model final : Interface {

        Model(T x)
            : data(std::move(x))
        {
        }

        Component clone() const { return data; }

        std::type_index type() const { return typeid(T); }

        T data;
    };

    std::unique_ptr<Interface> m_self;

public:
    template <typename T>
    Component(T x)
        : m_self(std::make_unique<Model<T>>(std::move(x)))
    {
    }

    Component(Component const& other)
        : m_self(other.m_self->clone().m_self)
    {
    }

    Component& operator=(Component const& other)
    {
        m_self = other.m_self->clone().m_self;
    }

    Component(Component&&) noexcept = default;
    Component& operator=(Component&&) noexcept = default;

    template <typename T>
    T const& get() const
    {
        if (m_self->type() != typeid(T)) {
            throw std::runtime_error("Types not equal");
        }
        return static_cast<Model<T> const*>(m_self.get())->data;
    }

    template <typename T>
    T& get()
    {
        if (m_self->type() != typeid(T)) {
            throw std::runtime_error("Types not equal");
        }
        return static_cast<Model<T>*>(m_self.get())->data;
    }

    template <typename T>
    bool is() const
    {
        Interface const& self = *m_self;
        return typeid(Model<T>) == typeid(self);
    }

    std::type_index type() const;
};

class Entity {
    std::unordered_map<std::type_index, Component> m_components;
    ECSEngine* m_engine = nullptr;

    using ListIter = std::list<Entity>::iterator;
    ListIter m_iter;

public:
    Entity() = default;
    Entity(std::vector<Component>&& components);

    void added(ECSEngine* engine, ListIter iter);

    void addComponent(Component&& component);

    void removeComponent(std::type_index type);

    template <typename T>
    void removeComponent() { removeComponent(typeid(T)); }

    bool has(std::type_index idx) const;

    template <typename T>
    bool has() const { return has(typeid(T)); }

    template <typename T>
    T& get() { return m_components.at(typeid(T)).get<T>(); }

    template <typename T>
    T const& get() const { return m_components.at(typeid(T)).get<T>(); }

    std::unordered_map<std::type_index, Component> const& components() const;
};
}

#endif // ENTITY_H
