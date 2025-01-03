// Author: Jake Rieger
// Created: 1/3/2025.
//

#pragma once

#include "UI.hpp"
#include "Layout.hpp"
#include "RenderContext.hpp"

#include <optional>

namespace x::UI {

    class UIElement : public std::enable_shared_from_this<UIElement> {
    protected:
        str id;
        Rect bounds;
        bool visible = true;
        bool enabled = true;
        WeakElementPtr parent;
        std::vector<ElementPtr> children;
        std::shared_ptr<Style> stylePtr;
        std::shared_ptr<Layout> layoutPtr;
        StyleMap properties;
        std::unordered_map<Event::Type, std::vector<EventCallback>> eventHandlers;

    public:
        explicit UIElement(str elementId) : id(std::move(elementId)) {}
        virtual ~UIElement() = default;

        void addChild(ElementPtr child) {
            if (child) {
                child->parent = weak_from_this();
                children.push_back(std::move(child));
                invalidateLayout();
            }
        }

        void removeChild(const ElementPtr& child) {
            auto it = std::find(children.begin(), children.end(), child);
            if (it != children.end()) {
                (*it)->parent.reset();
                children.erase(it);
                invalidateLayout();
            }
        }

        virtual void layout() {
            if (layoutPtr) layoutPtr->apply(*this);
            for (auto& child : children) {
                child->layout();
            }
        }

        virtual void render(class RenderContext& context) {
            if (!visible) return;
            context.pushTransform(bounds);
            if (stylePtr) { context.pushStyle(*stylePtr); }

            onRender(context);

            for (auto& child : children) {
                child->render(context);
            }

            if (stylePtr) { context.popStyle(); }
            context.popTransform();
        }

        void addEventListener(Event::Type type, EventCallback callback) {
            eventHandlers[type].push_back(std::move(callback));
        }

        bool handleEvent(const Event& event) {
            if (!enabled) return false;
            for (auto it = children.rbegin(); it != children.rend(); ++it) {
                if ((*it)->handleEvent(event)) { return true; }
            }
            if (auto it = eventHandlers.find(event.getType()); it != eventHandlers.end()) {
                for (const auto& handler : it->second) {
                    handler(event);
                    if (event.isHandled()) { return true; }
                }
            }
            return false;
        }

        template<typename T>
        void setProperty(const str& name, T value) {
            properties[name] = PropertyValue(std::move(value));
            invalidateLayout();
        }

        template<typename T>
        std::optional<T> getProperty(const str& name) const {
            if (auto it = properties.find(name); it != properties.end()) {
                if (const T* value = std::get_if<T>(&it->second)) { return *value; }
            }
            return {};
        }

    protected:
        virtual void onRender(RenderContext& context) = 0;

        void invalidateLayout() {
            if (auto p = parent.lock()) { p->invalidateLayout(); }
        }
    };

}  // namespace x::UI