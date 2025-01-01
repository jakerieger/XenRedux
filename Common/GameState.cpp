// Author: Jake Rieger
// Created: 12/31/2024.
//

#include "GameState.hpp"

namespace x {
    EntityId GameState::createEntity() {
        auto newId = ++_nextEntityId;
        return EntityId(newId);
    }

    void GameState::destroyEntity(EntityId entity) {
        _transforms.removeComponent(entity);
        _renderables.removeComponent(entity);
    }

    GameState GameState::clone() const {
        GameState newState;

        // Deep copy all components and state
        // This is called when swapping buffers
        newState._nextEntityId = _nextEntityId;
        newState._globalState  = _globalState;

        // Component Managers
        newState._transforms  = _transforms;
        newState._renderables = _renderables;

        return newState;
    }

    void GameState::setSun(const DirectionalLight& sun) {
        _globalState._lighting.sun = sun;
    }

    void GameState::updateCameraState(glm::mat4 view, glm::mat4 projection, glm::vec3 position) {
        _globalState._camera.view       = view;
        _globalState._camera.projection = projection;
        _globalState._camera.position   = position;
    }

    void GameState::releaseAllResources() {
        releaseComponentResources<RenderComponent>();
    }
}  // namespace x