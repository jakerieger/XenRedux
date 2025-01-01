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
        [[nodiscard]] GameState clone() const;

        template<typename T>
        const T* getComponent(EntityId entity) const {
            if constexpr (std::is_same_v<T, TransformComponent>) {
                return _transforms.getComponent(entity);
            } else if constexpr (std::is_same_v<T, RenderComponent>) {
                return _renderables.getComponent(entity);
            }
            return nullptr;
        }

        template<typename T>
        T* getComponentMutable(EntityId entity) {
            if constexpr (std::is_same_v<T, TransformComponent>) {
                return _transforms.getComponentMutable(entity);
            } else if constexpr (std::is_same_v<T, RenderComponent>) {
                return _renderables.getComponentMutable(entity);
            }
            return nullptr;
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

        void setSun(const DirectionalLight& sun);
        // void updatePointLight(i32 index);
        // void updateSpotLight(i32 index);
        // void updateAreaLight(i32 index);
        void updateCameraState(glm::mat4 view, glm::mat4 projection, glm::vec3 position);

        void releaseAllResources();

    private:
        u64 _nextEntityId;

        ComponentManager<TransformComponent> _transforms;
        ComponentManager<RenderComponent> _renderables;

        struct {
            CameraState _camera;
            LightingState _lighting;
        } _globalState;

        template<typename T>
        void releaseComponentResources() {
            if constexpr (detail::release_resources<T>::value) {
                getComponents<T>().releaseResources();
            }
        }
    };
}  // namespace x
