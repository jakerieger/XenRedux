// Author: Jake Rieger
// Created: 12/31/2024.
//

#include "Scene.hpp"

#include <functional>

namespace x {
    EntityId Scene::createEntity(const std::optional<x::EntityId>& parent) {
        EntityId entity      = _state.createEntity();
        auto node            = std::make_shared<SceneNode>();
        node->entity         = entity;
        node->localTransform = glm::mat4(1.0f);
        node->worldTransform = glm::mat4(1.0f);
        if ((!parent.has_value()) || !parent->valid()) {}

        return entity;
    }

    void Scene::removeEntity(const EntityId& entity) {}

    void Scene::attachEntity(EntityId child, EntityId parent) {}

    void Scene::detachEntity(EntityId child) {}

    bool Scene::loadFromFile(const str& filename) {
        return false;
    }

    bool Scene::saveToFile(const str& filename) {
        return false;
    }

    void Scene::unload() {
        std::function<void(std::shared_ptr<SceneNode>)> destroyNode;
        destroyNode = [&](std::shared_ptr<SceneNode> node) {
            for (auto& child : node->children) {
                destroyNode(child);
            }
            _state.destroyEntity(node->entity);
        };

        if (_root) { destroyNode(_root); }

        _nodes.clear();
        _root.reset();
    }

    void Scene::setWorldTransform(EntityId entity, const glm::mat4& transform) {}

    glm::mat4 Scene::getWorldTransform(EntityId entity) const {
        return glm::mat4(1.0f);
    }

    void Scene::updateWorldTransforms(std::shared_ptr<SceneNode> node,
                                      const glm::mat4& parentTransform) {}
}  // namespace x