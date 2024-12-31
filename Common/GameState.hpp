// Author: Jake Rieger
// Created: 12/31/2024.
//

#pragma once

#include "Types.hpp"
#include "ComponentManager.hpp"
#include "CameraState.hpp"
#include "LightingState.hpp"
#include "TransformComponent.hpp"

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
        T* getComponent(EntityId entity) {
            if constexpr (std::is_same_v<T, TransformComponent>) {
                return _transforms.getComponent(entity);
            }
            // else if constexpr (std::is_same_v<T, PhysicsComponent>) {
            //     return _physics.getComponent(entity);
            // } else if constexpr (std::is_same_v<T, RenderComponent>) {
            //     return _renderables.getComponent(entity);
            // }
        }

        template<typename T>
        T& addComponent(EntityId entity) {
            if constexpr (std::is_same_v<T, TransformComponent>) {
                return _transforms.addComponent(entity);
            }
            // else if constexpr (std::is_same_v<T, PhysicsComponent>) {
            //     return _physics.addComponent(entity);
            // } else if constexpr (std::is_same_v<T, RenderComponent>) {
            //     return _renderables.addComponent(entity);
            // }
        }

        template<typename T>
        std::vector<T> getComponents() {
            if constexpr (std::is_same_v<T, TransformComponent>) { return _transforms; }
        }

        CameraState& getCameraState() {
            return _globalState._camera;
        }

        LightingState& getLightingState() {
            return _globalState._lighting;
        }

        void updateCameraState(glm::mat4& view, glm::mat4& projection, glm::vec3& position);

    private:
        std::set<EntityId> _activeEntities;
        EntityId _nextEntityId = 0;

        ComponentManager<TransformComponent> _transforms;
        // ComponentManager<PhysicsComponent> _physics;
        // ComponentManager<RenderComponent> _renderables;

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
