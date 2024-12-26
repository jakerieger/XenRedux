// Author: Jake Rieger
// Created: 12/25/2024.
//

#include "Game.hpp"
#include "Model.hpp"
#include "PBRMaterial.hpp"
#include "PerspectiveCamera.hpp"
#include "PointLight.hpp"
#include "Filesystem/Filesystem.hpp"
#include "Graphics/Pipeline.hpp"
#include "Graphics/PostProcessQuad.hpp"
#include "Graphics/RenderTarget.hpp"
#include "Graphics/Effects/Tonemapper.hpp"
#include "Math/Random.inl"

#include <imgui/imgui.h>

using namespace x;

/// @briefs Returns the directory of this source file + "/Data".
static Filesystem::Path getDataPath();

class SpaceGame final : public IGame {
public:
    SpaceGame();

    void loadContent() override;
    void unloadContent() override;
    void update() override;
    void draw() override;
    void configurePipeline() override;
    void drawDebugUI() override;

private:
    DirectionalLight _sun;
    std::vector<std::shared_ptr<ILight>> _localLights;
    std::shared_ptr<PerspectiveCamera> _camera;
    std::unique_ptr<Model> _shaderBall;
    std::unique_ptr<Model> _groundPlane;
    std::unique_ptr<Graphics::RenderTarget> _renderTarget;
    std::unique_ptr<Graphics::PostProcessQuad> _postProcessQuad;
    std::unique_ptr<Graphics::TonemapperEffect> _tonemapper;

    // Debug UI vars
    glm::vec3 _albedo            = glm::vec3(1.0f, 1.0f, 1.0f);
    float _metallic              = 1.0f;
    float _roughness             = 1.0f;
    float _ao                    = 1.0f;
    glm::vec3 _sunColor          = glm::vec3(1.0f, 1.0f, 1.0f);
    glm::vec3 _sunDirection      = glm::vec3(0.0f, 0.0f, 1.0f);
    float _sunIntensity          = 1.0f;
    PBRMaterial* _material       = nullptr;
    u32 _tonemapOp               = 0;  // 0 - ACES, 1 - Reinhard, 2 - Linear
    const char* _tonemapNames[3] = {"ACES", "Reinhard", "Linear"};
    f32 _drawTime                = 0.0f;  // ms
    f32 _postProcessTime         = 0.0f;  // ms
};

SpaceGame::SpaceGame() : IGame("SpaceGame", 1600, 900, true) {
    _sun.setIntensity(0.15f);
    _sun.setColor(glm::vec3(0.9f));
    _sun.setDirection(glm::vec3(-0.577f, -0.04f, -0.577f));
    _camera          = std::make_shared<PerspectiveCamera>(45.f,
                                                  (f32)width / (f32)height,
                                                  0.1f,
                                                  100.0f,
                                                  glm::vec3(0.0f, 0.0f, 5.0f),
                                                  glm::vec3(0.0f, 0.0f, 0.0f),
                                                  glm::vec3(0.0f, 1.0f, 0.0f));
    _renderTarget    = std::make_unique<Graphics::RenderTarget>(width, height, true);
    _postProcessQuad = std::make_unique<Graphics::PostProcessQuad>();
    _tonemapper      = std::make_unique<Graphics::TonemapperEffect>();
    _tonemapper->setTextureSize(width, height);
    _tonemapper->setInputTexture(_renderTarget->getColorTexture());
    _tonemapper->setTonemapOperator(0);
    _shaderBall  = std::make_unique<Model>();
    _groundPlane = std::make_unique<Model>();

    // Create one hundred point lights
    for (int i = 0; i < 4; ++i) {
        using namespace x::Math::Random;

        const auto pl = std::make_shared<PointLight>();
        pl->setIndex(i);
        pl->setColor(glm::vec3(getRandomRange(0.0f, 1.f),
                               getRandomRange(0.0f, 1.f),
                               getRandomRange(0.0f, 1.f)));
        pl->setIntensity(getRandomRange(0.3f, 1.f));
        pl->setRadius(getRandomRange(45.0f, 90.0f));
        pl->setPosition(glm::vec3(getRandomRange(-4.0f, 4.0f),
                                  getRandomRange(-4.0f, 4.0f),
                                  getRandomRange(-4.0f, 4.0f)));
        _localLights.push_back(pl);
    }
}

void SpaceGame::loadContent() {
    const auto shaderBallPath = getDataPath() / "ShaderBall.fbx";
    _shaderBall->loadFromFile(shaderBallPath.toString());
    _material = _shaderBall->getMaterial<PBRMaterial>();
    _material->setMetallic(1.0f);
    _material->setRoughness(0.3f);
    _material->setAlbedo(glm::vec3(1.0f, 1.0f, 0.28f));

    // Initialize debug values with the shader ball's material properties.
    // From here on out, the debug ui will modify these values for the material.
    _albedo       = _material->getAlbedo();
    _metallic     = _material->getMetallic();
    _roughness    = _material->getRoughness();
    _ao           = _material->getAO();
    _sunColor     = _sun.getColor();
    _sunDirection = _sun.getDirection();
    _sunIntensity = _sun.getIntensity();

    const auto groundPlanePath = getDataPath() / "GroundPlane.glb";
    _groundPlane->loadFromFile(groundPlanePath.toString());
    _groundPlane->getMaterial<PBRMaterial>()->setMetallic(0.0f);
    _groundPlane->getMaterial<PBRMaterial>()->setRoughness(0.5f);
    _groundPlane->getMaterial<PBRMaterial>()->setAlbedo(glm::vec3(0.6f, 0.6f, 0.6f));
    _groundPlane->getTransform().setScale(glm::vec3(100.0f));

    // Register objects that need to be resized when our framebuffer size changes.
    _context->registerVolatile(_camera.get());
    _context->registerVolatile(_renderTarget.get());
    _context->registerVolatile(_tonemapper.get());
}

