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
    x::IBLPreprocessor::Settings _settings;
    x::IBLTextureHandles _textureHandles;
    str _exportPath;

    ImVec4 colorToImVec4(uint32_t color) {
        f32 a = CAST<f32>((color >> 24) & 0xFF) / 255.0f;
        f32 r = CAST<f32>((color >> 16) & 0xFF) / 255.0f;
        f32 g = CAST<f32>((color >> 8) & 0xFF) / 255.0f;
        f32 b = CAST<f32>(color & 0xFF) / 255.0f;
        return ImVec4(r, g, b, a);
    }

    void setTheme() {
        auto& style          = ImGui::GetStyle();
        style.FrameRounding  = 4.f;
        style.WindowRounding = 4.f;
        ImVec4* colors       = style.Colors;

        colors[ImGuiCol_HeaderHovered] = ImVec4(0.45f, 0.21f, 0.69f, 0.80f);  // Purple
        colors[ImGuiCol_HeaderActive]  = ImVec4(0.46f, 0.22f, 0.70f, 1.00f);  // Darker purple
        colors[ImGuiCol_Header] = ImVec4(0.44f, 0.20f, 0.68f, 0.31f);  // Semi-transparent purple
        // Also update related UI elements that use the accent color
        colors[ImGuiCol_Button]        = ImVec4(0.44f, 0.20f, 0.68f, 0.40f);  // Button color
        colors[ImGuiCol_ButtonHovered] = ImVec4(0.45f, 0.21f, 0.69f, 1.00f);  // Button hover
        colors[ImGuiCol_ButtonActive]  = ImVec4(0.46f, 0.22f, 0.70f, 1.00f);  // Button active
        // Slider and checkbox colors
        colors[ImGuiCol_SliderGrab]       = ImVec4(0.44f, 0.20f, 0.68f, 0.80f);
        colors[ImGuiCol_SliderGrabActive] = ImVec4(0.46f, 0.22f, 0.70f, 1.00f);
        colors[ImGuiCol_CheckMark]        = ImVec4(0.46f, 0.22f, 0.70f, 1.00f);
        // Background colors
        colors[ImGuiCol_WindowBg] = ImVec4(0.10f, 0.10f, 0.13f, 1.00f);
        colors[ImGuiCol_PopupBg]  = ImVec4(0.11f, 0.11f, 0.14f, 0.92f);
        // Text colors
        colors[ImGuiCol_Text]         = ImVec4(0.86f, 0.86f, 0.86f, 1.00f);
        colors[ImGuiCol_TextDisabled] = ImVec4(0.86f, 0.86f, 0.86f, 0.58f);
        // Border and separator colors
        colors[ImGuiCol_Border]       = ImVec4(0.44f, 0.20f, 0.68f, 0.29f);
        colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.24f);
        colors[ImGuiCol_Separator]    = ImVec4(0.44f, 0.20f, 0.68f, 0.29f);
        // Frame colors (for elements like input fields)
        colors[ImGuiCol_FrameBg]        = ImVec4(0.20f, 0.20f, 0.22f, 0.54f);
        colors[ImGuiCol_FrameBgHovered] = ImVec4(0.44f, 0.20f, 0.68f, 0.27f);
        colors[ImGuiCol_FrameBgActive]  = ImVec4(0.44f, 0.20f, 0.68f, 0.39f);
        // Scrollbar colors
        colors[ImGuiCol_ScrollbarBg]          = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
        colors[ImGuiCol_ScrollbarGrab]        = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
        colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
        colors[ImGuiCol_ScrollbarGrabActive]  = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
    }

