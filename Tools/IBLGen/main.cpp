// Author: Jake Rieger
// Created: 12/30/2024.
//

#include <glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <imgui/imgui.h>
#include <tinyfiledialogs/tinyfiledialogs.h>

#include "Panic.hpp"
#include "Game.hpp"
#include "IBLPreprocessor.hpp"
#include "Filesystem/Filesystem.hpp"

#include <Graphics/Shaders/Include/Skybox_FS.h>
#include <Graphics/Shaders/Include/Skybox_VS.h>
using namespace x::Filesystem;

static Path getDataPath() {
    return Path(__FILE__).parent() / "Data";
}

class IBLGen final : public x::IGame {
private:
    std::unique_ptr<x::IBLPreprocessor> _iblPreprocessor;
    x::IBLPreprocessor::Settings _settings;
    x::IBLTextureHandles _textureHandles;
    str _exportPath;
    u32 _skyboxVAO = 0;
    u32 _skyboxVBO = 0;
    std::shared_ptr<x::Graphics::ShaderProgram> _skyboxShader;
    const glm::mat4 _projection = glm::perspective(glm::radians(45.f), 16.f / 9.f, 0.1f, 100.f);
    const glm::mat4 _view =
      glm::lookAt(glm::vec3(0, 0, 5.0), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));

    static ImVec4 colorToImVec4(const u32 color) {
        f32 a = CAST<f32>((color >> 24) & 0xFF) / 255.0f;
        f32 r = CAST<f32>((color >> 16) & 0xFF) / 255.0f;
        f32 g = CAST<f32>((color >> 8) & 0xFF) / 255.0f;
        f32 b = CAST<f32>(color & 0xFF) / 255.0f;
        return {r, g, b, a};
    }

    static void setTheme() {
        const auto fontPath = getDataPath() / "inter.ttf";
        ImGuiIO& io         = ImGui::GetIO();
        io.Fonts->AddFontFromFileTTF(fontPath.cStr(), 14);

        auto& style          = ImGui::GetStyle();
        style.FrameRounding  = 4.f;
        style.WindowRounding = 4.f;
        ImVec4* colors       = style.Colors;

        const auto accent     = colorToImVec4(0xFF585858);
        const auto accentDark = ImVec4(accent.x, accent.y, accent.z, 0.6f);
        const auto background = colorToImVec4(0xff1b1b1b);
        const auto input      = colorToImVec4(0xff303030);

        colors[ImGuiCol_HeaderHovered] = accent;  // Purple
        colors[ImGuiCol_HeaderActive]  = accent;  // Darker purple
        colors[ImGuiCol_Header]        = accent;  // Semi-transparent purple
        // Also update related UI elements that use the accent color
        colors[ImGuiCol_Button]        = accent;      // Button color
        colors[ImGuiCol_ButtonHovered] = accentDark;  // Button hover
        colors[ImGuiCol_ButtonActive]  = accentDark;  // Button active
        // Slider and checkbox colors
        colors[ImGuiCol_SliderGrab]       = accent;
        colors[ImGuiCol_SliderGrabActive] = accentDark;
        colors[ImGuiCol_CheckMark]        = accent;
        // Background colors
        colors[ImGuiCol_WindowBg] = background;
        colors[ImGuiCol_PopupBg]  = background;
        // Text colors
        colors[ImGuiCol_Text]         = ImVec4(0.86f, 0.86f, 0.86f, 1.00f);
        colors[ImGuiCol_TextDisabled] = ImVec4(0.86f, 0.86f, 0.86f, 0.58f);
        // Border and separator colors
        colors[ImGuiCol_Border]       = ImVec4(0.44f, 0.20f, 0.68f, 0.29f);
        colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.24f);
        colors[ImGuiCol_Separator]    = ImVec4(0.44f, 0.20f, 0.68f, 0.29f);
        // Frame colors (for elements like input fields)
        colors[ImGuiCol_FrameBg]        = input;
        colors[ImGuiCol_FrameBgHovered] = input;
        colors[ImGuiCol_FrameBgActive]  = input;
        // Scrollbar colors
        colors[ImGuiCol_ScrollbarBg]          = input;
        colors[ImGuiCol_ScrollbarGrab]        = accent;
        colors[ImGuiCol_ScrollbarGrabHovered] = accentDark;
        colors[ImGuiCol_ScrollbarGrabActive]  = accentDark;
    }

public:
    IBLGen()
        : IGame("IBLGen", 1280, 720, false, false),
          _settings(x::IBLPreprocessor::Settings::defaultSettings()) {}

    void loadContent(x::GameState& state) override {
        setTheme();

        glGenVertexArrays(1, &_skyboxVAO);
        glGenBuffers(1, &_skyboxVBO);
        CHECK_GL_ERROR();
        glBindVertexArray(_skyboxVAO);
        glBindBuffer(GL_ARRAY_BUFFER, _skyboxVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(kCubeVertices), kCubeVertices, GL_STATIC_DRAW);
        CHECK_GL_ERROR();
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(f32), (void*)0);
        CHECK_GL_ERROR();
        glBindVertexArray(0);

        _skyboxShader =
          x::ShaderManager::instance().getShaderProgram(Skybox_VS_Source, Skybox_FS_Source);
        if (!_skyboxShader) { Panic("Failed to load skybox shaders"); }
    }

    void unloadContent() override {
        _skyboxShader.reset();
        glDeleteVertexArrays(1, &_skyboxVAO);
        glDeleteBuffers(1, &_skyboxVBO);
    }

    void update(x::GameState& state) override {}

    void draw(const x::GameState& state) override {
        if (_textureHandles.valid()) {
            glViewport(0, 0, 1280, 720);
            // glBindFramebuffer(GL_FRAMEBUFFER, 0);
            // glBindRenderbuffer(GL_RENDERBUFFER, 0);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            // Draw skybox
            _skyboxShader->use();
            _skyboxShader->setInt("uSkybox", 0);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_CUBE_MAP, _textureHandles.envCubemap);
            CHECK_GL_ERROR();
            // camera matrices
            const auto viewNoTranslation =
              glm::mat4(glm::mat3(_view));  // removes translation component
            const auto vp = _projection * viewNoTranslation;
            _skyboxShader->setMat4("uVP", vp);
            glBindVertexArray(_skyboxVAO);
            glBindBuffer(GL_ARRAY_BUFFER, _skyboxVBO);
            glDrawArrays(GL_TRIANGLES, 0, 36);
            CHECK_GL_ERROR();

            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindVertexArray(0);
            CHECK_GL_ERROR();
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