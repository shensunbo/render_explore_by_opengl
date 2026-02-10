#include <iostream>
#include <chrono>
#include <stb_image.h>
#include "gl/gl_headers.h"


// #include "core/shader.h"
// #include "core/camera.h"
#include "core/refactor/VehicleVirCamera.h" 
#include "core/refactor/VehicleMeshInfo.h"
#include "core/refactor/VehicleShader.h"
#include "core/refactor/Skybox.h"
#include "core/refactor/VehicleRenderer.h"
#include "log/mylog.h"

#include "configParser/ConfigParser.h"
#include "platform/Platform.h"

void processInput(platform::Platform &platform, RenderToggles &toggles);
void processCameraInput(const platform::Platform &platform);

void dumpTextureToFile(GLuint texture, int width, int height, const char* filename);

static bool FrameRatemonitorAnd100msTick(void);
// settings
const unsigned int SCR_WIDTH = 1080;
const unsigned int SCR_HEIGHT = 720;

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
int main()
{
    mylog(LogLevel::I, "Starting Refactor");

    platform::Platform platform(SCR_WIDTH, SCR_HEIGHT, "explore render");
    if (!platform.valid()) {
        return -1;
    }
    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);

    RendererConfig config{};
    config.width = SCR_WIDTH;
    config.height = SCR_HEIGHT;
    config.resourceRoot = ""; // Run from repo root or build copy.
    vRender.create(config);
    vRender.ourShader->use();
    glm::mat4 skyboxModel = glm::mat4(1.0f);
    skyboxModel = glm::rotate(skyboxModel, glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    vRender.ourShader->setMat4("cubemapRotateMatrix", skyboxModel);

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, -0.7f, -0.5f)); // Translate to center the asset.
    model = glm::scale(model, glm::vec3(0.0001f));    // Scale the asset down to fit the scene.
    model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    vRender.ourShader->setMat4("model", model);

    glm::mat4 projection = glm::mat4(1.0);
    glm::mat4 view = glm::mat4(1.0);

    CHECK_GLES_STATUS();

    // Optional: draw in wireframe.
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // render loop
    // -----------
    while (!platform.shouldClose())
    {   
        // per-frame time logic
        // --------------------
    auto frameStartTime = std::chrono::high_resolution_clock::now();
    float currentFrame = static_cast<float>(platform.timeSeconds());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        (void)FrameRatemonitorAnd100msTick();

        // input
        // -----
    processInput(platform, toggles);
    processCameraInput(platform);
        // camera.ProcessJump(deltaTime);

        if(camera.updateEvent){
            projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 1000.0f);
            view = camera.GetViewMatrix();
        }

        FrameParams params = buildFrameParams(model, projection, view, toggles);

        vRender.renderFrame(params);

        if (toggles.dumpRes) {
            toggles.dumpRes = false;
        }

        // clear update event
        camera.updateEvent = false;

        CHECK_GLES_STATUS();
        // frame end time stamp
        auto frameEndTime = std::chrono::high_resolution_clock::now();
        auto frameDuration = std::chrono::duration<float, std::milli>(frameEndTime - frameStartTime).count();
        
        auto swapBufStartTime = std::chrono::high_resolution_clock::now();
        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
    platform.swapBuffers();
    platform.pollEvents();

        auto swapBufEndTime = std::chrono::high_resolution_clock::now();
        auto swapBufCostTime = std::chrono::duration<float, std::milli>(swapBufEndTime - swapBufStartTime).count();

        // Optionally print timing information (can be enabled/disabled)
        static bool printTiming = false;
        if (printTiming) {
            std::cout << "Frame Timing (ms):\n"
                    << "  rendering time: " << frameDuration << "\n"
                    << "  swap buffer time: " << swapBufCostTime << "\n"
                    << "------------------------\n";
        }
    }

    return 0;
}



void processInput(platform::Platform &platform, RenderToggles &toggles)
{
    const auto &input = platform.input();

    if (input.isDown(platform::Key::Escape)) {
        platform.requestClose();
    }
    if (input.isDown(platform::Key::I)) {
        toggles.dumpRes = true;
        mylog(LogLevel::I, "Dump frame to file");
    }
    if (input.isDown(platform::Key::F)) {
        toggles.fboEnable = true;
        mylog(LogLevel::I, "FBO enabled");
    }
    if (input.isDown(platform::Key::E)) {
        toggles.fboEnable = false;
        mylog(LogLevel::I, "FBO disabled");
    }
}

void processCameraInput(const platform::Platform &platform)
{
    const auto &input = platform.input();
    if (input.isDown(platform::Key::W))
        camera.ProcessKeyboard(FORWARD, deltaTime);
    else if (input.isDown(platform::Key::S))
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    else if (input.isDown(platform::Key::A))
        camera.ProcessKeyboard(LEFT, deltaTime);
    else if (input.isDown(platform::Key::D))
        camera.ProcessKeyboard(RIGHT, deltaTime);
    else if (input.isDown(platform::Key::T))
        camera.ProcessKeyboard(UPROLL, deltaTime);
    else if (input.isDown(platform::Key::G))
        camera.ProcessKeyboard(DOWNROLL, deltaTime);
    else if (input.isDown(platform::Key::Space) && camera.isOnGround)
        camera.ProcessKeyboard(SPACE, deltaTime);
    else if (input.isDown(platform::Key::R))
        camera.ProcessKeyboard(R, deltaTime);
    else if (input.isDown(platform::Key::M))
        camera.ProcessKeyboard(K1, deltaTime);

    if (!input.leftButton && (input.deltaX != 0.0 || input.deltaY != 0.0)) {
        camera.ProcessMouseMovement(static_cast<float>(input.deltaX), static_cast<float>(input.deltaY));
    }

    if (input.scrollY != 0.0) {
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
        std::cout<< "FPS: " << (int)fps << std::endl;
        startTime = currentTime;
        frameCount = 0;
    }

    if (tickTime >= 0.03f) {
        tickStartTime = currentTime;
        return true;
    }

    return false;
}