public:
    IBLGen() : IGame("IBLGen", 1280, 720, false, false) {}

    void loadContent(x::GameState& state) override {
        // setTheme();
    }

    void unloadContent() override {}

    void update(x::GameState& state) override {}

    void draw(const x::GameState& state) override {
        if (_textureHandles.valid()) {
            // Draw skybox
        }
    }

    void configurePipeline() override {}

    void drawDebugUI(const x::GameState& state) override {
        static i32 cubemapSize            = 1024;
        static i32 irradianceSize         = 32;
        static i32 prefilterSize          = 128;
        static i32 brdfSize               = 512;
        static i32 mipLevels              = 5;
        static str selectedHdr            = "";
        static char exportPathBuffer[255] = {'\0'};

        ImGui::SetNextWindowPos(ImVec2(4, 4));
        constexpr i32 flags = ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse |
                              ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize |
                              ImGuiWindowFlags_NoTitleBar;
        ImGui::Begin("Configuration", nullptr, flags);
        auto labelWidth  = ImGui::CalcTextSize("Prefiltered Mip Levels:").x;
        auto windowWidth = ImGui::GetContentRegionAvail().x;
        auto inputWidth  = 100.f;
        auto spacing     = 20.f;
        auto buttonWidth = labelWidth + spacing + inputWidth;
        f32 lineSpacing =
          labelWidth + spacing + 8.f;  // idk where this comes from but it's necessary

        if (ImGui::Button("Load HDR", ImVec2(buttonWidth, 0))) {
            const char* title     = "Load HDR";
            const char* filters[] = {"*.hdr", "*.exr"};
            const char* filename =
              tinyfd_openFileDialog(title, getDataPath().cStr(), 2, filters, nullptr, 0);
            if (filename) {
                selectedHdr = filename;
                glfwSetWindowTitle(_window, ("IBLGen | " + selectedHdr).c_str());
                if (_iblPreprocessor) { _iblPreprocessor.reset(); }
                _iblPreprocessor = std::make_unique<x::IBLPreprocessor>(Path(filename));
            }
        }
        ImGui::Spacing();
        ImGui::Spacing();
        ImGui::Text("Settings");
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        ImGui::Text("Environment Map Size:");
        ImGui::SameLine(lineSpacing);
        ImGui::SetNextItemWidth(inputWidth);
        ImGui::InputInt("##cubemap", &cubemapSize);

        ImGui::Text("Irradiance Map Size:");
        ImGui::SameLine(lineSpacing);
        ImGui::SetNextItemWidth(inputWidth);
        ImGui::InputInt("##irradiance", &irradianceSize);

        ImGui::Text("Prefiltered Map Size:");
        ImGui::SameLine(lineSpacing);
        ImGui::SetNextItemWidth(inputWidth);
        ImGui::InputInt("##prefilter", &prefilterSize);

        ImGui::Text("Prefiltered Mip Levels:");
        ImGui::SameLine(lineSpacing);
        ImGui::SetNextItemWidth(inputWidth);
        ImGui::InputInt("##mips", &mipLevels);

        ImGui::Text("BRDF LUT Size:");
        ImGui::SameLine(lineSpacing);
        ImGui::SetNextItemWidth(inputWidth);
        ImGui::InputInt("##brdflut", &brdfSize);

        ImGui::Text("Export To:");
        ImGui::SetNextItemWidth(windowWidth - 36.f);
        ImGui::InputText("##exportPath", exportPathBuffer, 255);
        ImGui::SameLine();
        if (ImGui::Button("...", ImVec2(28.f, 0))) {
            const char* dir = tinyfd_selectFolderDialog("Export to...", getDataPath().cStr());
            if (dir) { std::strcpy(exportPathBuffer, dir); }
        }

        _settings.cubemapSize        = cubemapSize;
        _settings.irradianceSize     = irradianceSize;
        _settings.prefilterSize      = prefilterSize;
        _settings.prefilterMipLevels = mipLevels;
        _settings.brdfLUTSize        = brdfSize;

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();
        ImGui::Spacing();

        ImGui::BeginDisabled(selectedHdr.empty());
        if (ImGui::Button("Generate IBL", ImVec2(buttonWidth, 0))) {
            if (_iblPreprocessor) {
                _textureHandles = _iblPreprocessor->generateIBLTextures(_settings);
            }
        }
        ImGui::EndDisabled();

        ImGui::Spacing();

        ImGui::BeginDisabled(!_textureHandles.valid());
        if (ImGui::Button("Export", ImVec2(buttonWidth, 0))) {
            if (!x::IBLPreprocessor::exportIBLTextures(_textureHandles, _exportPath)) {
                std::ignore =
                  tinyfd_messageBox("IBLGen - Export",
                                    "An error occurred during export. See log for more details.",
                                    "ok",
                                    "error",
                                    0);
            }
        }
        ImGui::EndDisabled();

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