void SpaceGame::unloadContent() {
    _shaderBall.reset();
    _groundPlane.reset();
    _renderTarget.reset();
    _tonemapper.reset();
}

void SpaceGame::update() {
    const auto dT = _clock->getDeltaTime();
    _camera->update(_clock);
    _shaderBall->getTransform().rotate(glm::vec3(0.0f, 10.0f * dT, 0.0f));
}

void SpaceGame::draw() {
    const auto drawStart = std::chrono::high_resolution_clock::now();
    _renderTarget->bind();
    _context->clear();  // Clear the render target before drawing
    _groundPlane->draw(_camera, _sun, _localLights);
    _shaderBall->draw(_camera, _sun, _localLights);
    const auto drawEnd = std::chrono::high_resolution_clock::now();
    _drawTime          = std::chrono::duration<f32, std::milli>(drawEnd - drawStart).count();

    _renderTarget->unbind();

    const auto ppStart = std::chrono::high_resolution_clock::now();
    _tonemapper->apply();
    _postProcessQuad->draw(_tonemapper->getOutputTexture());
    const auto ppEnd = std::chrono::high_resolution_clock::now();
    _postProcessTime = std::chrono::duration<f32, std::milli>(ppEnd - ppStart).count();
}

void SpaceGame::configurePipeline() {
    Graphics::Pipeline::setDepthTest(true);
    Graphics::Pipeline::setCullMode(true);
    Graphics::Pipeline::setBlendMode(true);
    Graphics::Pipeline::setEnableHDR(true);
}

void SpaceGame::drawDebugUI() {
    if (!_material) return;

    {
        ImGui::Begin("Profiler",
                     nullptr,
                     ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize);

        ImGui::Text("%.0f fps", _clock->getFrameRate());
        ImGui::Text("%.4f ms", _clock->getFrameTime());
        ImGui::Text("");
        ImGui::Text("Draw Time:");
        ImGui::Text("%.6f ms", _drawTime);
        ImGui::Text("");
        ImGui::Text("Post Process Time:");
        ImGui::Text("%.6f ms", _postProcessTime);

        ImGui::End();
    }

    {
        ImGui::Begin("Material",
                     nullptr,
                     ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize);

        ImGui::DragFloat3("Albedo", &_albedo[0], 0.01f, 0.0f, 1.0f);
        _material->setAlbedo(_albedo);

        ImGui::DragFloat("Metallic", &_metallic, 0.01f, 0.0f, 1.0f);
        _material->setMetallic(_metallic);

        ImGui::DragFloat("Roughness", &_roughness, 0.05f, 0.0f, 1.0f);
        _material->setRoughness(_roughness);

        ImGui::DragFloat("AO", &_ao, 0.01f, 0.0f, 1.0f);
        _material->setAO(_ao);
    }

    {
        ImGui::Begin("Lights",
                     nullptr,
                     ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize);

        ImGui::Text("Sun");
        ImGui::Separator();
        ImGui::DragFloat3("Direction", &_sunDirection[0], 0.01f, -1.0f, 1.0f);
        ImGui::DragFloat("Intensity", &_sunIntensity, 0.01f, 0.0f, 1.0f);
        ImGui::DragFloat3("Color", &_sunColor[0], 0.01f, 0.0f, 1.0f);

        _sun.setDirection(_sunDirection);
        _sun.setIntensity(_sunIntensity);
        _sun.setColor(_sunColor);

        ImGui::End();
    }

    {
        ImGui::Begin("Post Processing",
                     nullptr,
                     ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize);

        if (ImGui::BeginCombo("Tonemap Operator", _tonemapNames[_tonemapOp])) {
            for (int i = 0; i < IM_ARRAYSIZE(_tonemapNames); i++) {
                const bool isSelected = (_tonemapOp == i);
                if (ImGui::Selectable(_tonemapNames[i], isSelected)) { _tonemapOp = i; }
                if (isSelected) { ImGui::SetItemDefaultFocus(); }
            }

            ImGui::EndCombo();
        }

        // Update tonemap op
        _tonemapper->setTonemapOperator(_tonemapOp);

        ImGui::End();
    }

    ImGui::End();
}

int main() {
    SpaceGame game;
    game.run();
    return 0;
}

static Filesystem::Path getDataPath() {
    const Filesystem::Path currentFile(__FILE__);
    const auto dataPath = currentFile.parent() / "Data";
    return dataPath;
}