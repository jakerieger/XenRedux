// Author: Jake Rieger
// Created: 12/31/2024.
//

#pragma once

#include "Types.hpp"
#include "ComponentManager.hpp"
#include "CameraState.hpp"
#include "LightingState.hpp"
#include "TransformComponent.hpp"
#include "RenderComponent.hpp"

#include <set>
#include <vector>
#include <unordered_map>

namespace x {
    class GameState {
    public:
        EntityId createEntity();
        void destroyEntity(EntityId entity);
        void setParent(EntityId child, EntityId parent);
        [[nodiscard]] GameState clone() const;

        template<typename T>
        const T* getComponent(EntityId entity) const {
            if constexpr (std::is_same_v<T, TransformComponent>) {
                return _transforms.getComponent(entity);
            } else if constexpr (std::is_same_v<T, RenderComponent>) {
                return _renderables.getComponent(entity);
            }
        }

        template<typename T>
        T& addComponent(EntityId entity) {
            if constexpr (std::is_same_v<T, TransformComponent>) {
                return _transforms.addComponent(entity).component;
            } else if constexpr (std::is_same_v<T, RenderComponent>) {
                return _renderables.addComponent(entity).component;
            }
        }

        template<typename T>
        const ComponentManager<T>& getComponents() const {
            if constexpr (std::is_same_v<T, TransformComponent>) {
                return _transforms;
            } else if constexpr (std::is_same_v<T, RenderComponent>) {
                return _renderables;
            }
        }

        template<typename T>
        ComponentManager<T>& getComponents() {
            if constexpr (std::is_same_v<T, TransformComponent>) {
                return _transforms;
            } else if constexpr (std::is_same_v<T, RenderComponent>) {
                return _renderables;
            }
        }

        [[nodiscard]] CameraState const& getCameraState() const {
            return _globalState._camera;
        }

        [[nodiscard]] LightingState const& getLightingState() const {
            return _globalState._lighting;
        }

        void updateCameraState(glm::mat4 view, glm::mat4 projection, glm::vec3 position);

    private:
        std::set<EntityId> _activeEntities;
        EntityId _nextEntityId = 0;

        ComponentManager<TransformComponent> _transforms;
        ComponentManager<RenderComponent> _renderables;

        struct HierarchyNode {
            EntityId parent = 0;
            std::vector<EntityId> children;
        };

        std::unordered_map<EntityId, HierarchyNode> _hierarchy;

        struct {
            CameraState _camera;
            LightingState _lighting;
        } _globalState;
    };
}  // namespace x
