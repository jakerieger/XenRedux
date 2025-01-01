// Author: Jake Rieger
// Created: 12/25/2024.
//

#include "Game.hpp"
#include "PerspectiveCamera.hpp"
#include "Filesystem/Filesystem.hpp"
#include "Graphics/Pipeline.hpp"

#include <imgui/imgui.h>

using namespace x::Filesystem;
using namespace x::Graphics;

/// @briefs Returns the directory of this source file + "/Data".
/// Used for accessing data in the source directories without having
/// to copy it to the build dir.
static Path getDataPath() {
    const Path currentFile(__FILE__);
    const auto dataPath = currentFile.parent() / "Data";
    return dataPath;
}

class SpaceGame final : public x::IGame {
public:
    SpaceGame() : IGame("SpaceGame", 1600, 900, true, false) {}

    void loadContent(x::GameState& state) override;
    void unloadContent() override;
    void update(x::GameState& state) override;
    void draw(const x::GameState& state) override;
    void drawDebugUI(const x::GameState& state) override;
    void configurePipeline() override;
    void onKeyDown(u16 key) override;
    void onKeyUp(u16 key) override;
    void onMouseMove(i32 x, i32 y) override;
    void onMouseDown(u16 button, i32 x, i32 y) override;
    void onMouseUp(u16 button, i32 x, i32 y) override;

private:
    x::PerspectiveCamera _camera;
    x::ModelHandle _model;
};

void SpaceGame::loadContent(x::GameState& state) {
    x::EntityId modelEntity = state.createEntity();
    auto& transform         = state.addComponent<x::TransformComponent>(modelEntity);
    auto& renderer          = state.addComponent<x::RenderComponent>(modelEntity);
    auto modelPath          = getDataPath() / "ShaderBall.fbx";
    if (x::ModelHandle::tryLoad(modelPath.string(), _model)) {
        renderer.setModel(_model);
    } else {
        Panic("Failed to load model");
    }
    transform.setScale(glm::vec3(0.01f));
    transform.setPosition(glm::vec3(0.0f, -1.0f, 0.0f));
}

void SpaceGame::unloadContent() {}

void SpaceGame::update(x::GameState& state) {
    _camera.update();
    state.updateCameraState(_camera.getView(), _camera.getProjection(), _camera.getPosition());

    for (const auto& [entityId, transform] :
         state.getComponents<x::TransformComponent>().mutableView()) {
        transform.update();
    }

    // update other engine systems like physics or AI
}

void SpaceGame::draw(const x::GameState& state) {
    const auto& cameraState = state.getCameraState();
    const auto& lightState  = state.getLightingState();
    const auto& renderables = state.getComponents<x::RenderComponent>();
    for (const auto& [entityId, renderable] : renderables) {
        auto* transform = state.getComponent<x::TransformComponent>(entityId);
        if (transform) { renderable.draw(cameraState, lightState, *transform); }
    }
}

void SpaceGame::drawDebugUI(const x::GameState& state) {
    auto& cameraState = state.getCameraState();

    f32 mainThread   = _frameGraph.mainThreadTime.load();
    f32 renderThread = _frameGraph.renderThreadTime.load();
    f32 gpuTime      = _frameGraph.gpuTime.load();
    f32 frameTime    = _frameGraph.frameTime.load();
    f32 fps          = 1000.0f / frameTime;

    ImGui::SetNextWindowPos(ImVec2(4, 4));
    ImGui::Begin("Frame Graph",
                 nullptr,
                 ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize |
                   ImGuiWindowFlags_NoMove);
    ImGui::Text("Frame Timings (ms):");
    ImGui::Separator();

    // Create a more detailed breakdown
    ImGui::Columns(2, "timings");
    ImGui::Text("Main Thread:");
    ImGui::NextColumn();
    ImGui::Text("%.2f ms", mainThread);
    ImGui::NextColumn();

    ImGui::Text("Render Thread:");
    ImGui::NextColumn();
    ImGui::Text("%.2f ms", renderThread);
    ImGui::NextColumn();

    ImGui::Text("GPU Time:");
    ImGui::NextColumn();
    ImGui::Text("%.2f ms", gpuTime);
    ImGui::NextColumn();

    ImGui::Text("Total Frame:");
    ImGui::NextColumn();
    ImGui::Text("%.2f ms (%.1f FPS)", frameTime, fps);
    ImGui::NextColumn();

    ImGui::Columns(1);
    ImGui::Separator();

    // Add a simple frame time graph
    static float frameTimes[100] = {};
    static int frameTimeIndex    = 0;

    frameTimes[frameTimeIndex] = frameTime;
    frameTimeIndex             = (frameTimeIndex + 1) % IM_ARRAYSIZE(frameTimes);

    ImGui::PlotLines("Frame Times",
                     frameTimes,
                     IM_ARRAYSIZE(frameTimes),
                     frameTimeIndex,
                     nullptr,
                     0.0f,
                     33.3f,  // 33.3ms = 30 FPS
                     ImVec2(0, 80));

    ImGui::End();
}

void SpaceGame::configurePipeline() {
    Pipeline::setBlendMode(true);
    Pipeline::setCullMode(true);
    Pipeline::setDepthTest(true);
    Pipeline::setEnableHDR(true);
}

void SpaceGame::onKeyDown(u16 key) {}

void SpaceGame::onKeyUp(u16 key) {}

void SpaceGame::onMouseMove(i32 x, i32 y) {}

void SpaceGame::onMouseDown(u16 button, i32 x, i32 y) {}

void SpaceGame::onMouseUp(u16 button, i32 x, i32 y) {}

int main() {
    SpaceGame game;
    game.run();
    return 0;
}