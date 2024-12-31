// Author: Jake Rieger
// Created: 12/31/2024.
//

#pragma once

#include "Types.hpp"

#include <vector>
#include <unordered_map>

namespace x {
    // Forward declarations
    class TransformComponent;
    class PhysicsComponent;
    class RenderComponent;

    using EntityId = u64;

    template<typename T>
    class ComponentManager {
    private:
        std::vector<T> _components;
        std::unordered_map<EntityId, size_t> _entityToIndex;
        std::unordered_map<size_t, EntityId> _indexToEntity;

    public:
        T& addComponent(EntityId entity) {
            size_t newIndex = _components.size();
            _components.emplace_back();
            _entityToIndex[entity]   = newIndex;
            _indexToEntity[newIndex] = entity;
            return _components.back();
        }

        T* getComponent(EntityId entity) {
            auto it = _entityToIndex.find(entity);
            if (it != _entityToIndex.end()) { return &(_components[it->second]); }
            return nullptr;
        }

        void removeComponent(EntityId entity) {
            auto it = _entityToIndex.find(entity);
            if (it != _entityToIndex.end()) {
                // Move last element to removed position to maintain contiguous memory
                size_t indexToRemove = it->second;
                size_t lastIndex     = _components.size() - 1;
                if (indexToRemove != lastIndex) {
                    _components[indexToRemove]    = std::move(_components[lastIndex]);
                    EntityId movedEntity          = _indexToEntity.at(lastIndex);
                    _entityToIndex[movedEntity]   = indexToRemove;
                    _indexToEntity[indexToRemove] = movedEntity;
                }
                _components.pop_back();
                _entityToIndex.erase(entity);
                _indexToEntity.erase(lastIndex);
            }
        }

        const std::vector<T>& getAllComponents() const {
            return _components;
        }
    };
}  // namespace x
