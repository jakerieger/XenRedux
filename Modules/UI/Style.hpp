// Author: Jake Rieger
// Created: 1/3/2025.
//

#pragma once

#include "UI.hpp"

#include <optional>

namespace x::UI {
    class Style {
        StyleMap properties;
        std::shared_ptr<Style> parent;

    public:
        void setProperty(const str& name, PropertyValue value) {
            properties[name] = std::move(value);
        }

        std::optional<PropertyValue> getProperty(const str& name) const {
            if (auto it = properties.find(name); it != properties.end()) { return it->second; }
            if (parent) { return parent->getProperty(name); }
            return {};
        }

        void setParent(std::shared_ptr<Style> parent) {
            this->parent = std::move(parent);
        }
    };
}  // namespace x::UI