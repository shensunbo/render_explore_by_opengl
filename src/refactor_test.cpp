#include <iostream>
#include <chrono>
#include <cfloat>
#include <stb_image.h>
#include "gl/gl_headers.h"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <thread>

// #include "core/shader.h"
#include "core/refactor/VehicleVirCamera.h"
#include "core/refactor/VehicleMeshInfo.h"
#include "core/refactor/VehicleShader.h"
#include "core/refactor/Skybox.h"
#include "core/refactor/VehicleRenderer.h"
#include "log/mylog.h"
#include "configParser/ConfigParser.h"
#include "platform/Platform.h"

struct RenderToggles;
void processInput(platform::Platform &platform, RenderToggles &toggles, bool blockKeyboard);
void processCameraInput(const platform::Platform &platform, bool blockMouse, bool blockKeyboard);
static void drawControlWindow(RenderToggles &toggles, float frameDuration);
static void applyRenderToggles(VehicleRenderer &renderer, const RenderToggles &toggles);

void dumpTextureToFile(GLuint texture, int width, int height, const char* filename);

static bool FrameRatemonitorAnd100msTick(void);
// settings
const unsigned int SCR_WIDTH = 1600;
const unsigned int SCR_HEIGHT = 900;
// TODO: Handle callback event routing in a cleaner way.
// Camera state.
VehicleVirCamera camera(glm::vec3(0.0f, 0.0f, 0.9f));
// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;
// static unsigned int glerror = 0;

static VehicleRenderer vRender;
struct RenderToggles {
    bool dumpRes{false};
    bool fboEnable{false};
    bool timing{false};
    float exposure{1.1f};
    bool usePbr{false};
    bool useDiffuse{true};
    bool useSpecular{true};
    bool useNormal{true};
    bool useAO{true};
    bool useRoughness{true};
    bool useMetallic{true};
    bool limitFPS{false};
    int targetFPS{60};
};

static RenderToggles toggles;
static FrameParams buildFrameParams(const glm::mat4& model,
                                    const glm::mat4& projection,
                                    const glm::mat4& view,
                                    const RenderToggles& tgs) {
    FrameParams params{};
    params.projection = projection;
    params.view = view;
    params.model = model;
    glm::mat4 invLook = glm::inverse(view);
    params.eye = glm::vec3(invLook[3]);
    params.enableFbo = tgs.fboEnable;
    params.dumpOnce = tgs.dumpRes;
    return params;
}

static void drawControlWindow(RenderToggles &toggles, float frameDuration) {
    ImGui::SetNextWindowSize(ImVec2(360, 260), ImGuiCond_FirstUseEver);
    if (ImGui::Begin("Controls", nullptr, ImGuiWindowFlags_NoCollapse)) {
        ImGui::TextUnformatted("Quick actions");
        ImGui::Separator();
        if (ImGui::Button("Dump frame", ImVec2(-FLT_MIN, 0))) {
            toggles.dumpRes = true;
        }

        ImGui::Spacing();
        ImGui::TextUnformatted("Render pipeline");
        ImGui::Separator();
        ImGui::Checkbox("Enable FBO", &toggles.fboEnable);
        ImGui::Checkbox("Show timing overlay", &toggles.timing);
    ImGui::Checkbox("Use PBR shading", &toggles.usePbr);
        ImGui::SliderFloat("Exposure", &toggles.exposure, 0.1f, 3.0f, "%.2f");

        ImGui::Spacing();
        ImGui::TextUnformatted("Textures");
        ImGui::Separator();
        if (ImGui::BeginTable("texture_toggles", 2, ImGuiTableFlags_SizingStretchProp)) {
            ImGui::TableNextColumn();
            ImGui::Checkbox("Diffuse", &toggles.useDiffuse);
            ImGui::Checkbox("Normal", &toggles.useNormal);
            ImGui::Checkbox("Roughness", &toggles.useRoughness);

            ImGui::TableNextColumn();
            ImGui::Checkbox("Specular", &toggles.useSpecular);
            ImGui::Checkbox("AO", &toggles.useAO);
            ImGui::Checkbox("Metallic", &toggles.useMetallic);
            ImGui::EndTable();
        }

        ImGui::Spacing();
        ImGui::TextUnformatted("Performance");
        ImGui::Separator();
        ImGui::Text("Frame time: %.2f ms (%.1f FPS)", frameDuration,
                    (frameDuration > 0.0f ? 1000.0f / frameDuration : 0.0f));
        ImGui::Checkbox("Limit FPS", &toggles.limitFPS);
        ImGui::BeginDisabled(!toggles.limitFPS);
        ImGui::SliderInt("Target FPS", &toggles.targetFPS, 10, 240);
        ImGui::EndDisabled();
    }
    ImGui::End();
}

