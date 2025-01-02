// Author: Jake Rieger
// Created: 12/31/2024.
//

#pragma once

#include "Resource.hpp"
#include "Types.hpp"

#include <vector>
#include <unordered_map>
#include <numeric>

namespace x {
    // TODO: Put this in its own class file
    class EntityId {
    public:
        constexpr EntityId() : _value(kInvalidValue) {}
        explicit constexpr EntityId(u64 value) : _value(value) {}

        constexpr u64 value() const {
            return _value;
        }

        constexpr bool operator==(const EntityId& other) const {
            return _value == other._value;
        }

        constexpr bool operator!=(const EntityId& other) const {
            return _value != other._value;
        }

        constexpr bool operator<(const EntityId& other) const {
            return _value < other._value;
        }

        constexpr bool operator>(const EntityId& other) const {
            return _value > other._value;
        }

        constexpr bool operator<=(const EntityId& other) const {
            return _value <= other._value;
        }

        constexpr bool operator>=(const EntityId& other) const {
            return _value >= other._value;
        }

        constexpr u64 operator*() const {
            return _value;
        }

        constexpr bool valid() const {
            return _value != kInvalidValue;
        }

        static constexpr EntityId Invalid() {
            return EntityId();
        }

    private:
        u64 _value;
        static constexpr u64 kInvalidValue = std::numeric_limits<u64>::max();
    };

    namespace detail {
        template<typename T>
        struct release_resources {
            static constexpr bool value = std::is_base_of_v<Resource, T>;
        };
    }  // namespace detail
}  // namespace x

#ifndef X_ENTITY_ID_HASH_SPECIALIZATION
    #define X_ENTITY_ID_HASH_SPECIALIZATION
// Allow EntityId to be used in std::unordered_map/set
namespace std {
    template<>
    struct hash<x::EntityId> {
        std::size_t operator()(const x::EntityId& id) const {
            return std::hash<u64> {}(id.value());
        }
    };
}  // namespace std
#endif

namespace x {
    // Forward declarations
    class TransformComponent;
    class PhysicsComponent;
    class RenderComponent;

    template<typename T>
    class ComponentManager {
    private:
        std::vector<T> _components;
        std::unordered_map<EntityId, size_t> _entityToIndex;
        std::vector<EntityId> _indexToEntity;

    public:
        void releaseResources() {
            if constexpr (detail::release_resources<T>::value) {
                for (auto& component : _components) {
                    component.release();
                }
            }
        }

        struct ComponentView {
            EntityId entity;
            T& component;
        };

        class Iterator {
        private:
            std::vector<T>& _components;
            std::vector<EntityId>& _entities;
            size_t _index;

        public:
            Iterator(std::vector<T>& components, std::vector<EntityId>& entities, size_t index)
                : _components(components), _entities(entities), _index(index) {}

            ComponentView operator*() const {
                return {_entities[_index], _components[_index]};
            }

            Iterator& operator++() {
                ++_index;
                return *this;
            }

            bool operator!=(const Iterator& other) const {
                return _index != other._index;
            }

            bool operator==(const Iterator& other) const {
                return _index == other._index;
            }
        };

        struct ConstComponentView {
            EntityId entity;
            const T& component;
        };

        Iterator beginMutable() {
            return {_components, _indexToEntity, 0};
        }

        Iterator endMutable() {
            return {_components, _indexToEntity, _components.size()};
        }

        class MutableView {
        private:
            ComponentManager& _manager;

        public:
            MutableView(ComponentManager& manager) : _manager(manager) {}
            Iterator begin() const {
                return _manager.beginMutable();
            }
            Iterator end() const {
                return _manager.endMutable();
            }
        };

        MutableView mutableView() {
            return {*this};
        }

        class ConstIterator {
        private:
            const std::vector<T>& _components;
            const std::vector<EntityId>& _entities;
            size_t _index;

        public:
            ConstIterator(const std::vector<T>& components,
                          const std::vector<EntityId>& entities,
                          size_t index)
                : _components(components), _entities(entities), _index(index) {}

            ConstComponentView operator*() const {
                return {_entities[_index], _components[_index]};
            }

            ConstIterator& operator++() {
                ++_index;
                return *this;
            }

            bool operator!=(const ConstIterator& other) const {
                return _index != other._index;
            }

            bool operator==(const ConstIterator& other) const {
                return _index == other._index;
            }
        };

        ConstIterator begin() const {
            return ConstIterator(_components, _indexToEntity, 0);
        }

        ConstIterator end() const {
            return ConstIterator(_components, _indexToEntity, _components.size());
        }

        ComponentView addComponent(EntityId entity) {
            size_t newIndex = _components.size();
            _components.emplace_back();
            _entityToIndex[entity] = newIndex;
            _indexToEntity.push_back(entity);
            return {entity, _components.back()};
        }

        void removeComponent(EntityId entity) {
            auto it = _entityToIndex.find(entity);
            if (it != _entityToIndex.end()) {
                size_t indexToRemove = it->second;
                size_t lastIndex     = _components.size() - 1;
                if (indexToRemove != lastIndex) {
                    _components[indexToRemove]    = std::move(_components[lastIndex]);
                    EntityId movedEntity          = _indexToEntity[lastIndex];
                    _entityToIndex[movedEntity]   = indexToRemove;
                    _indexToEntity[indexToRemove] = movedEntity;
                }
                _components.pop_back();
                _indexToEntity.pop_back();
                _entityToIndex.erase(entity);
            }
        }

        const T* getComponent(EntityId entity) const {
            auto it = _entityToIndex.find(entity);
            if (it != _entityToIndex.end()) { return &_components[it->second]; }
            return nullptr;
        }

        T* getComponentMutable(EntityId entity) {
            auto it = _entityToIndex.find(entity);
            if (it != _entityToIndex.end()) { return &_components[it->second]; }
            return nullptr;
        }

        EntityId getEntity(const T* component) const {
            size_t index = component - _components.data();
            if (index < _components.size()) { return _indexToEntity[index]; }
            return EntityId {0};
        }

        const std::vector<T>& getRawComponents() const {
            return _components;
        }
    };
}  // namespace x