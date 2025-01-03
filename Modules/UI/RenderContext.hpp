// Author: Jake Rieger
// Created: 1/3/2025.
//

#pragma once

#include <glad.h>
#include "UI.hpp"
#include "Style.hpp"

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace x::UI {
    class RenderContext {
        struct State {
            glm::mat4 transform;
            Style style;
        };

        std::vector<State> stateStack;
        u32 currentProgram = 0;
        u32 currentVAO     = 0;
        u32 rectProgram    = 0;
        u32 rectVAO        = 0;
        i32 transformLoc;
        i32 colorLoc;

    public:
        void pushTransform(const Rect& bounds) {
            glm::mat4 transform =
              glm::translate(glm::mat4(1.0f), glm::vec3(bounds.x, bounds.y, 0.0f));
            transform = glm::scale(transform, glm::vec3(bounds.width, bounds.height, 1.0f));
            if (!stateStack.empty()) { transform = stateStack.back().transform * transform; }
            State state;
            state.transform = transform;
            stateStack.push_back(std::move(state));
        }

        void popTransform() {
            if (!stateStack.empty()) { stateStack.pop_back(); }
        }

        void pushStyle(const Style& style) {
            State state = stateStack.back() ? State() : stateStack.back();
            if (auto color = style.getProperty("color")) {
                state.style.setProperty("color", *color);
            }
            stateStack.push_back(std::move(state));
        }

        void popStyle() {
            if (!stateStack.empty()) { stateStack.pop_back(); }
        }

        void drawRect(const Rect& rect, const glm::vec4& color) {
            if (currentProgram != rectProgram) {
                glUseProgram(rectProgram);
                currentProgram = rectProgram;
            }
            glUniformMatrix4fv(transformLoc,
                               1,
                               GL_FALSE,
                               glm::value_ptr(stateStack.back().transform));
            glUniform4fv(colorLoc, 1, glm::value_ptr(color));

            glBindVertexArray(rectVAO);
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        }
    };
}  // namespace x::UI