static void applyRenderToggles(VehicleRenderer &renderer, const RenderToggles &toggles) {
    renderer.setPbrEnabled(toggles.usePbr);
    VehicleShader* shader = renderer.activeShader();
    if (shader) {
        shader->use();
        shader->setFloat("exposure", toggles.exposure);
        shader->setBool("enableDiffuseTex", toggles.useDiffuse);
        shader->setBool("enableSpecularTex", toggles.useSpecular);
        shader->setBool("enableNormalTex", toggles.useNormal);
        shader->setBool("enableAOTex", toggles.useAO);
        shader->setBool("enableRoughnessTex", toggles.useRoughness);
        shader->setBool("enableMetallicTex", toggles.useMetallic);
    }
    renderer.setTimingEnabled(toggles.timing);
}

int main()
{
    mylog(LogLevel::I, "Starting Refactor");
    platform::Platform platform(SCR_WIDTH, SCR_HEIGHT, "explore render");
    if (!platform.valid()) {
        return -1;
    }
    // configure global opengl state
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_FRAMEBUFFER_SRGB);

    RendererConfig config{};
    config.width = SCR_WIDTH;
    config.height = SCR_HEIGHT;
    config.resourceRoot = ""; // Run from repo root or build copy.
    vRender.create(config);

    glm::mat4 skyboxModel = glm::mat4(1.0f);
    skyboxModel = glm::rotate(skyboxModel, glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    vRender.setCubemapRotation(skyboxModel);

    // ImGui initialization
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    const char* glsl_version = "#version 330";
    ImGui_ImplGlfw_InitForOpenGL(platform.rawWindow(), true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, -0.7f, -0.5f)); // Translate to center the asset.
    model = glm::scale(model, glm::vec3(0.0001f));                // Scale the asset down to fit the scene.
    model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    // Model matrix is applied per frame via FrameParams

    glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom),
                                            static_cast<float>(platform.width()) / static_cast<float>(platform.height()),
                                            0.1f, 1000.0f);
    glm::mat4 view = camera.GetViewMatrix();

    CHECK_GLES_STATUS();

    // Optional: draw in wireframe.
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // render loop
    float frameDuration = 0.0f;          // Duration of last completed frame (includes any sleep)
    while (!platform.shouldClose()) {
        auto frameStartTime = std::chrono::high_resolution_clock::now();

        float currentFrame = static_cast<float>(platform.timeSeconds());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        drawControlWindow(toggles, frameDuration);

        const ImGuiIO& frameIo = ImGui::GetIO();
        processInput(platform, toggles, frameIo.WantCaptureKeyboard);
        processCameraInput(platform, frameIo.WantCaptureMouse, frameIo.WantCaptureKeyboard);

        int newW = platform.width();
        int newH = platform.height();
        if (platform.consumeResize(newW, newH)) {
            vRender.resize(newW, newH);
        }

        projection = glm::perspective(glm::radians(camera.Zoom),
                                      static_cast<float>(platform.width()) / static_cast<float>(platform.height()),
                                      0.1f, 1000.0f);
        view = camera.GetViewMatrix();

        FrameParams params = buildFrameParams(model, projection, view, toggles);
        applyRenderToggles(vRender, toggles);
        vRender.renderFrame(params);

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        if (toggles.dumpRes) {
            toggles.dumpRes = false;
        }

        camera.updateEvent = false;

        CHECK_GLES_STATUS();

        auto swapBufStartTime = std::chrono::high_resolution_clock::now();
        platform.swapBuffers();
        platform.pollEvents();
        auto swapBufEndTime = std::chrono::high_resolution_clock::now();
        auto swapBufCostTime = std::chrono::duration<float, std::milli>(swapBufEndTime - swapBufStartTime).count();

        // Frame limiter (sleep is included in measured frameDuration)
        if (toggles.limitFPS && toggles.targetFPS > 0) {
            float minFrameTime = 1000.0f / toggles.targetFPS;
            float sleepTime = minFrameTime - std::chrono::duration<float, std::milli>(swapBufEndTime - frameStartTime).count();
            if (sleepTime > 0.0f) {
                std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(sleepTime)));
            }
        }

        auto frameEndTime = std::chrono::high_resolution_clock::now();
        frameDuration = std::chrono::duration<float, std::milli>(frameEndTime - frameStartTime).count();
        (void)FrameRatemonitorAnd100msTick();
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    return 0;
}

