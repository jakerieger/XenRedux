// Author: Jake Rieger
// Created: 12/19/2024.
//

#include <glad.h>
#include "Pipeline.hpp"

namespace x::Graphics {
    void Pipeline::setBlendMode(bool enable) {
        if (enable) {
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        } else {
            glDisable(GL_BLEND);
        }
    }

    void Pipeline::setDepthTest(bool enable) {
        if (enable) {
            glEnable(GL_DEPTH_TEST);
        } else {
            glDisable(GL_DEPTH_TEST);
        }
    }

    void Pipeline::setCullMode(bool enable) {
        if (enable) {
            glEnable(GL_CULL_FACE);
            glCullFace(GL_BACK);
            glFrontFace(GL_CCW);
        } else {
            glDisable(GL_CULL_FACE);
        }
    }

    void Pipeline::setPolygonMode(bool wireframe) {
        glPolygonMode(GL_FRONT_AND_BACK, wireframe ? GL_LINE : GL_FILL);
    }

    void Pipeline::setEnableHDR(bool enable) {
        glClampColor(GL_CLAMP_READ_COLOR, GL_FALSE);
    }
}  // namespace x::Graphics