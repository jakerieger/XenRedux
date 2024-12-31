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
};

void SpaceGame::loadContent(x::GameState& state) {}

void SpaceGame::unloadContent() {}

void SpaceGame::update(x::GameState& state) {
    _camera.update(_clock);
    state.updateCameraState(_camera.getView(), _camera.getProjection(), _camera.getPosition());
}

void SpaceGame::draw(const x::GameState& state) {}

void SpaceGame::drawDebugUI(const x::GameState& state) {
    auto& cameraState = state.getCameraState();

    ImGui::Begin("Settings");
    ImGui::Text("Camera");
    ImGui::Text("Position: (%.2f, %.2f, %.2f)",
                cameraState.position.x,
                cameraState.position.y,
                cameraState.position.z);
    ImGui::Text("View");
    ImGui::Text("Projection");
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