#include "renderer_api.h"
#include "gl/gl_headers.h"
#include "core/refactor/VehicleRenderer.h"
#include "log/mylog.h"

RendererAPI::RendererAPI() 
    : vRender(nullptr), width(0), height(0),
      model(glm::mat4(1.0f)),
      skyboxModel(glm::mat4(1.0f)),
      projection(glm::mat4(1.0f)),
      view(glm::mat4(1.0f))
{
}

RendererAPI::~RendererAPI()
{
    deinit();
}

void RendererAPI::init(int w, int h, const std::string& resourcePrefix)
{
    width = w;
    height = h;

    // Create the vehicle renderer instance
    if (vRender == nullptr) {
        vRender = new VehicleRenderer();
    }

    // Configure global OpenGL state
    glEnable(GL_DEPTH_TEST);
#ifndef __ANDROID__
    glEnable(GL_MULTISAMPLE);
#endif

    // Create and initialize the vehicle renderer
    vRender->create(width, height, resourcePrefix);
    vRender->ourShader->use();

    // Setup skybox model matrix with rotation
    skyboxModel = glm::mat4(1.0f);
    skyboxModel = glm::rotate(skyboxModel, glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    vRender->ourShader->setMat4("cubemapRotateMatrix", skyboxModel);

    // Setup model matrix for the vehicle
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, -0.7f, -0.5f)); // translate it down so it's at the center of the scene
    model = glm::scale(model, glm::vec3(0.00007f)); // it's a bit too big for our scene, so scale it down
    model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    vRender->ourShader->setMat4("model", model);

    // Setup projection and view matrices for static rendering
    projection = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 1000.0f);
    view = glm::lookAt(
        glm::vec3(0.0f, 0.0f, 0.9f),   // camera position
        glm::vec3(0.0f, 0.0f, 0.0f),   // look at
        glm::vec3(0.0f, 1.0f, 0.0f)    // up vector
    );

    glm::mat4 mvp = projection * view * model;
    vRender->ourShader->setMat4("uMVP", mvp);

    // Setup view position
    glm::vec3 viewPosition = glm::vec3(0.0f, 0.0f, 0.9f);
    vRender->ourShader->setVec3("viewPosition", viewPosition);

    glViewport(0, 0, width, height);

    mylog(LogLevel::I, "RendererAPI initialized");
}

void RendererAPI::deinit()
{
    if (vRender != nullptr) {
        vRender->destroy();
        delete vRender;
        vRender = nullptr;
    }
    mylog(LogLevel::I, "RendererAPI deinitialized");
}

void RendererAPI::update()
{
    // For static rendering, no updates needed per frame
    // This method can be called if needed in the future for animations
}

void RendererAPI::render()
{
    if (vRender == nullptr) {
        mylog(LogLevel::W, "RendererAPI not initialized");
        return;
    }

    // Clear the screen
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.2f, 0.5f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Render the vehicle
    vRender->ourShader->use();
    vRender->cubemap->ActiveCubeMap();
    vRender->ourShader->setInt("cubemap", vRender->cubemap->GetBindingPoint());
    vRender->draw();

    // Render the skybox
    glm::mat4 skymodel = glm::mat4(1.0f);
    glm::mat4 skyboxView = glm::mat4(glm::mat3(view)); // Remove translation component
    glm::mat4 skyboxMvp = projection * skyboxView * skymodel;
    vRender->cubemap->updateMvpMatrix(skyboxMvp);
    vRender->cubemap->drawSkybox();
}