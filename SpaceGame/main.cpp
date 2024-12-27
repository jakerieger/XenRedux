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
#include "Graphics/Effects/AntiAliasing.hpp"
#include "Graphics/Effects/Tonemapper.hpp"
#include "Math/Random.inl"
#include "Skybox.hpp"

#include <imgui/imgui.h>

using namespace x;

/// @briefs Returns the directory of this source file + "/Data".
/// Used for accessing data in the source directories without having
/// to copy it to the build dir.
static Filesystem::Path getDataPath();

struct params {
    glm::vec3 albedo                             = glm::vec3(1.0f, 1.0f, 1.0f);
    float metallic                               = 1.0f;
    float roughness                              = 1.0f;
    float ao                                     = 1.0f;
    glm::vec3 sunColor                           = glm::vec3(1.0f, 1.0f, 1.0f);
    glm::vec3 sunDirection                       = glm::vec3(0.0f, 0.0f, 1.0f);
    float sunIntensity                           = 1.0f;
    PBRMaterial* material                        = nullptr;
    u32 tonemapOp                                = 0;  // 0 - ACES, 1 - Reinhard, 2 - Linear
    const char* tonemapNames[3]                  = {"ACES", "Reinhard", "Linear"};
    f32 drawTime                                 = 0.0f;  // ms
    f32 postProcessTime                          = 0.0f;  // ms
    glm::vec3 localLightColors[kMaxPointLights]  = {};
    float localLightIntensities[kMaxPointLights] = {};
} DebugParams;

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
    std::unique_ptr<Skybox> _skybox;
    std::unique_ptr<Graphics::RenderTarget> _renderTarget;
    std::unique_ptr<Graphics::PostProcessQuad> _postProcessQuad;
    std::unique_ptr<Graphics::Tonemapper> _tonemapper;
    std::unique_ptr<Graphics::AntiAliasing> _antiAliasing;
};

SpaceGame::SpaceGame() : IGame("SpaceGame", 1600, 900, true) {
    _sun.setIntensity(0.15f);
    _sun.setColor(glm::vec3(0.9f));
    _sun.setDirection(glm::vec3(-0.577f, -0.04f, -0.577f));
    _camera          = std::make_shared<PerspectiveCamera>(45.f,
                                                  (f32)width / (f32)height,
                                                  0.1f,
                                                  1000.0f,
                                                  glm::vec3(0.0f, 0.0f, 5.0f),
                                                  glm::vec3(0.0f, 0.0f, 0.0f),
                                                  glm::vec3(0.0f, 1.0f, 0.0f));
    _renderTarget    = std::make_unique<Graphics::RenderTarget>(width, height, true);
    _postProcessQuad = std::make_unique<Graphics::PostProcessQuad>();

    _antiAliasing = std::make_unique<Graphics::AntiAliasing>(Graphics::AATechnique::FXAA);
    _antiAliasing->setTextureSize(width, height);
    _antiAliasing->setInputTexture(_renderTarget->getColorTexture());

    _tonemapper = std::make_unique<Graphics::Tonemapper>();
    _tonemapper->setTextureSize(width, height);
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
        pl->setPosition(glm::vec3(getRandomRange(-4.0f, 1.0f),
                                  getRandomRange(-4.0f, 1.0f),
                                  getRandomRange(-4.0f, 1.0f)));
        _localLights.push_back(pl);
    }
}

void SpaceGame::loadContent() {
    const auto skyboxTexture = getDataPath() / "Sky.hdr";
    _skybox                  = std::make_unique<Skybox>(skyboxTexture.toString());

    const auto shaderBallPath = getDataPath() / "ShaderBall.fbx";
    _shaderBall->loadFromFile(shaderBallPath.toString());
    DebugParams.material = _shaderBall->getMaterial<PBRMaterial>();
    DebugParams.material->setMetallic(1.0f);
    DebugParams.material->setRoughness(0.3f);
    DebugParams.material->setAlbedo(glm::vec3(1.0f, 1.0f, 0.28f));

    // Initialize debug values with the shader ball's material properties.
    // From here on out, the debug ui will modify these values for the material.
    DebugParams.albedo       = DebugParams.material->getAlbedo();
    DebugParams.metallic     = DebugParams.material->getMetallic();
    DebugParams.roughness    = DebugParams.material->getRoughness();
    DebugParams.ao           = DebugParams.material->getAO();
    DebugParams.sunColor     = _sun.getColor();
    DebugParams.sunDirection = _sun.getDirection();
    DebugParams.sunIntensity = _sun.getIntensity();
    for (int i = 0; i < _localLights.size(); ++i) {
        const auto light                     = _localLights[i];
        DebugParams.localLightColors[i]      = light->getColor();
        DebugParams.localLightIntensities[i] = light->getIntensity();
    }

    const auto groundPlanePath = getDataPath() / "GroundPlane.glb";
    _groundPlane->loadFromFile(groundPlanePath.toString());
    _groundPlane->getMaterial<PBRMaterial>()->setMetallic(0.0f);
    _groundPlane->getMaterial<PBRMaterial>()->setRoughness(0.3f);
    _groundPlane->getMaterial<PBRMaterial>()->setAlbedo(glm::vec3(0.6f, 0.6f, 0.6f));
    _groundPlane->getTransform().setScale(glm::vec3(100.0f));

    // Register objects that need to be resized when our framebuffer size changes.
    _context->registerVolatile(_camera.get());
    _context->registerVolatile(_renderTarget.get());
    _context->registerVolatile(_tonemapper.get());
    _context->registerVolatile(_antiAliasing.get());
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
    _skybox->update(_clock, _camera);
    _shaderBall->getTransform().rotate(glm::vec3(0.0f, 10.0f * dT, 0.0f));
}