void processInput(platform::Platform &platform, RenderToggles &toggles, bool blockKeyboard)
{
    if (blockKeyboard) return;
    const auto &input = platform.input();

    if (input.isDown(platform::Key::Escape)) {
        platform.requestClose();
    }
    if (input.isPressed(platform::Key::I)) {
        toggles.dumpRes = true;
        mylog(LogLevel::I, "Dump frame to file");
    }
    if (input.isPressed(platform::Key::F)) {
        toggles.fboEnable = true;
        mylog(LogLevel::I, "FBO enabled");
    }
    if (input.isPressed(platform::Key::E)) {
        toggles.fboEnable = false;
        mylog(LogLevel::I, "FBO disabled");
    }
    if (input.isPressed(platform::Key::P)) {
        toggles.timing = !toggles.timing;
        if (toggles.timing) {
            mylog(LogLevel::I, "Timing enabled");
        } else {
            mylog(LogLevel::I, "Timing disabled");
        }
    }
}

void processCameraInput(const platform::Platform &platform, bool blockMouse, bool blockKeyboard)
{
    if (blockKeyboard && blockMouse) return;
    const auto &input = platform.input();
    if (!blockKeyboard && input.isDown(platform::Key::W))
        camera.ProcessKeyboard(FORWARD, deltaTime);
    else if (!blockKeyboard && input.isDown(platform::Key::S))
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    else if (!blockKeyboard && input.isDown(platform::Key::A))
        camera.ProcessKeyboard(LEFT, deltaTime);
    else if (!blockKeyboard && input.isDown(platform::Key::D))
        camera.ProcessKeyboard(RIGHT, deltaTime);
    else if (!blockKeyboard && input.isDown(platform::Key::T))
        camera.ProcessKeyboard(UPROLL, deltaTime);
    else if (!blockKeyboard && input.isDown(platform::Key::G))
        camera.ProcessKeyboard(DOWNROLL, deltaTime);
    else if (!blockKeyboard && input.isDown(platform::Key::Space) && camera.isOnGround)
        camera.ProcessKeyboard(SPACE, deltaTime);
    else if (!blockKeyboard && input.isDown(platform::Key::R))
        camera.ProcessKeyboard(R, deltaTime);
    else if (!blockKeyboard && input.isDown(platform::Key::M))
        camera.ProcessKeyboard(K1, deltaTime);

    if (!blockMouse && !input.leftButton && (input.deltaX != 0.0 || input.deltaY != 0.0)) {
        camera.ProcessMouseMovement(static_cast<float>(input.deltaX), static_cast<float>(input.deltaY));
    }

    if (!blockMouse && input.scrollY != 0.0) {
        camera.ProcessMouseScroll(static_cast<float>(input.scrollY));
    }
}

static bool FrameRatemonitorAnd100msTick(void) {
    static std::chrono::high_resolution_clock::time_point startTime =
        std::chrono::high_resolution_clock::now();
    static std::chrono::high_resolution_clock::time_point tickStartTime =
        std::chrono::high_resolution_clock::now();
    static int frameCount = 0;

    std::chrono::high_resolution_clock::time_point currentTime =
        std::chrono::high_resolution_clock::now();
    double deltaTime =
        std::chrono::duration<double>(currentTime - startTime).count();
    double tickTime =
        std::chrono::duration<double>(currentTime - tickStartTime).count();

    frameCount++;

    if (deltaTime >= 3) {
        double fps = frameCount / deltaTime;
        mylog(LogLevel::I, "FPS: %d", static_cast<int>(fps));
        startTime = currentTime;
        frameCount = 0;
    }

    if (tickTime >= 0.03f) {
        tickStartTime = currentTime;
        return true;
    }

    return false;
}