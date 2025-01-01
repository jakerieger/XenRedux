// Author: Jake Rieger
// Created: 12/31/2024.
//

#include "GameState.hpp"

namespace x {
    EntityId GameState::createEntity() {
        EntityId newId = ++_nextEntityId;
        _activeEntities.insert(newId);
        return newId;
    }

    void GameState::destroyEntity(EntityId entity) {
        _transforms.removeComponent(entity);
        _renderables.removeComponent(entity);

        auto hierarchyIt = _hierarchy.find(entity);
        if (hierarchyIt != _hierarchy.end()) {
            auto& node = hierarchyIt->second;
            if (node.parent != 0) {
                auto& parentChildren = _hierarchy[node.parent].children;
                parentChildren.erase(
                  std::remove(parentChildren.begin(), parentChildren.end(), entity),
                  parentChildren.end());
            }
            // Reparent any children to root (or you could choose to destroy them)
            for (EntityId child : node.children) {
                setParent(child, 0);
            }
            _hierarchy.erase(hierarchyIt);
        }

        _activeEntities.erase(entity);
    }

    void GameState::setParent(EntityId child, EntityId parent) {
        auto& childNode = _hierarchy[child];
        if (childNode.parent != 0) {
            auto& oldParentChildren = _hierarchy[childNode.parent].children;
            oldParentChildren.erase(
              std::remove(oldParentChildren.begin(), oldParentChildren.end(), child),
              oldParentChildren.end());
        }
        childNode.parent = parent;
        if (parent != 0) { _hierarchy[parent].children.push_back(child); }
    }

    GameState GameState::clone() const {
        GameState newState;

        // Deep copy all components and state
        // This is called when swapping buffers
        newState._activeEntities = _activeEntities;
        newState._nextEntityId   = _nextEntityId;
        newState._hierarchy      = _hierarchy;
        newState._globalState    = _globalState;

        // Component Managers
        newState._transforms  = _transforms;
        newState._renderables = _renderables;

        return newState;
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