void SpaceGame::draw() {
    const auto drawStart = std::chrono::high_resolution_clock::now();
    _renderTarget->bind();
    Context::clear();  // Clear the render target before drawing
    _skybox->draw();
    _groundPlane->draw(_camera, _sun, _localLights);
    _shaderBall->draw(_camera, _sun, _localLights);
    const auto drawEnd   = std::chrono::high_resolution_clock::now();
    DebugParams.drawTime = std::chrono::duration<f32, std::milli>(drawEnd - drawStart).count();

    _renderTarget->unbind();

    const auto ppStart = std::chrono::high_resolution_clock::now();
    _antiAliasing->apply();
    _tonemapper->setInputTexture(_antiAliasing->getOutputTexture());
    _tonemapper->apply();
    _postProcessQuad->draw(_tonemapper->getOutputTexture());
    const auto ppEnd            = std::chrono::high_resolution_clock::now();
    DebugParams.postProcessTime = std::chrono::duration<f32, std::milli>(ppEnd - ppStart).count();
}

void SpaceGame::configurePipeline() {
    Graphics::Pipeline::setDepthTest(true);
    Graphics::Pipeline::setCullMode(true);
    Graphics::Pipeline::setBlendMode(true);
    Graphics::Pipeline::setEnableHDR(true);
}

void SpaceGame::drawDebugUI() {
    ImGui::Begin("DevTools",
                 nullptr,
                 ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize);

    // Profiler
    {
        ImGui::Text("%.0f fps", _clock->getFrameRate());
        ImGui::Text("%.4f ms", _clock->getFrameTime());
        ImGui::Text("");
        ImGui::Text("Draw Time:");
        ImGui::Text("%.6f ms", DebugParams.drawTime);
        ImGui::Text("");
        ImGui::Text("Post Process Time:");
        ImGui::Text("%.6f ms", DebugParams.postProcessTime);
        ImGui::Text("");
    }

    if (ImGui::CollapsingHeader("Material")) {
        ImGui::DragFloat3("Albedo", &DebugParams.albedo[0], 0.01f, 0.0f, 1.0f);
        DebugParams.material->setAlbedo(DebugParams.albedo);

        ImGui::DragFloat("Metallic", &DebugParams.metallic, 0.01f, 0.0f, 1.0f);
        DebugParams.material->setMetallic(DebugParams.metallic);

        ImGui::DragFloat("Roughness", &DebugParams.roughness, 0.05f, 0.0f, 1.0f);
        DebugParams.material->setRoughness(DebugParams.roughness);

        ImGui::DragFloat("AO", &DebugParams.ao, 0.01f, 0.0f, 1.0f);
        DebugParams.material->setAO(DebugParams.ao);
    }

    if (ImGui::CollapsingHeader("Transform")) {}

    if (ImGui::CollapsingHeader("Camera")) {}

    if (ImGui::CollapsingHeader("Lights")) {
        // Sun
        {
            ImGui::Text("Sun");
            ImGui::Separator();
            ImGui::DragFloat3("Direction", &DebugParams.sunDirection[0], 0.01f, -1.0f, 1.0f);
            ImGui::DragFloat("Intensity", &DebugParams.sunIntensity, 0.01f, 0.0f, 1.0f);
            ImGui::DragFloat3("Color", &DebugParams.sunColor[0], 0.01f, 0.0f, 1.0f);
            _sun.setDirection(DebugParams.sunDirection);
            _sun.setIntensity(DebugParams.sunIntensity);
            _sun.setColor(DebugParams.sunColor);

            ImGui::Separator();
            ImGui::Text("Local Lights");
            for (int i = 0; i < _localLights.size(); i++) {
                //
                const auto light  = _localLights.at(i);
                const auto header = "Point Light [" + std::to_string(i) + "]";
                if (ImGui::CollapsingHeader(header.c_str())) {
                    const auto colorLabel = "Color##PL" + std::to_string(i);
                    ImGui::DragFloat3(colorLabel.c_str(),
                                      (float*)&DebugParams.localLightColors[i],
                                      0.01f,
                                      0.0f,
                                      1.0f);
                    light->setColor(DebugParams.localLightColors[i]);

                    const auto intensityLabel = "Intensity##PL" + std::to_string(i);
                    ImGui::DragFloat(intensityLabel.c_str(),
                                     &DebugParams.localLightIntensities[i],
                                     0.01f,
                                     0.0f,
                                     1.0f);
                    light->setIntensity(DebugParams.localLightIntensities[i]);
                }
            }
        }
    }

    if (ImGui::CollapsingHeader("Post Process")) {
        if (ImGui::BeginCombo("Tonemap Operator",
                              DebugParams.tonemapNames[DebugParams.tonemapOp])) {
            for (int i = 0; i < IM_ARRAYSIZE(DebugParams.tonemapNames); i++) {
                const bool isSelected = (DebugParams.tonemapOp == i);
                if (ImGui::Selectable(DebugParams.tonemapNames[i], isSelected)) {
                    DebugParams.tonemapOp = i;
                }
                if (isSelected) { ImGui::SetItemDefaultFocus(); }
            }

            ImGui::EndCombo();
        }

        // Update tonemap op
        _tonemapper->setTonemapOperator((i32)DebugParams.tonemapOp);
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