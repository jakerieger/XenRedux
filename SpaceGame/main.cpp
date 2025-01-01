// Author: Jake Rieger
// Created: 12/25/2024.
//

#include "Game.hpp"
#include "PBRMaterial.hpp"
#include "PerspectiveCamera.hpp"
#include "Scene.hpp"
#include "Filesystem/Filesystem.hpp"
#include "Graphics/Pipeline.hpp"
#include "Graphics/PostProcessQuad.hpp"
#include "Graphics/RenderTarget.hpp"
#include "Graphics/Effects/Tonemapper.hpp"

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
    std::unique_ptr<x::Scene> _activeScene;
    std::unique_ptr<RenderTarget> _renderTarget;
    std::unique_ptr<PostProcessQuad> _postProcessQuad;
    std::unique_ptr<Tonemapper> _tonemapper;
};

void SpaceGame::loadContent(x::GameState& state) {
    _activeScene = std::make_unique<x::Scene>("MainScene", state);
    auto root    = _activeScene->createEntity();

    // Load the shader ball model
    auto modelPath = getDataPath() / "ShaderBall.fbx";
    if (!x::ModelHandle::tryLoad(modelPath.string(), _model)) { Panic("Failed to load model"); }

    auto model1      = _activeScene->createEntity(root);
    auto& transform1 = state.addComponent<x::TransformComponent>(model1);
    auto& renderer1  = state.addComponent<x::RenderComponent>(model1);
    transform1.setScale(glm::vec3(0.01f));
    transform1.setPosition(glm::vec3(0, -1.25, -3));
    renderer1.setModel(_model);
    renderer1.getMaterial()->As<x::PBRMaterial>()->setAlbedo(glm::vec3(1.f, 0.5f, 0.0f));
    renderer1.getMaterial()->As<x::PBRMaterial>()->setMetallic(1.f);
    renderer1.getMaterial()->As<x::PBRMaterial>()->setRoughness(0.15f);

    auto model2      = _activeScene->createEntity(root);
    auto& transform2 = state.addComponent<x::TransformComponent>(model2);
    auto& renderer2  = state.addComponent<x::RenderComponent>(model2);
    transform2.setScale(glm::vec3(0.008f));
    transform2.setPosition(glm::vec3(-2, -1.25, -1));
    renderer2.setModel(_model);

    auto model3      = _activeScene->createEntity(root);
    auto& transform3 = state.addComponent<x::TransformComponent>(model3);
    auto& renderer3  = state.addComponent<x::RenderComponent>(model3);
    transform3.setScale(glm::vec3(0.008f));
    transform3.setPosition(glm::vec3(2, -1.25, -1));
    renderer3.setModel(_model);

    x::DirectionalLight sun;
    sun.setDirection(glm::vec3(-1, -1, -1));
    sun.setColor(1.f, 1.f, 1.f);
    sun.setIntensity(100.f);
    state.setSun(sun);

    _renderTarget    = std::make_unique<RenderTarget>(1600, 900, true);
    _postProcessQuad = std::make_unique<PostProcessQuad>();
    _tonemapper      = std::make_unique<Tonemapper>();
    _tonemapper->setTextureSize(1600, 900);
    _tonemapper->setInputTexture(_renderTarget->getColorTexture());
    _tonemapper->setTonemapOperator(0);
}

void SpaceGame::unloadContent() {
    _renderTarget.reset();
    _postProcessQuad.reset();
}

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

    // Scene pass
    _renderTarget->bind();
    x::Context::clear();
    const auto& renderables = state.getComponents<x::RenderComponent>();
    for (const auto& [entityId, renderable] : renderables) {
        auto* transform = state.getComponent<x::TransformComponent>(entityId);
        if (transform) { renderable.draw(cameraState, lightState, *transform); }
    }
    _renderTarget->unbind();

    // Post processing pass
    _tonemapper->apply();
    _postProcessQuad->draw(_tonemapper->getOutputTexture());
}

void SpaceGame::drawDebugUI(const x::GameState& state) {
    auto& cameraState = state.getCameraState();

    f32 mainThread   = _frameGraph.mainThreadTime.load();
    f32 renderThread = _frameGraph.renderThreadTime.load();
    f32 gpuTime      = _frameGraph.gpuTime.load();
    f32 frameTime    = _frameGraph.frameTime.load();
    f32 fps          = 1000.0f / frameTime;

    ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize |
                                   ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse;

    ImGui::SetNextWindowPos(ImVec2(4, 4));
    ImGui::SetNextWindowBgAlpha(0.4f);
    ImGui::Begin("Frame Graph", nullptr, windowFlags);
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

    ImGui::Text("ImGui Frame:");
    ImGui::NextColumn();
    ImGui::Text("%.2f ms (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    ImGui::NextColumn();

    ImGui::Columns(1);
    ImGui::Separator();

    // Add a simple frame time graph
    static float frameTimes[100] = {};
    static int frameTimeIndex    = 0;

    frameTimes[frameTimeIndex] = frameTime;
    frameTimeIndex             = (frameTimeIndex + 1) % IM_ARRAYSIZE(frameTimes);

    // Find the maximum value in our current buffer to determine color
    float maxFrameTime = 0.0f;
    for (int i = 0; i < IM_ARRAYSIZE(frameTimes); i++) {
        maxFrameTime = std::max(maxFrameTime, frameTimes[i]);
    }

    // Choose color based on max frame time
    ImVec4 plotColor;
    if (maxFrameTime <= 16.6f) {                     // 60 FPS
        plotColor = ImVec4(0.0f, 1.0f, 0.0f, 1.0f);  // Green
    } else if (maxFrameTime <= 33.3f) {              // 30 FPS
        plotColor = ImVec4(1.0f, 1.0f, 0.0f, 1.0f);  // Yellow
    } else {
        plotColor = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);  // Red
    }

    // Save the current plot color
    ImVec4 oldPlotColor = ImGui::GetStyle().Colors[ImGuiCol_PlotLines];

    // Set our new color
    ImGui::GetStyle().Colors[ImGuiCol_PlotLines] = plotColor;

    // Draw the plot
    ImGui::PlotLines("##FrameTimes",
                     frameTimes,
                     IM_ARRAYSIZE(frameTimes),
                     frameTimeIndex,
                     "FT Graph",
                     0.0f,
                     33.3f,
                     ImVec2(0, 64));

    // Restore the original plot color
    ImGui::GetStyle().Colors[ImGuiCol_PlotLines] = oldPlotColor;

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