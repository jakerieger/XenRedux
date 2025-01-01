// Author: Jake Rieger
// Created: 12/31/2024.
//

#include "Scene.hpp"

#include <functional>
#include <pugixml.hpp>

namespace x {
    EntityId Scene::createEntity(const std::optional<x::EntityId>& parent) {
        EntityId entity      = _state.createEntity();
        auto node            = std::make_shared<SceneNode>();
        node->entity         = entity;
        node->localTransform = glm::mat4(1.0f);
        node->worldTransform = glm::mat4(1.0f);

        if ((!parent.has_value()) || !parent->valid()) {
            if (!_root) {
                _root = node;
            } else {
                // add as child of root
                node->parent = _root;
                _root->children.push_back(node);
            }
        } else {
            // add as child of specified parent
            auto parentNode = _nodes[*parent];
            node->parent    = parentNode;
            parentNode->children.push_back(node);
        }

        _nodes[entity] = node;
        return entity;
    }

    void Scene::removeEntity(const EntityId& entity) {
        auto nodeIt = _nodes.find(entity);
        if (nodeIt == _nodes.end()) { return; }

        auto node = nodeIt->second;
        for (auto& child : node->children) {
            removeEntity(child->entity);
        }

        if (auto parent = node->parent.lock()) {
            auto& siblings = parent->children;
            siblings.erase(std::remove_if(siblings.begin(),
                                          siblings.end(),
                                          [entity](const auto& n) { return n->entity == entity; }),
                           siblings.end());
        }

        _nodes.erase(entity);
        _state.destroyEntity(entity);
        if (_root && _root->entity == entity) { _root.reset(); }
    }

    void Scene::attachEntity(EntityId child, EntityId parent) {
        auto childIt = _nodes.find(child);
        if (childIt == _nodes.end()) { return; }

        auto parentIt = _nodes.find(parent);
        if (parentIt == _nodes.end()) { return; }

        auto childNode  = childIt->second;
        auto parentNode = parentIt->second;

        // Store the child's current world transform before we modify its hierarchy.
        // This lets us maintain its world position after reparenting.
        glm::mat4 childWorldTransform = childNode->worldTransform;

        // if child is already attached to a different parent, remove it from that parent first.
        if (auto oldParent = childNode->parent.lock()) {
            auto& oldParentChildren = oldParent->children;
            oldParentChildren.erase(
              std::remove_if(oldParentChildren.begin(),
                             oldParentChildren.end(),
                             [child](const auto& node) { return node->entity == child; }),
              oldParentChildren.end());
        }

        // update the hierarchy relationships
        childNode->parent = parentNode;
        parentNode->children.push_back(childNode);

        // Calculate the new local transform that will maintain the child's world position
        // worldTransform = parentWorldTransform * localTransform
        // Therefore: localTransform = inverse(parentWorldTransform) * worldTransform
        childNode->localTransform = glm::inverse(parentNode->worldTransform) * childWorldTransform;

        // update the transforms for this node an all its children
        updateWorldTransforms(childNode, parentNode->worldTransform);
    }

    void Scene::detachEntity(EntityId child) {
        auto childIt = _nodes.find(child);
        if (childIt == _nodes.end()) { return; }

        auto childNode  = childIt->second;
        auto parentNode = childNode->parent.lock();
        if (!parentNode) { return; }

        glm::mat4 worldTransform = childNode->worldTransform;
        auto& parentChildren     = parentNode->children;
        parentChildren.erase(
          std::remove_if(parentChildren.begin(),
                         parentChildren.end(),
                         [child](const auto& node) { return node->entity == child; }),
          parentChildren.end());
        childNode->parent.reset();
        childNode->localTransform = worldTransform;

        // If we have a root node and this isn't it, make it a child of root
        if (_root && childNode != _root) {
            childNode->parent = _root;
            _root->children.push_back(childNode);
            childNode->localTransform = glm::inverse(_root->worldTransform) * worldTransform;
            updateWorldTransforms(childNode, _root->worldTransform);
        } else {
            childNode->worldTransform = worldTransform;
            updateWorldTransforms(childNode, glm::mat4(1.0f));
        }
    }

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

    void Scene::setWorldTransform(EntityId entity, const glm::mat4& worldTransform) {
        auto nodeIt = _nodes.find(entity);
        if (nodeIt == _nodes.end()) { return; }

        auto node   = nodeIt->second;
        auto parent = node->parent.lock();
        if (parent) {
            node->localTransform = glm::inverse(parent->worldTransform) * worldTransform;
        } else {
            node->localTransform = worldTransform;
        }

        updateWorldTransforms(node, parent ? parent->worldTransform : glm::mat4(1.0f));
    }

    glm::mat4 Scene::getWorldTransform(EntityId entity) const {
        auto nodeIt = _nodes.find(entity);
        if (nodeIt == _nodes.end()) { return glm::mat4(1.0f); }
        return nodeIt->second->worldTransform;
    }

    void Scene::updateWorldTransforms(std::shared_ptr<SceneNode> node,
                                      const glm::mat4& parentTransform) {
        node->worldTransform = parentTransform * node->localTransform;
        if (auto* transform = _state.getComponentMutable<TransformComponent>(node->entity)) {
            // Extract position from four column of matrix
            glm::vec3 position = glm::vec3(node->worldTransform[3]);

            // extract scale via measuring length of each basis vector
            glm::vec3 scale(glm::length(glm::vec3(node->worldTransform[0])),
                            glm::length(glm::vec3(node->worldTransform[1])),
                            glm::length(glm::vec3(node->worldTransform[2])));

            // create pure rotation matrix by removing scale
            glm::mat3 rotationMatrix(glm::vec3(node->worldTransform[0]) / scale.x,
                                     glm::vec3(node->worldTransform[1]) / scale.y,
                                     glm::vec3(node->worldTransform[2]) / scale.z);

            // extract rotation euler angles in X->Y->Z order
            glm::vec3 eulerAngles;
            eulerAngles.y = glm::degrees(atan2(rotationMatrix[2][0], rotationMatrix[0][0]));
            f32 cosY      = cos(glm::radians(eulerAngles.y));
            if (abs(cosY) > 0.0001f) {
                // Normal case, no gimbal lock
                eulerAngles.x = glm::degrees(atan2(-rotationMatrix[1][2], rotationMatrix[1][1]));
                eulerAngles.z = glm::degrees(atan2(-rotationMatrix[2][1], rotationMatrix[0][1]));
            } else {
                // gimbal lock :(
                eulerAngles.x = glm::degrees(atan2(rotationMatrix[1][0], rotationMatrix[1][1]));
                eulerAngles.z = 0.0f;
            }

            transform->setPosition(position);
            transform->setRotation(eulerAngles);
            transform->setScale(scale);
        }

        for (const auto& child : node->children) {
            updateWorldTransforms(child, node->worldTransform);
        }
    }
}  // namespace x