// Author: Jake Rieger
// Created: 12/30/2024.
//

#include <glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>
#include <glm/glm.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <imgui/imgui.h>
#include <tinyfiledialogs/tinyfiledialogs.h>

#include "Panic.hpp"
#include "Game.hpp"
#include "IBLPreprocessor.hpp"
#include "Filesystem/Filesystem.hpp"
using namespace x::Filesystem;

static Path getDataPath() {
    return Path(__FILE__).parent() / "Data";
}

class IBLGen : public x::IGame {
private:
    std::unique_ptr<x::IBLPreprocessor> _iblPreprocessor;

public:
    IBLGen() : IGame("IBLGen", 1600, 900, false, false) {}

    void loadContent(x::GameState& state) override {}

    void unloadContent() override {}

    void update(x::GameState& state) override {}

    void draw(const x::GameState& state) override {}

    void configurePipeline() override {}

    void drawDebugUI(const x::GameState& state) override {
        ImGui::SetNextWindowPos(ImVec2(4, 4));
        constexpr ImGuiWindowFlags flags = ImGuiWindowFlags_AlwaysAutoResize |
                                           ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove |
                                           ImGuiWindowFlags_NoResize;
        ImGui::Begin("Configuration", nullptr, flags);

        if (ImGui::Button("Load HDR", ImVec2(200, 0))) {
            const char* title     = "Load HDR";
            const char* filters[] = {"*.hdr", "*.exr"};
            const char* filename =
              tinyfd_openFileDialog(title, getDataPath().cStr(), 2, filters, nullptr, 0);
            if (filename) {
                if (_iblPreprocessor) { _iblPreprocessor.reset(); }
                _iblPreprocessor = std::make_unique<x::IBLPreprocessor>(Path(filename));
            }
        }

        ImGui::End();
    }

    void onKeyDown(u16 key) override {}
    void onKeyUp(u16 key) override {}
    void onMouseMove(i32 x, i32 y) override {}
    void onMouseDown(u16 button, i32 x, i32 y) override {}
    void onMouseUp(u16 button, i32 x, i32 y) override {}
};

i32 main() {
    const auto testHdr = getDataPath() / "CloudySky_2k.hdr";
    if (!testHdr.exists()) { Panic("Could not find test data"); }

    IBLGen iblGen;
    iblGen.run();

    return 0;
}