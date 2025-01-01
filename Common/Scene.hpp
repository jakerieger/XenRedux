// Author: Jake Rieger
// Created: 12/31/2024.
//

#pragma once

#include "Types.hpp"
#include "GameState.hpp"

#include <pugixml.hpp>

namespace x {
    class Scene {
    public:
        Scene(const str& name, const GameState& state) : _name(name), _state(state) {}

        struct SceneNode {
            EntityId entity;
            std::vector<std::shared_ptr<SceneNode>> children;
            std::weak_ptr<SceneNode> parent;  // avoid circular deps by using weak ptr
            glm::mat4 localTransform;         // relative to parent
            glm::mat4 worldTransform;         // cached world transform
        };

        EntityId createEntity(const std::optional<x::EntityId>& parent = std::nullopt);
        void removeEntity(const EntityId& entity);

        void attachEntity(EntityId child, EntityId parent);
        void detachEntity(EntityId child);

        bool loadFromFile(const str& filename);
        bool saveToFile(const str& filename);
        void unload();

        void setWorldTransform(EntityId entity, const glm::mat4& transform);
        glm::mat4 getWorldTransform(EntityId entity) const;

    private:
        str _name;
        GameState _state;
        std::unordered_map<EntityId, std::shared_ptr<SceneNode>> _nodes;
        std::shared_ptr<SceneNode> _root;

        void updateWorldTransforms(std::shared_ptr<SceneNode> node,
                                   const glm::mat4& parentTransform);
    };
}  // namespace x
